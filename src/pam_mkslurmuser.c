#include <pwd.h>
#include <security/pam_appl.h>
#include <security/pam_ext.h>
#include <security/pam_modules.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

static char sacctmgr[4096];

int find_slurm_user(const char *pw_name);
int create_slurm_account(const char *pw_name);
int create_slurm_user(const char *pw_name);

int find_slurm_user(const char *pw_name) {
  FILE *fp;
  char cmd[8128];
  char output[8128];
  int ret = 0;

  // Run command
  snprintf(cmd, sizeof(cmd), "%s show user --quiet --noheader --oneliner %s",
           sacctmgr, pw_name);
  syslog(LOG_DEBUG, "%s", cmd);
  fp = popen(cmd, "r");
  if (fp == NULL) {
    fprintf(stderr, "'%s' failed to execute\n", cmd);
    ret = -1;
    goto cleanup;
  }

  // Fetch output
  while (fgets(output, sizeof(output), fp) != NULL) {
    if (strstr(output, pw_name)) {
      ret = 1;
    }
  }

cleanup:
  if (pclose(fp) != 0) {
    fprintf(stderr, "failed to close command '%s'\n", cmd);
  }

  return ret;
}

int create_slurm_account(const char *pw_name) {
  FILE *fp;
  char cmd[8128];
  char output[8128];
  int ret = 0;

  // Run command
  snprintf(cmd, sizeof(cmd), "%s create --immediate --quiet account name=%s",
           sacctmgr, pw_name);
  syslog(LOG_DEBUG, "%s", cmd);
  fp = popen(cmd, "r");
  if (fp == NULL) {
    fprintf(stderr, "'%s' failed to execute\n", cmd);
    ret = -1;
    goto cleanup;
  }

  // Fetch output
  while (fgets(output, sizeof(output), fp) != NULL) {
  }

cleanup:
  if (pclose(fp) != 0) {
    fprintf(stderr, "failed to close command '%s'\n", cmd);
  }

  return ret;
}

int create_slurm_user(const char *pw_name) {
  FILE *fp;
  char cmd[8128];
  char output[8128];
  int ret = 0;

  // Run command
  snprintf(cmd, sizeof(cmd), "%s create --immediate user name=%s account=%s",
           sacctmgr, pw_name, pw_name);
  syslog(LOG_DEBUG, "%s", cmd);
  fp = popen(cmd, "r");
  if (fp == NULL) {
    fprintf(stderr, "'%s' failed to execute\n", cmd);
    ret = -1;
    goto cleanup;
  }

  // Fetch output
  while (fgets(output, sizeof(output), fp) != NULL) {
  }

cleanup:
  if (pclose(fp) != 0) {
    fprintf(stderr, "failed to close command '%s'\n", cmd);
  }

  return ret;
}

PAM_EXTERN int pam_sm_open_session(pam_handle_t *pamh, int flags, int argc,
                                   const char **argv) {
  const struct passwd *pwent;
  const char *user;
  const char *arg;
  int res;

  // Parse plugin parameters
  snprintf(sacctmgr, sizeof(sacctmgr), "%s", "/usr/bin/sacctmgr");
  for (int i = 0; i < argc; i++) {
    if (strncmp("sacctmgr_path=", argv[i], 14) == 0) {
      arg = argv[i] + 14;
      snprintf(sacctmgr, sizeof(sacctmgr), "%s", arg);
    }
  }

  /* Who are we talking about anyway? */
  res = pam_get_user(pamh, &user, NULL);
  if (res != PAM_SUCCESS) return res;

  /* Fetch passwd entry */
  pwent = getpwnam(user);
  if (!pwent || !pwent->pw_name) {
    pam_error(pamh, "User not found in passwd?");
    return PAM_CRED_INSUFFICIENT;
  }

  // Find user
  openlog("pam_mkslurmuser", LOG_PID, LOG_AUTH);
  res = find_slurm_user(pwent->pw_name);
  if (res == -1) {
    pam_error(pamh, "failed to find user");
    return PAM_SERVICE_ERR;
  }

  // If user doesn't exists, create a slurm user.
  if (res == 0) {
    if (create_slurm_account(pwent->pw_name) != 0) {
      pam_error(pamh, "failed to create slurm account");
      return PAM_SERVICE_ERR;
    }
    if (create_slurm_user(pwent->pw_name) != 0) {
      pam_error(pamh, "failed to create slurm user");
      return PAM_SERVICE_ERR;
    }
  }

  return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_close_session(pam_handle_t *pamh, int flags, int argc,
                                    const char **argv) {
  (void)pamh;
  (void)flags;
  (void)argc;
  (void)argv;
  return PAM_SUCCESS;
}

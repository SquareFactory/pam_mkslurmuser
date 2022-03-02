cmake_minimum_required(VERSION 3.9)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                      ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

project(
  pam_mkslurmuser
  VERSION 1.0.0
  DESCRIPTION "PAM module to create missing Slurm User"
  LANGUAGES C)

# Availables options
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INSTALL_PREFIX /lib64/security)
find_package(PAM REQUIRED)

add_compile_options(
  -O3
  -m64
  -std=c99
  -Wall
  -Wextra
  -Werror
  -Wshadow
  -Wformat=2
  -Wpointer-arith
  -Winit-self
  -Wcast-align
  -Wcast-qual
  -Wunreachable-code
  -fno-common
  -Wno-unused-parameter)

file(GLOB_RECURSE sources src/*.c src/*.h)

add_library(pam_mkslurmuser SHARED ${sources})

set_target_properties(pam_mkslurmuser PROPERTIES C_STANDARD 11 PREFIX "")

target_include_directories(pam_mkslurmuser PUBLIC ${SLURM_SRC_DIR})

target_link_libraries(pam_mkslurmuser PRIVATE pam)

install(TARGETS pam_mkslurmuser LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX})

include(package.cmake)

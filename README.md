# pam_mkuserslurm: PAM Module which creates missing Slurm users

## Configuring

Availables options are shown in the `CMakeLists.txt`.

## Compiling

```sh
mkdir -p build
cd build && cmake ..
make -j$(nproc)
# Or with Ninja:
# cd build && cmake .. -G Ninja
# ninja
```

## Installing

```sh
sudo make install  # or sudo ninja install
```

This will install `pam_mkslurmuser.so` in the `/lib64/security` or `/usr/lib/x86_64-linux-gnu/security` directory.

## Using

In the `/etc/pam.d/system-auth`, add:

```conf
session optional pam_mkslurmuser.so sacctmgr_path=/usr/bin/sacctmgr
```

`sacctmgr_path` is optional.

The program will simply use `sacctmgr` to creates a Slurm user and an account with the same name of the UNIX user.

We may plan to use `libslurm.so` directly (maybe).

Feel free to check the source code as it is extremely simple.

## LICENSE

```sh
MIT License

Copyright (c) 2022 Marc Nguyen, and the DeepSquare Association

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

```

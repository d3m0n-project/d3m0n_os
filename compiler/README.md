# d3m0n os - d3c

This folder provides the official platform `[SDK](./sdk/)` and compiler: `d3c`.


## Installation
To install `d3c` simply run these few commands.
```bash
git clone https://github.com/d3m0n-project/d3m0n_os.git
cd d3m0n_os/compiler/

make INSTALL_FOR_EVERYONE=1 install   # for root installation
# or
make INSTALL_FOR_EVERYONE=0 install   # for current user only

# make sure installation directory is included in $PATH environement variable
# then you can run the compiler as you want. e.g:

d3c my_app.cpp -o my_app   # builds your app, and export it as a executable d3m0n application
```

## Uninstallation
To remove `d3c` you only need to run this command:
```bash
make remove
```

Credits 4re5 group - 2026, all rights reserved
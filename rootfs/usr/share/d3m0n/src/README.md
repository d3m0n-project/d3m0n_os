# d3m0n C main software

## Startup steps
for each .d3m0n file in `/usr/share/d3m0n` and subfolder, unzip it into /usr/share/d3m0n/temp/RANDOM_TOKEN

[example .d3m0n app](https://github.com/d3m0n-project/d3m0n_os/tree/main/rootfs/usr/share/d3m0n/apps/test_app/source)

## App handler
Phone loads apps from temp directory created when startup, 

then reads manifest file located at `/usr/share/d3m0n/temp/APP_TOKEN/app`
- Gets app name, description, category, icon (format "key: value")
then reads main layout file (.layout) in `/usr/share/d3m0n/temp/APP_TOKEN/layouts/main.layout` (example [here](https://github.com/d3m0n-project/d3m0n_os/blob/main/rootfs/usr/share/d3m0n/apps/test_app/source/layouts/main.layout))

then execute code (.src) in `/usr/share/d3m0n/temp/APP_TOKEN/src/main.src` (example [here](https://github.com/d3m0n-project/d3m0n_os/blob/main/rootfs/usr/share/d3m0n/apps/test_app/source/src/main.src))
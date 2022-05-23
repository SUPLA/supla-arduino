# Readme

This docker image contain whole toolset required to compile supla-device
for FreeRTOS Linux port and standard Linux variant.

FreeRTOS is located in /supla/FreeRTOS directory.
supla-device is located in /supla/supla-device directory.
By default supla-device latest version is fetched from github during image
build, however if you have your own local copy and you want to modify it, it
will be better to mount your host's directory containing supla-device and
use it instead. In "Run" section there is example how to do it.

There is also /supla/projects directory where you should mount host's directory
which contain project you would like to build (see "Run" section).

# Build

`docker build --rm -f Dockerfile -t supla/supla-device/freertos:local .`

# Run

Please provide your local machine's path to directory with project you would
like to compile:

`docker run --rm -it \
    --mount src=/home/user/my_projects,target=/supla/projects,type=bind \
    supla/supla-device/freertos:local`

If you want to mount local copy of supla-device, use:

`docker run --rm -it \
    --mount src=/home/user/my_projects,target=/supla/projects,type=bind \
    --mount src=/home/user/your_path_to_supla/supla-device,target=/supla/supla-device,type=bind \
    supla/supla-device/freertos:local`



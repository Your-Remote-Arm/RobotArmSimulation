# RobotArmSimulation

## Compilation Instructions

### Linux
Tested on Arch Linux. Other distros may be tested soon.
First, ensure cppzmq, glfw, and glm are installed.
On Arch:
```bash
pacman -S glfw cppzmq glm
```

Once the repo is cloned:
```bash
make init
```
This will install NRA_visionGL and imgui.

To compile and run:
```bash
make run PROGRAM=<arm_sim | arm_controller | arm_file_sink | arm_file_source>
```
This runs from the .bin/\<program> directory. Keep in mind that the first thing the program does is exit 2 directories (i.e. exits the .bin directory). This will be toggleable in a future update.

### Windows
Currently not functioning, pending zeroMQ integration

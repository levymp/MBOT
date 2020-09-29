# MBOT
![alt text](./documentation/img/MBOT.jpeg)

## Contributors
Michael Levy / Hamil Shah / Sam Ansaldo / Xun Fu

## Usage
All of this code is run on a [BeagleBone Blue](https://beagleboard.org/blue) specifically for the University of Michigian Graduate Robotic Systems Laboratory Course.   
All of this code is being used for low level control of the MBOT.

## Documentation
See documentation and videos of Michael's work at [www.robotics.mplevy.com](https://robotics.mplevy.com/). The groups work site is coming soon.

## Michigan Information
Initial stencil forked from [mobilebot repository](https://gitlab.eecs.umich.edu/rob550-f20/mobilebot).   
See (MBOT-RPI)[https://github.com/levymp/MBOT-RPI] repoistory for work on Raspberry Pi for high level control.

## License 
License is MIT License and University of Michigan License. 

### Directoy Information
Directory | Details
------------ | -------------
bin/ | Binaries folder
mobilebot/mobilebot.c/.h | Main setup and threads   
test_motors/test_motors.c/.h | Program to test motor implementation   
meas..params/meas..params.c/.h | Program to measure motor parameters   
common/mb_controller.c/.h | Contoller for manual and autonomous nav
common/mb_defs.h | Define hardware config
common/mb_odometry.c/.h | Odometry and dead reckoning 
lcmtypes/ | lcmtypes for Mobilebot
java/ | java build folder for lcmtypes for lcm-spy
setenv.sh | sets up java PATH variables for lcm-spy (run with: source setenv.sh)

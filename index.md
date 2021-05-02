# Slam-bot

This page details a SLAM (simultaneous localization and mapping) robot that was built as a final project for ECE 4180 at Georgia Tech. To view source code for this project, please click the blue "view on GitHub" button in the upper-right corner of this webpage.

## Project Idea

The intention of this project was to create a simple, cheap, and easily reproducible SLAM robot. This was accomplished using only parts found in the 4180 Mbed kits, which are easily found on Sparkfun. The robot uses a lidar sensor mounted on a cheap brushed DC motor to sense its surroundings, and performs position estimation using an IMU and dead reckoning. Two more brushed DC motors are used for navigation.

## Components List

- Mbed LPC1768
- Raspberry Pi Zero W
- Magician Chassis/Robot kit: https://www.sparkfun.com/products/retired/10825
- Extra DG01D-A130 gearbox/motor/wheel (for lidar platform - the Magician Chassis kit comes with two gearbox/motor/wheel combinations, but a third is required)
- LSM9DS1 9dof IMU (https://www.sparkfun.com/products/13284)
- VL53L0X LIDAR distance sensor (https://www.adafruit.com/product/3317)
- 2x TB6612FNG dual H-bridge (https://www.pololu.com/product/713)

## Schematic

## Video Demo

`![Image](src)`

[Link to video](https://youtube.com)


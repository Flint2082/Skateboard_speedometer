
# Skateboard Speedometer
In this project i will be comparing different ways to measure the speed of a skateboard. I will be using a ESP32 microcontroller. I will compare the following methods: 
- Using reflective sensor on the wheel
- Using a GPS module
- Using a IMU sensor
- Using a smart combination of GPS and IMU

## Reflective sensor
The reflective sensor will be placed on the wheel of the skateboard. The sensor will be able to detect the speed of the wheel by counting the amount of times the signal oscillates. The speed of the skateboard can be calculated by multiplying the speed of the wheel by the diameter of the wheel.

## GPS module
The GPS module will give location data. By calculating the distance between two points and dividing it by the time it took to travel between the two points, the speed can be calculated.

## IMU sensor
The IMU sensor will give acceleration data. By integrating the acceleration data, the speed can be calculated.
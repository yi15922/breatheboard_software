# `breatheboard`: an Arduino fidget toy project
**Note**: the code in this repo is written for the pin layout of an Arduino Nano. It might not work as well with other Arduino models. 

## Overview
The code contains 3 modes of operation for the Arduino. The 4 input pins in the code selects and interacts with all the operation modes. To exit a mode, press the reset button on the Arduino. 

**Note:** This is only the software side of the project, PCB manufacturing files will be added in the near future. 

## Mode 1: Breathe Mode
In this mode, LEDs connected to pins D6-D13 fade on one by one from left to right, then fade off from right to left. This "animation" is designed to assist controlled breathing. Future updates can allow buttons to control the speed of the exercise. 

### Dimming the LEDs
An Arduino uses purely digital outputs. For the LEDs to fade on and off, a PWM signal with variable duty cycle is needed. However, the Arduino Nano does not have 8 PWM pins. To get around this, the function `writeDutyCycle` creates "emulated pwm" by running a loop at 1kHz. The desired pin is driven HIGH and LOW for a specified duration in each 1000µs cycle (e.g. for 10% duty cycle, LED is HIGH for 100µs and LOW for 900µs). This essentially "fakes" PWM on pins that are not PWM enabled by default. A pulsing frequency  of 1kHz is more than sufficient for the human eye to perceive a continuous light. After this is achieved, the duty cycle can be incremented or decremented every few milliseconds to create the illusion of dimming. 

A direct trade-off from this approach is that code execution is blocked as long as an LED is driven in the emulated PWM state. This is because an explicit loop is running to create the dimmed state, and the Arduino does not have multithreading capabilities. The trade-off does not affect this project because no LED needs to be kept in an intermediate state. 

## Mode 2: Fidget Mode
This mode simply allows the user to "scroll" the LED that is current on left and right using buttons connected to the Arduino. If the scrolling reaches the end of the LED row, it wraps back around to the other end. The code keeps track of the currently on LED to determine which is the next LED to turn on. 

A more efficient implementation of this feature can involve a circular doubly linked list. This approach will be tested in a future update. 

## Mode 3: Blinky Light Game
I saw this game in an arcade somewhere, but I don't remember what it was called. Essentially, an LED is turned on at random and the rest of the LEDs scroll back and forth on the row. The user's goal is to press the button exactly when the scrolling reaches the constantly on LED. Once the user succeeds in this, the game pauses, and the user can press the button to continue the game. Each time the user succeeds in "catching" the scrolling LED, the on duration of the LED decreases. In other words, the game gets faster and faster. Once the LED on duration reaches 0, the game is over and the user can press the reset button on the Arduino to exit. 

The implementation of this mode was far less complicated than I had imagined, as I was able to reuse most of the LEd scrolling functions created for the fidget mode. 

## Known Bugs
In the `setup()` code, one might notice that I reference the `LEDPins` array from index 1 to 9 rather than 0 to 8. For some unknown reason, if I don't pad index 0 of the `LEDPins` array with a number and start my iterator from index 1, the first LED does not turn on. I believe this is some form of memory addressing issue in the Arduino processor architecture. 
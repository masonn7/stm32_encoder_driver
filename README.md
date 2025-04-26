# encoder.h
Rotary encoder driver based on hardware timer & software position polling. For use with STM32. Compatibile with RTOS.

## Description
Multiple parallel encoder driver using timer Encoder Mode, implemented as state-machine and data structure - each encoder instance has all the parameters stored separately.

The code is using STM32 HAL functions and macros.

Maximum number of encoder instances may be limited by hardware and memory.

Developed initially for user interface encoders, in combination with a button driver.

Compatible with 8/16/32-bit timers supporting Encoder Mode.

## How to use
* Setup a timer in encoder mode with parameters as required per model of the encoder - standard procedure.
* Init the encoder data structure instance.
* Use functions to modify passed variables by the encoder, poll & read or set position of any encoder instance.

## Functions:
#### Position sensing
* Write absolute pos (HW timer counter)
* Read absolute pos (HW timer counter)
* Read delta pos
* Read last direction
#### Value adjustment
* Modify variable by sensing change in physical position (rotation)
* Set min / max limit
* Setting to loop in wrap condition (inc max->min & dec max<-min), or not

## Authors
Martin Andruska

## Version History
* 1.01
    * Format and comments
* 1.00
    * Working version tested
    * Adjust value mode is now working independently and does not interfere with position sensing (timer counter value).
    * Passed variable is simply incremented or decremented by processor. This method is considered more safe.
    * More effective limit & loop conditions.
    * Very fast response time in direction sensing. Depends on polling interval.

In previous versions a button driver was embedded with encoder instance, for reason of efectiveness the button driver was separated to be used with any buttons in the system.

## To do
* comments
* improve position sensing functions
* add velocity estimation
* add acceleration estimation
## License
This project is licensed under the MIT License - see the LICENSE file for details

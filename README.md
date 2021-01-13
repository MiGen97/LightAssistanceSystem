# LightAssistanceSystem
The purpose of the application is to control the light system of a car. The system is using a light sensor for detecting the environmental light intensity and two infrared sensors for detecting the obstacles from the left of the car, right of the car or ahead ( both infrared sensor detecting an object in the same time ). The light system of the example car is composed of two L.E.D. ( one for the left side and one for the right side ).
The microcontroller that is used in this project is Intel Quark SoC x1000. It have all the needed components to run an operating system ( in our case, a Linux kernel designed for the embedded devices ). The internal components of this microcontroller that will be used in this project are: GPIOs ( for digital inputs and PWM outputs ), ADC ( for analog input ), Timer and Interrupts ( for configuring the PWM outputs in a timely manner ).



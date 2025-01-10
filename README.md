# Project Name: Breathalyzer Lockbox
By: Manasa Srikanth and Christine Choi

The breathalyzer lockbox is an embedded system based on an alcohol sensor that prevents drunk driving. It operates on the STM32L476G MCU and utilizes the SG90 Positional Servo motor, four LEDS, Mikroe MIKROE-1586 Alcohol click, and the built-in analog-to-digital converter (ADC). The motivation for this project is to discourage driving under the influence by preventing users from accessing their key if they do not pass the sobriety test with the alcohol sensor. 

Three LEDS indicate the state of the test: No alcohol detected (GREEN), alcohol detected under legal limit (YELLOW), and alcohol detected exceeds legal limit (RED). When the sensor detects a RED state, the servo motor lowers into a locking position and the box is locked until a YELLOW or GREEN state is entered [5]. A  blinking blue LED indicates when the sensor is collecting samples in the air. 

<a href="https://drive.google.com/file/d/1zTKHhEs0Cthu2mtPGB4oGMNHaoFBl7xz/view?usp=sharing" target="_blank"> Watch the demo here! </a>
![Screenshot 2025-01-09 174345](https://github.com/user-attachments/assets/9e0a45f5-7fd3-47e7-b1ca-753ab30fcd0e)


# FastLED-IR-Controller
A simple arduino sketch for controlling a strip of LEDs with fastLED and IRremote.


This is a small bit of code I wrote to controll the LEDs in my PC since the cheap chinese one I bought did not work.

Used the remote and the IR receiver from that to create this.

IR receiver is connected to PWM PIN on the board, with 5v power and ground also from the board.

The WS2812B leds are on PIN 2. Ground and power of the LED strip come directly from the PSU and not from the arduino (it cant power them).


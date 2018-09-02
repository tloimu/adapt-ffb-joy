# What is Adapt-FFB-Joy Direct Drive #

Adapt-FFB-Joy _Direct Drive_ is a development branch of [Adapt-FFB-Joy](https://github.com/tloimu/adapt-ffb-joy) that interfaces L298N (or similar) H-Bridge for force feedback effects. The proof of concept is done by refitting an Mircosoft Sidewinder Force Feedback Pro (FFP) into fully functioning USB force feedback joystick. It bypasses the FFP's own GamePort MIDI-like protocol and hardware with AVR based solution for calculating and controlling the FFP's motors directly.

An L298N H-bridge module is connected between the Adapt-FFB-Joy and FFP's motors.

The firmware software project is configured to compile for ATmega32U4 with WinAVR-20100110 or newer version.

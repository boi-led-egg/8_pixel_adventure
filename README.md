# 8 Pixel Adventure
An adventure game for arduino using only 8 LEDs and one button.

You need to escape a dungeon you discovered yourself in. The only way is forward, through 255 levels, each is filled with traps.

The game is designed to use only one byte as output, which can be represented as 8 LEDs. That's why some imagination is required.

# Hardware
- Arduino Nano
- 8 LEDs any color
- 8 220 Ohm resistors for LEDs
- 1 push button
- 1 kOhm resistor for the button
- some wires

LEDs are connected to D2-D9 pins through resistors.
Button is connected to pin D13 and ground through a 1k resistor, other leg is connected to 5V.

Here's an example how it can be assembled:
![8 LED and arduino](https://github.com/cgtu/8_pixel_adventure/blob/master/breadboard.jpg)

# Rules
You start as a pixel on the right(or on the left, if assembled incorrectly). Each level has one or more trap -- blinking pixels. Each button press moves you forward. However, if there's a trap on the next cell you either jump over it, if it's open, or... die. A trap is open if an LED is off, and closed if it's on. If there are several adjasent traps, you jump over all of them, that's why all of them must be open when you push the button.
If/when you loose, the screen will show position you died at, then(after a button press) your level encoded in binary(that's a limitation of only having 8 pixels). And then you start a new game.
Good luck to see what happens when you finish all 255 levels.

# Gameplay
Here's a recording of a gameplay: https://youtu.be/8PoZcoDHj3E

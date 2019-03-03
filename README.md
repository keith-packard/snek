# Snek — a tiny python-inspired language for embedded computing

![Snek picture](snek.svg)

Snek is a tiny embeddable language targeting processors with only a
few kB of flash and ram. Think of something that would have been
running BASIC years ago and you'll have the idea. These processors are
too small to run [MicroPython](https://micropython.org/).

## Snek Capabilities

Snek borrows semantics and syntax from python, but only provides a
tiny subset of that large language. The goal is to have Snek programs
able to run in a full Python (version 3) implementation so that any
knowledge gained in learning Snek will transfer directly to learning
Python.

### Datatypes

To keep things simple, Snek does not support Python objects at
all. Inspired by BASIC, Snek provides only a single numeric data
type, 32-bit floats. In addition, there are lists, tuples, strings and
functions. True and False are just syntax for 1 and 0. None doesn't
exist at all.

### Programming constructs

For expressions, snek includes the usual arithmetic (+, -, *, /, %,
//), relational (<=, <, ==, >, >=, !=) and logical (or, and, not)
operators.

Snek implements 'if', 'while' and 'for' control flow, along with
'break', 'continue' and 'return'.

### Functions

You can define new functions in Snek; parameters and local variables
have lexical scope, and you can refer to global variables with the
'global' statement (just like python).

## GPIO interface

Snek targets tiny embedded computers, especially for use in
educational environments. Instead of using an Arduino-style of GPIO
control, with precise management of each pin configuration, Snek uses
a simpler interface derived from Lego Logo. Pins are implicitly
switched from input to output mode as the user operates on them. Pin
selection is separate from pin operation so that each function takes
either one or zero parameters.

For motor controls which require two pins (one for power control and one
for direction), the Snek GPIO API uses a tuple containing both pins
allowing the user to declare a single variable containing both
numbers. There are a couple of operations which manipulate the
direction pin while the remaining operations work on the power pin.

### Pin Numbering

On the Arduino, Snek uses the traditional numbers digital
pins (0-13). For the analog pins, Snek uses the 'alternate' numbers
14-19 so that there is no overlap with the digital pin numbers.

### Input and Output values

To unify values across the digital and analog modes, Snek always uses
values in the range of 0-1 for input and output. Digital outputs are
turned off for a value of 0 and on for any non-zero value. Outputs
capable of PWM operation will provide PWM output for anything between
0 and 1.

Specifying an output value of less than zero is the same as specifying
a value of zero. Similarly, values greater than one are the same as
one.

When reading from the analog pins (14-19), the ADC unit is always
used, so values will range from 0-1 depending on the precise value.

Digital inputs are always configured in ”pull-up” mode so any simple
switches should be connected between the pin and ground to read as 0
when closed and 1 when open. Switches connected to analog inputs
would require an external pull-up (or pull-down) resistor.

### GPIO pin selection

 * talkto(pin) — specifies the pin number(s) for output operations. If
   ”pin” is a tuple, the first value specifies the current output pin
   and the second value specifies the current direction
   pin. Otherwise, both current output and current direction pins are
   set to ”pin”.

 * listento(pin) — specifies the pin number for input
   operations. Input operations on the analog pins always use the
   ADC. Input operations on digital pins always select pull-up mode.

### Output operations

 * setpower(power) — specifies the power level for the current output
   pin. If the pin is already ”on”, the power level is changed
   immediately. For pins with PWM capability, power levels between 0 and 1
   will enable PWM operation. For pins without PWM capability, a power
   level of 0 will set the pin to low logic level while any other
   value will set the pin to high logic level.

 * on() — mark the current output pin as ”on” and set the power to the
   currently configured power level.

 * off() — mark the current output pin as ”off” and set the pin to low
   logic level.

 * onfor(secs) — perform an on() operation, followed by a delay for
   ”secs” seconds (which can be any floating point value), followed by
   an off() operation.

 * setleft() — set the current direction pin to high logic level.

 * setright() — set the current direction pin to low logic level.

 * stopall() — turn all pins off. This performs an off() operation for
   all pins which are marked as ”on”.

### Input operations

 * read() — return the value of the current input pin. For digital
   pins (0-13), this will be either 0 or 1. For analog pins (14-19),
   this will range from 0 to 1.

## To Do list

I think the basic Snek language is pretty complete at this point, but
there are always improvements that can be made.

 * Documentation. We should be able to crib from existing Python
   documentation where it applies, but it would be good to have a
   comprehensive reference manual for the existing language

 * IDE for embedded computers. I don't think we'll have space on the
   ATMega328P for any source code editing, so some off-line IDE on the
   host seems required.

## Recent Changes

Here's some places that have seen recent work

 * Debian packaging. This includes .desktop files and icons to run
   snekde in a terminal window, and a shell script to install
   snek-duino onto an Arduino.

 * Dictionaries. Implemented using the list code, these can be indexed
   with any immutable type (everything other than lists and
   dictionaries). Making ROM space on Arduino for these required
   significant changes in lola to pack the parse tables even smaller.

 * Using EEPROM on the ATMega328P to store source code. Use the new
   Snek Development Environment, snekde, to edit code on a USB
   connected host. You can save source code into eeprom and it will be
   run at boot time. To fit this in, I had to re-structure the parse
   tables generated by lola, so if you want to build this, make sure
   you update lola before building snek.

 * GPIO interface. This follows the Lego Logo model with global state
   that requires fewer parameters to functions, but more function calls.

 * Port to NuttX. I've added files to the NuttX “app” repository that
   allow inclusion of snek into any NuttX build. As snek uses the GPL
   license, the NuttX project does not want to have even these interface
   files (which are BSD licensed) incorporated into the public
   repository, so I'm hosting them on
   [my machine](https://keithp.com/cgit/apps.git/) and also on
   [github](https://github.com/keith-packard/nuttx-app). This
   repository also includes integration code for ao-scheme, the tiny
   scheme interpreter I built in 2017.

 * Switch from meson to make. Meson is awesome for larger projects,
   but snek is small and targets embedded machines where meson
   support is not nearly as awesome. Snek now 'installs' by copying
   source code and Makefile fragments to a host directory from which
   it can then be compiled into other systems.

 * Two different ”snek” games. ”snek-bsd.py” is transliterated from the BSD
   snake game, ”snek.py” is an implementation of the [classic as seen
   on many mobile platforms](https://en.wikipedia.org/wiki/Snake_(video_game_genre)).

 * Rename. This project started out called 'newt', but a friend
   suggested it might be better named 'snek'.

 * Add named parameters. Now you can use name=value in both formals
   and actuals. There's a bit more work to do here for error
   detection, but I'd like to make it smaller first as the arduino
   binary is up over 32kB again.

 * Refactoring and shrinking the code. This reduced the arduino binary
   to just over 31kB.

 * Port to Arduino Duemilanova, which is a system with only 32kB of
   flash and 2kB of RAM. The whole language is included, but all error
   messages are elided to save ram as the AVR processor cannot easily
   read data from flash and so the compiler places strings in RAM.

 * Custom lexer. This replaces the flex-generated lexer and saves a
   pile of memory.

 * Custom table-driven LL parser. I've updated my LL parser generator,
   lola and have changed snek to use it. The resulting parse tables
   and parser are about 6kB smaller and do not use malloc at all.

 * Port to a microcontroller. I've got Snek running on an STM32L152
   discovery board under AltOS. The whole system takes 35kB of flash
   and 14kB of data. Still have a bit of work to get it onto a stock
   Arduino.

## A complete Snek system

I'd like to allow Snek systems to support local development, instead
of relying on a larger remote system for editing and storing
programs. That means creating a tiny IDE of some kind. I think what I
want is a text-mode system with two panes — an editor pane containing
the active Snek code and an interactive pane where you can type
interactive Snek commands to run. With that and some builtins to
save/load projects the system should be usable through a terminal
emulator running on another host, or through a local text interface.

## Build and Run!

### Dependencies
To build Snek you need these dependencies:

  * [Lola](https://keithp.com/cgit/lola.git/)
  * [gcc-avr](https://ccrma.stanford.edu/~juanig/articles/wiriavrlib/AVR_GCC.html)
  * [avr-libc](https://www.nongnu.org/avr-libc/)
  * [python3](https://www.python.org/)
  * [pyserial](https://github.com/pyserial/)
  * [python curses](https://docs.python.org/3/library/curses.html)

On Debian, you can get everything other than Lola from the main archive:

	# apt install gcc-avr avr-libc python3-serial

To install Lola, download the source and install it;

	$ git clone git://keithp.com/git/lola.git
	$ cd lola && make install

### Building and install

In the source of the project run:

	$ make
	$ make install

### Running on Linux

	$ snek

Then, just enjoy!

### Running on Arduino

Snek takes over the entire Arduino device, without leaving room for
the serial boot loader. Because of this, you will need an AVR programming device, such as a
[USBtiny from Adafruit](https://www.adafruit.com/product/46).

Once snek has been compiled, first reset the fuses on the device to
make the Arduino run Snek instead of attempting to run the
non-existent boot loader. You will only need to set the fuses once per device.

	$ (cd snek-duino && make set-fuse)

Next, flash the Snek interpreter:

	$ (cd snek-duino && make load)

Then, figure out which serial port your Arduino is connected to using a tool provided with pyserial:

	$ python3 -m serial.tools.list_ports
	/dev/ttyUSB0        
	1 ports found

Finally, you can run the snek development environment:

	$ snekde --port /dev/ttyUSB0

The snekde window is split into two parts. The upper 2/3 is a text
editor for source code. The bottom 1/3 lets you interact with the
Arduino over the serial port. The very top line lists functions that
you can invoke by pressing the associated function key:

 * F1 — Device. Connect to a serial port.
 * F2 — Get. Get source code saved to the Arduino eeprom into the editor pane.
 * F3 — Put. Put code from the editor pane into the Arduino eeprom.
 * F4 — Quit. Exit snekde.
 * F5 — Load. Read source code from the file system into the editor pane.
 * F6 — Save. Write source code from the editor pane to the file system.

There are a couple more keybindings which you'll want to know:

 * Page-up/Page-down — Switch between the editor pane and the interaction pane.
 * Ctrl-X/Ctrl-C/Ctrl-V — Usual cut/copy/paste commands.
 * Ctrl-C — In the interaction pane, this interrupts any snek program running on the device. Note that
   this means you don't get a Copy command in the interaction pane.
 * Ctrl-Z — Undo.

Tab auto-indents the current line. Backspace backs up over a tabstop
when appropriate.

### Examples

There are examples provided which work with both Python and Snek.

## Debian Packages

I've packaged lola and snek (for both amd64 and i386) and have
uploaded those to a small apt repository on my home machine. You can
use this archive to install snek yourself using the following instructions:

 * First, add my archive key to your apt system:
   ~~~ 
   # wget -O - https://keithp.com/archive/archive-key | apt-key add -
   ~~~
 * Next, append these lines to /etc/apt/sources.list:
    ~~~
    deb     http://keithp.com/archive unstable/
    deb-src http://keithp.com/archive unstable/
    ~~~
 * Finally, install snek (and snek-bin, if you are running on amd64 or
   i386) on your machine:
    ~~~
    # apt update
    # apt install snek snek-bin
    ~~~
## Contributions

I'd love to receive more contributions in the form of patches,
suggestions and bug reports. Please feel free to send mail or use the
github process. I've created a mailing list for collaboration; 
you'll need to subscribe to be able to post. Subscribe to the [snek
mailing list here](https://keithp.com/mailman/listinfo/snek)

# Snek — a tiny python-inspired language for embedded computing

![Snek picture](snek.svg)

Snek is a tiny embeddable language targeting processors with only a
few kB of flash and ram. Think of something that would have been
running BASIC years ago and you'll have the idea. These processors are
too small to run [MicroPython](https://micropython.org/).

## Documentation

 * The [Snek Home Page](https://sneklang.org) provides more information
   about the language and community.

 * [Snek Manual in HTML format](https://sneklang.org/snek.html)

 * [Snek Manual in PDF format](https://sneklang.org/snek.pdf)

## Downloads

 * Snek is packaged for Debian and is available for the unstable release.

 * Packages for Linux, including older versions of Debian, Mac OS X
   and Windows are available in the
   [Snek Dist directory](http://sneklang.org/dist)

## Supported Hardware

Snek supports analog and digital I/O operations on all of these
boards. Snek also has some support for built-in devices on boards as
described below.

 * [Adafruit Crickit](https://www.adafruit.com/product/3093). This is
   sold as an expansion board for other Adafruit products, but Snek is
   able to run on the local processor. Snek supports the motor
   controllers and Neopixel.

 * [Arduino Duemilanove](https://www.arduino.cc/en/Main/arduinoBoardDuemilanove). 
   
 * [Adafruit Feather M0 Express](https://www.adafruit.com/product/3403).
   Snek supports digital and analog input and output with this device.

 * [Seeed Grove Beginner Kit for Arduino](https://www.seeedstudio.com/Grove-Beginner-Kit-for-Arduino-p-4549.html).
   Snek supports the LED, Buzzer, Light, Sound, OLED Display, Button, Rotary
   Potentiometer and 3-axis Accelerometer.

 * [Adafruit ItsyBitsy 32u4 - 5V](https://www.adafruit.com/product/3677),
   [Adafruit ItsyBitsy 32u4 - 3V](https://www.adafruit.com/product/3675).

 * [Adafruit ItsyBitsy M0 Express](https://www.adafruit.com/product/3727)
 * [Arduino Mega 2560](https://store.arduino.cc/usa/mega-2560-r3)
 * [Adafruit METRO M0 Express](https://www.adafruit.com/product/3505)
 * [Arduino Nano 33 IoT](https://store.arduino.cc/usa/nano-33-iot)
 * [Crowd Supply 1284 Narrow](https://www.crowdsupply.com/pandauino/narrow)

 * [Circuit Playground Express](https://www.adafruit.com/product/3333).
   Snek supports the NeoPixels, sound, light sensor and temperature
   sensor. It does not support the accelerometer yet.

 * [Snekboard](https://www.crowdsupply.com/keith-packard/snekboard).
   This board was designed to run Snek and control Lego Power Function motors.

 * [Crowd Supply µduino](https://www.crowdsupply.com/uduino/uduino).
   This board doesn't have any access to the programming pins, so
   you'd need to get Snek pre-programmed before it was split off of
   the carrier board.

 * [Arduino Uno](https://store.arduino.cc/usa/arduino-uno-rev3). This
   is pretty much the same as a Duemilanove board, but with a
   different USB to serial converter.

 * [Seeeduino XIAO](https://www.seeedstudio.com/Seeeduino-XIAO-Arduino-Microcontroller-SAMD21-Cortex-M0+-p-4426.html).

 * [LEGO® MINDSTORMS® EV3](https://www.lego.com/en-us/product/lego-mindstorms-ev3-31313).
   Snek supports EV3 motors and sensors.

 * [Seeed Grove Beginner Kit](https://www.seeedstudio.com/Grove-Beginner-Kit-for-Arduino-p-4549.html).
   The port includes drivers for everything except the temperature,
   humidity and barometric pressure sensors.

 * [Narrow 1284](https://www.crowdsupply.com/pandauino/narrow).

 * [Arduino Nano Every](https://store.arduino.cc/usa/nano-every).

 * [LilyPad Arduino 328](https://www.sparkfun.com/products/13342).
   This port leaves out dictionaries and slices so that it has space
   for the tone driver.

## To Do list

 * Convert parser from LL to SLR. The hope here is to reduce the
   amount of parse stack space needed due to the large number of
   precedence levels in the language.

## Recent Changes

Here's some places that have seen recent work

 * [LilyPad Arduino 328](https://www.sparkfun.com/products/13342)
   port. This is a round ATmega328p-based board designed for wearable
   projects. This board runs at 8MHz, which can't run the serial port
   at 115.2kbaud, so it runs at 57.6k instead. Thanks to Douglas
   Fraser for this.

 * Support multiple baud rates in snekde, auto-detect baud rate when
   connecting to non-USB based devices.

 * Fix NaN comparisons and make dicts containing NaN keys work.
   [Jake Edge's article, “Revisiting NaNs in Python”](https://lwn.net/Articles/869231/)
   led to the discovery that Snek had several bugs in NaN
   comparisons. Snek now computes the right value for comparisons with
   Nan, as well as permitting NaN to be used as a key in dictionaries.

 * Support explicit serial synchronization using ENQ/ACK so that
   applications sending lots of data do not require OS flow control
   support. With many devices connecting via USB/serial adapters that
   do not provide any flow control, adding explicit flow control to
   the Snek implementation provides a way to make downloading code
   reliable for them.

 * Fixed incorrect precedence between bitwise and (&) and bitwise xor
   (^) operators.

 * Lots of improvements to the EV3 port making it more like
   other Snek devices.

 * [Narrow 1284](https://www.crowdsupply.com/pandauino/narrow) port.
   This is a small board with the ATmega1284 SoC which is like the
   ATmega328 but with much more Flash and RAM which provides way more
   room for Snek to run.

 * [Seeed Grove Beginner Kit](https://www.seeedstudio.com/Grove-Beginner-Kit-for-Arduino-p-4549.html)
   port. This is another ATmega328p based board but with a range of
   devices provided in the kit. Snek supports the LED, Buzzer, Light,
   Sound, OLED Display, Button, Rotary Potentiometer and 3-axis
   Accelerometer.

 * [Seeeduino XIAO](https://www.seeedstudio.com/Seeeduino-XIAO-Arduino-Microcontroller-SAMD21-Cortex-M0+-p-4426.html)
   port. This is a SAMD21-based device on a tiny board with a USB-C
   connector.

 * [Arduino Nano Every](https://store.arduino.cc/usa/nano-every) port.
   This uses the ATmega4809 which has 6kB of RAM, providing much more
   space for Snek programs.

## Build and Run!

If you want to build Snek yourself, you'll need to have the build
tools and other dependencies installed.

### Dependencies
To build Snek you need these dependencies:

  * [Lola](https://keithp.com/cgit/lola.git/)
  * [gawk](https://www.gnu.org/software/gawk/)
  * [gcc-avr](https://ccrma.stanford.edu/~juanig/articles/wiriavrlib/AVR_GCC.html)
  * [avr-libc](https://www.nongnu.org/avr-libc/)
  * [gcc-arm-none-eabi](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)
  * [gcc-riscv64-unknown-elf](https://github.com/sifive/freedom-tools/)
  * [picolibc](https://keithp.com/picolibc/)
  * [python3](https://www.python.org/)
  * [pyserial](https://github.com/pyserial/)
  * [python curses](https://docs.python.org/3/library/curses.html)
  * [readline](https://www.gnu.org/software/readline/)

On Debian unstable, you can get everything from the main archive:

	# apt install lola gcc-avr avr-libc python3-serial gawk \
          gcc-arm-none-eabi gcc-riscv64-unknown-elf libreadline-dev \
	  picolibc-arm-none-eabi picolibc-riscv64-unknown-elf

Debian stable has too old a version of lola, so if you're using that,
you'll need to install lola from the git repository above.

The snek build scripts use the GNU awk implementation, `gawk`,
including a GNU extension, `strtonum`, which is not available in other
awk implementations. It's not critical to compiling the code as it's
only used to print out ROM usage for AVR targets so you can tell how
much space is still available, but if you don't have gawk, you will
get build failures.

### Building and install

Here are some useful options to control the build. Each of these is
run from the top level directory.

	$ make

If you just type 'make', the system will build all of the embedded
binaries and a native binary to run locally.

	$ make install

'make install' will copy all of the built programs to /usr/local

	$ make PREFIX=$HOME/.local

This compiles everything to run from your home directory, instead of
/usr/local.

	$ make PREFIX=$HOME/.local install

Make sure you use the same PREFIX value every time you run make so that
the snek install scripts (which get PREFIX embedded in them) that are
installed know where to find the snek binaries for each device.

## Running on Embedded Devices

Snek is designed to run on small embedded devices with 32kB or more of
flash and 2kB or more of RAM. Snek has been ported to a variety of
embedded devices, providing access to pins as GPIOs as well as some
built-in peripherals. Documentation about the supported boards can be
found in the [Snek Manual](https://sneklang.org/snek.html).

## Running on Linux, Mac OS X Windows

Snek is also built to run natively on Linux, Mac OS X and
Windows. When installed, you'll find it available in the regular
system menu.

## The Mu Editor

[mu](https://codewith.mu/) is an IDE especially designed for new
Python developers. It has support for boards running snek upstream,
but no release has been made with that code yet.

## The Snek Development Environment

Snekde is provided on all three platforms; you'll find it in the
system menu on Linux and Windows and down in your personal
Applications directory on Mac OS X.

The snekde window is split into two parts. The upper 2/3 is a text
editor for source code. The bottom 1/3 lets you interact with the
Snek device over the USB port. The very top line lists functions that
you can invoke by pressing the associated function key:

 * F1 — Device. Connect to a serial port.
 * F2 — Get. Get source code saved to the Snek device eeprom into the editor pane.
 * F3 — Put. Put code from the editor pane into the Snek device eeprom.
 * F4 — Quit. Exit snekde.
 * F5 — Load. Read source code from the file system into the editor pane.
 * F6 — Save. Write source code from the editor pane to the file system.

There are a couple more keybindings which you'll want to know:

 * Page-up/Page-down — Switch between the editor pane and the interaction pane.
 * Ctrl-X/Ctrl-C/Ctrl-V — Usual cut/copy/paste commands.
 * Ctrl-C — In the interaction pane, this interrupts any snek program running on the device. Note that
   this means you don't get a Copy command in the interaction pane.
 * Ctrl-Z — Undo.

Tab auto-indents the current line. Backspace backs up over a tab stop
when appropriate.

## Examples

There are examples provided, some of which work with both Python and
Snek, and some of which show how to use SoC-specific APIs that aren't
available in Python.

## Contributions

I'd love to receive more contributions in the form of patches,
suggestions and bug reports. Please feel free to send mail or use the
github process. I've created a mailing list for collaboration; 
you'll need to subscribe to be able to post. Subscribe to the [snek
mailing list here](https://keithp.com/mailman/listinfo/snek)

## Releases

Here are more specific notes about Snek releases.

### Version 1.8

 * Add 'str' builtin. Just like Python, the str builtin converts any
   value to a string.

 * Fix 'chained' comparisons (e.g. a < b < c). These generated
   incorrect code that left the stack messed up unless all comparisons
   were true.

 * Allow 3 * 'a' as well as 'a' * 3. The code for evaluating
   expressions only permitted the string to be on the left side.

 * Add support in snekde for auto-detecting device baud rate between
   57600 and 115200 baud.

 * Add port to the ATMega 328 based LilyPad, in both the regular size
   as well as the big version which replaces the boot loader to gain
   more functionality.

### Version 1.7

 * Fix NaN comparisons and make dicts containing NaN keys work.
   [Jake Edge's article, “Revisiting NaNs in Python”](https://lwn.net/Articles/869231/)
   led to the discovery that Snek had several bugs in NaN
   comparisons. Snek now computes the right value for comparisons with
   Nan, as well as permitting NaN to be used as a key in dictionaries.

 * Add math functions to nano-every port; the ATmega4809 has plenty of
   flash space for these.

### Version 1.6

 * Support explicit serial synchronization using ENQ/ACK so that
   applications sending lots of data do not require OS flow control
   support. With many devices connecting via USB/serial adapters that
   do not provide any flow control, adding explicit flow control to
   the Snek implementation provides a way to make downloading code
   reliable for them.

 * Fixed incorrect precedence between bitwise and (&) and bitwise xor
   (^) operators.

 * Lots of improvements to the EV3 port making it more like
   other Snek devices.

 * [Narrow 1284](https://www.crowdsupply.com/pandauino/narrow) port.
   This is a small board with the ATmega1284 SoC which is like the
   ATmega328 but with much more Flash and RAM which provides way more
   room for Snek to run.

 * Seeed Grove Beginner Kit port. This is another ATmega328p based board
   but with a range of devices provided in the kit. Snek supports the
   LED, Buzzer, Light, Sound, OLED Display, Button, Rotary Potentiometer
   and 3-axis Accelerometer.

 * Seeeduino XIAO port. This is a SAMD21-based device on a tiny board
   with a USB-C connector.

 * Arduino Nano Every port.  This uses the ATmega4809 which has 6kB of
   RAM, providing much more space for Snek programs.

### Version 1.5

 * Arduino Uno port.  This port, is mostly the same as the Duemilanove
   port but includes replacement firmware for the Atmega 16u2 on the Uno
   board which acts as the USB interface.

 * Handle 'chained' comparison operators (a < b < c) correctly.

 * Add sound output support for the Adafruit Circuit Playground
   Express board.

 * Use Optiboot on the Duemilanove board so that Snek can be replaced
   without needing a separate programming device.

### Version 1.4

 * LEGO EV3 port.
   This port, done by Mikhail Gusarov, includes custom functions for
   the motors and sensors.

 * HiFive1 Rev B port.
   This uses a bunch of code from for all of the core and device support.

 * Lots of bugs fixed in string interpolation.

 * Lessons using LEGO with Snekboard
   There are four lessons that show you how to build a simple robot
   with step-by-step construction information, and then explore
   programming on a Snekboard.

 * Improved time.sleep precision. time.sleep is now accurate to the
   resolution of the timer. On a Duemilanove, that's
   4µs. time.monotonic still returns a float, so the longer
   the board has been running, the lower the precision...

 * Support for optiboot in Duemilanove code. Once you've installed
   optiboot on your Duemilanove, you can install Snek using that over
   the USB port, instead of needing an ISP.

### Version 1.3

 * Ports to ESP32 and µduino

 * Memory-corruption bug in string interpolation fixed

 * Bunch of code refactoring saves some space

 * Master now tested after every push on github


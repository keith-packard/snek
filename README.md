# Snek — a tiny python-inspired language for embedded computing

![Snek picture](snek.svg)

Snek is a tiny embeddable language targeting processors with only a
few kB of flash and ram. Think of something that would have been
running BASIC years ago and you'll have the idea. These processors are
too small to run [MicroPython](https://micropython.org/).

## Documentation

 * The [Snek Home Page](https://keithp.com/snek) provides more information
   about the language and community.

 * [Snek Manual in HTML format](https://keithp.com/snek/snek.html)

 * [Snek Manual in PDF format](https://keithp.com/snek/snek.pdf)

## Downloads

 * Snek is packaged for Debian and is available for the unstable release.

 * Packages for Linux, including older versions of Debian, Mac OS X
   and Windows are available in the
   [Snek Dist directory](http://keithp.com/snek/dist)

## To Do list

I think the basic Snek language is pretty complete at this point, but
there are always improvements that can be made.

## Recent Changes

Here's some places that have seen recent work

 * [Arduino Nano 33 IoT](https://store.arduino.cc/usa/nano-33-iot)
   port.  This device uses the Atmel SAMD21 SoC, but can't run
   Circuit Python because it doesn't have flash storage. The Snek port
   can drive the GPIO pins in simple ways, but doesn't have drivers
   for the other devices on the board.

 * QEMU, both ARM and RISC-V. This is the first RISC-V port of Snek,
   but I've got plans for real hardware once I've done a bit more work
   on the underlying SiFive freedom-metal system.

## Build and Run!

If you want to build Snek yourself, you'll need to have the build
tools and other dependencies installed.

### Dependencies
To build Snek you need these dependencies:

  * [Lola](https://keithp.com/cgit/lola.git/)
  * [gcc-avr](https://ccrma.stanford.edu/~juanig/articles/wiriavrlib/AVR_GCC.html)
  * [avr-libc](https://www.nongnu.org/avr-libc/)
  * [gcc-arm-none-eabi](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)
  * [gcc-riscv64-unknown-elf](https://github.com/sifive/freedom-tools/)
  * [picolibc](https://keithp.com/picolibc/)
  * [python3](https://www.python.org/)
  * [pyserial](https://github.com/pyserial/)
  * [python curses](https://docs.python.org/3/library/curses.html)

On Debian, you can get everything other than picolibc from the main archive:

	# apt install lola gcc-avr avr-libc python3-serial gcc-arm-none-eabi gcc-riscv64-unknown 

To install picolibc, download the source and install it. Instructions
for building and installing that are included in the picolibc project:

	$ git clone git://keithp.com/git/picolibc.git

### Building and install

In the source of the project run:

	$ make
	$ make install

### Some useful make options

If you just type 'make', the system will build all of the embedded
binaries along with Linux and Windows versions of snek to run locally
and then create packages for Linux, Windows and Mac OS X. 'make
install' will copy all of the build products to /usr/local. Here are
some useful options to control the build:

	$ make SNEK_OTHEROS=0 SNEK_OTHEROS_DIR=hosts/linux

This instructs the build to only build a Linux package, and not a
Windows or Mac OS X package.

	$ make PREFIX=$HOME/.local

This compiles everything to run from your home directory, instead of
/usr/local. You can use this option with `make PREFIX=$HOME/.local
install` to actually install things there.

## Running on Embedded Devices

Snek is designed to run on small embedded devices with 32kB or more of
flash and 2kB or more of RAM. Snek has been ported to a variety of
embedded devices, providing access to pins as GPIOs as well as some
built-in peripherals. Documentation about the supported boards can be
found in the [Snek Manual](https://keithp.com/snek/snek.html).

## Running on Linux, Mac OS X Windows

Snek is also built to run natively on Linux, Mac OS X and
Windows. When installed, you'll find it available in the regular
system menu.

## The Mu Editor

[mu](https://codewith.mu/) is an IDE especially designed for new
Python developers. It already has support for embedded boards running
MicroPython and CircuitPython, and there are patches available for
[Snek as well](https://github.com/keith-packard/mu).

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

Tab auto-indents the current line. Backspace backs up over a tabstop
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

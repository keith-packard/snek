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

 * Snek is packaged for Debian but not yet in the main archive. I have
   made packages available in my personal archive for
   now. Instructions for using that can be found
   [here](http://keithp.com/archive/README).  This archive also
   includes _lola_, the LL parser generator used in building Snek.

 * Packages for Linux, Mac OS X and Windows are available in the
   [Snek Dist directory](http://keithp.com/snek/dist)

## To Do list

I think the basic Snek language is pretty complete at this point, but
there are always improvements that can be made.

## Recent Changes

Here's some places that have seen recent work

 * [Crickit FeatherWing](https://www.adafruit.com/product/3343) port.
   This device has built-in drivers for motors and other devices,
   along with convenient plugs for other devices. If you want to get
   started building Snek robots, this is a great board to buy.

 * Port names. I've added names for all of the GPIO pins on the
   embedded devices. This was motivated by the new Crickit port. That
   device has six groups of I/O pins, which made using simple numbers
   pretty challenging. There are names for all GPIO pins on all
   devices now.
 
 * Documentation. [The Snek Programming Language](https://keithp.com/snek/snek.html)
   manual is getting more usable. This document includes a tutorial
   section for learning the language.

 * Packaging. Debian, General Linux, Windows and Mac OS X packages are
   available. These packages include menu entries and icons to run the
   host snek implementation and snekde in a terminal window along with
   binaries for Duemilanove, Metro M0 Express and Crickit
   FeatherWing. The Linux packages also include a shell script to
   install snek-duino onto an Duemilanove.

## Build and Run!

If you want to build Snek yourself, 

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

## Running on Duemilanove

On the Duemilanove board, Snek takes over the entire device, without
leaving room for the serial boot loader. Because of this, you will
need an AVR programming device, such as a [USBtiny from
Adafruit](https://www.adafruit.com/product/46).

The snek installation for Linux includes a script, snek-duino-install,
which uses avrdude to adjust the fuse values and flash the snek
image. On that platform, all you need to do is run it:

	$ snek-duino-install

## Running on the Metro M0 Express or Crickit (or compatible)

On the Adafruit Metro M0 Express and Adafruit Crickit boards, Snek
uses only a small part of the available flash space and so it can be
installed using the regular boot loader. Connect the board over USB to
the target machine (Linux, Mac OSX and Windows all work fine). Now
press the reset button twice. The boot loader should present a file
system on your host. Copy the binary included with the snek
distribution, which is named “metro-snek-<version>.uf2” (for the Metro
M0) or “crickit-snek-<version>.uf2” to the CURRENT.UF2 file on that
file system. This will magically flash the application to the device
and then it will reboot running Snek.

## Running on Linux or Windows

Snek is also built to run natively on Linux and Windows. When
installed, you'll find it available in the regular system menu. Snek
should be easy to get running on Mac OS X, but I don't have a
cross-compilation environment available on Linux to provide a
pre-built version.

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

There are examples provided which work with both Python and Snek.

## Contributions

I'd love to receive more contributions in the form of patches,
suggestions and bug reports. Please feel free to send mail or use the
github process. I've created a mailing list for collaboration; 
you'll need to subscribe to be able to post. Subscribe to the [snek
mailing list here](https://keithp.com/mailman/listinfo/snek)

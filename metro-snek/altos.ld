/*
 * Copyright © 2012 Keith Packard <keithp@keithp.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */

MEMORY {
	rom (rx) :   ORIGIN = 0x00002000, LENGTH = 128K - 8K
	flash (rw) : ORIGIN = 0x00020000, LENGTH = 64K
	ram (!w) :   ORIGIN = 0x20000000, LENGTH = 32K - 2K - 8
	stack (!w) : ORIGIN = 0x20000000 + 32k - 2K - 8, LENGTH = 2K
}

INCLUDE registers.ld

SECTIONS {
	/*
	 * Rom contents
	 */

	.text ORIGIN(rom) : {
		__text_start__ = .;

		KEEP(*(.interrupt))	/* interrupt vectors */

		. = ORIGIN(rom) + 0x100;

		/* Ick. What I want is to specify the
		 * addresses of some global constants so
		 * that I can find them across versions
		 * of the application. I can't figure out
		 * how to make gnu ld do that, so instead
		 * we just load the two files that include
		 * these defines in the right order here and
		 * expect things to 'just work'. Don't change
		 * the contents of those files, ok?
		 */
		ao_romconfig.o(.romconfig*)
		ao_product.o(.romconfig*)

		*(.text*)	/* Executable code */
	} > rom

	.ARM.exidx : {
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} > rom

	.rodata : {
		. = ALIGN(8);
		*(.rodata*)	/* Constants */
		. = ALIGN(8);
	} > rom

	__text_end__ = .;

	/* Functions placed in RAM (required for flashing)
	 *
	 * Align to 8 bytes as that's what the ARM likes text
	 * segment alignments to be, and if we don't, then
	 * we end up with a mismatch between the location in
	 * ROM and the desired location in RAM. I don't
	 * entirely understand this, but at least this appears
	 * to work...
	 */

	__data_start__ = ORIGIN(ram);

	.textram : {
		*(.ramtext)
		. = ALIGN(8);
	} >ram AT>rom

	/* Data -- relocated to RAM, but written to ROM
	 */
	.data : {
		*(.data)	/* initialized data */
		. = ALIGN(8);
		__data_end__ = .;
	} >ram AT>rom

	.bss : {
		__bss_start__ = .;
		*(.bss)
		*(COMMON)
		. = ALIGN(8);
		__bss_end__ = .;
	} >ram

	PROVIDE(end = .);

	PROVIDE(__stack__ = ORIGIN(stack) + LENGTH(stack));

	PROVIDE(__flash__ = ORIGIN(flash));
	PROVIDE(__flash_end__ = ORIGIN(flash) + LENGTH(flash));
}

ENTRY(start);



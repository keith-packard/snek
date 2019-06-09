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
	ram (!w) :   ORIGIN = 0x20000000, LENGTH = 32K - 2K
	stack (!w) : ORIGIN = 0x20000000 + 32k - 2K, LENGTH = 2K
}

SECTIONS {
	/*
	 * Rom contents
	 */

	.text ORIGIN(rom) : {
		__text_start__ = .;

		KEEP(*(.interrupt))	/* interrupt vectors */

		. = ORIGIN(rom) + 0x100;

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
		__snek_data_start__ = .;
		snek*(.data)
		__snek_data_end__ = .;
		*(.data)	/* initialized data */
		. = ALIGN(8);
		__data_end__ = .;
	} >ram AT>rom

	.bss : {
		__bss_start__ = .;
		__snek_bss_start__ = .;
		snek*(.bss)
		snek*(COMMON)
		__snek_bss_end__ = .;
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



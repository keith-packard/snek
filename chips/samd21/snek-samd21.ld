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
	stack (!w) : ORIGIN = 0x20000000 + 32K - 2K, LENGTH = 2K
}

SECTIONS {
	/*
	 * Rom contents
	 */

	.text : {
		__text_start = .;

		KEEP(*(.interrupt))	/* interrupt vectors */

		*(.text*)	/* Executable code */
	} > rom

	.ARM.exidx : {
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} > rom

	.rodata : {
		*(.rodata*)	/* Constants */
	} > rom

	. = ALIGN(8);

	__text_end = .;

	/* Functions placed in RAM (required for flashing) */

	.textram : ALIGN_WITH_INPUT {
		PROVIDE(__data_start = .);
		*(.ramtext)
	} >ram AT>rom
	PROVIDE(__data_source = LOADADDR(.textram));

	/* Data -- relocated to RAM, but written to ROM */
	.data : ALIGN_WITH_INPUT {
		PROVIDE(__snek_data_start = .);
		snek*(.data)
		__snek_data_end = .;
		*(.data)	/* initialized data */
	} >ram AT>rom
	PROVIDE(__snek_data_source = LOADADDR(.data));
	PROVIDE(__snek_data_size = __snek_data_end - __snek_data_start);

	/* Place .tdata initialization values in rom
	 * These will be after the regular data initialization
	 * and when copied will be placed in .tdata_space.
	 * Allocate addresses in ram so that we can make sure
	 * there is space. Not that this means the ram
	 *
	 * The .tbss segment must immediately follow the .tdata
	 * segment or the linker complains about them not beingn
	 * adjacent
	 */
	.tdata : ALIGN_WITH_INPUT {
		PROVIDE(__tls_base = .);
		*(.tdata .tdata.* .gnu.linkonce.td.*)
		__data_end = .;
	} >ram AT>rom
	PROVIDE(__tdata_source = LOADADDR(.tdata));
	PROVIDE(__tdata_size = SIZEOF(.tdata));
	PROVIDE(__data_size = __data_end - __data_start);

	/* Assign .tbss symbols addresses in rom so that they have the
	 * right values to make them fall right after the .tdata
	 * symbols. When linked, the resulting relocations will be
	 * written with correct values. I think this relies on the
	 * .tbss segment not being large enough to overflow the rom
	 * memory area
	 */
	.tbss : {
		PROVIDE(__bss_start = .);
		*( .tbss .tbss.* .gnu.linkonce.tb.* )
		*( .tcommon .tcommon.* )
		PROVIDE(__tls_end = .);
	} >ram
	PROVIDE(__tbss_size = SIZEOF(.tbss));
	__tls_size = __tls_end - __tls_base;

	/*
	 * The linker special cases .tbss segments which are
	 * identified as segments which are not loaded and are
	 * thread_local.
	 *
	 * For these segments, the linker does not advance 'dot'
	 * across them.  We actually need memory allocated for tbss,
	 * so we create a special segment here just to make room
	 */
	.tbss_space : {
		. = . + __tbss_size;
	} >ram

	.bss : {
		PROVIDE(__snek_bss_start = .);
		snek*( .bss .bss.* )
		snek*(COMMON)
		__snek_bss_end = .;
		*( .bss .bss.*)
		*(COMMON)
		__bss_end = .;
		PROVIDE(end = .);
	} >ram
	PROVIDE(__snek_bss_size = (__snek_bss_end - __snek_bss_start));
	PROVIDE(__bss_size = __bss_end - __bss_start);

	PROVIDE(__stack = ORIGIN(stack) + LENGTH(stack));

	PROVIDE(__flash__ = ORIGIN(flash));
	PROVIDE(__flash_end__ = ORIGIN(flash) + LENGTH(flash));
}

ENTRY(_start);



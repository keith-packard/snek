OUTPUT_ARCH("riscv")

ENTRY(_enter)

MEMORY
{
	flash (rxai!w) : ORIGIN = 0x20010000, LENGTH = 0x6a120
	itim (wx!rai) : ORIGIN = 0x8000000, LENGTH = 0x2000
	ram (wxa!ri) : ORIGIN = 0x80000000, LENGTH = 0x4000
}

PHDRS
{
	flash PT_LOAD;
	ram PT_LOAD;
	ram_init PT_LOAD;
	itim PT_LOAD;
	itim_init PT_LOAD;
	tls PT_TLS;
}

SECTIONS
{

	PROVIDE(__metal_boot_hart = 0);
	PROVIDE(__metal_chicken_bit = 0);

	.init : {
		KEEP (*(.text.metal.init.enter))
		KEEP (*(.text.metal.init.*))
		KEEP (*(SORT_NONE(.init)))
		KEEP (*(.text.libgloss.start))
	} >flash AT>flash :flash

	.fini : {
		KEEP (*(SORT_NONE(.fini)))
	} >flash AT>flash :flash



	.text : {
		*(.text.unlikely .text.unlikely.*)
		*(.text.startup .text.startup.*)
		*(.text .text.*)
		*(.gnu.linkonce.t.*)
	} >flash AT>flash :flash

	PROVIDE (__etext = .);
	PROVIDE (_etext = .);
	PROVIDE (etext = .);


	.rodata : {
		*(.rdata)
		*(.rodata .rodata.*)
		*(.gnu.linkonce.r.*)
		. = ALIGN(8);
		*(.srodata.cst16)
		*(.srodata.cst8)
		*(.srodata.cst4)
		*(.srodata.cst2)
		*(.srodata .srodata.*)
	} >flash AT>flash :flash


	. = ALIGN(8);

	.preinit_array : {
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP (*(.preinit_array))
		PROVIDE_HIDDEN (__preinit_array_end = .);
	} >flash AT>flash :flash

	.init_array : {
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
		KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
		KEEP (*(SORT_BY_INIT_PRIORITY(.metal.init_array.*)));
		KEEP (*(.metal.init_array));
		PROVIDE_HIDDEN (__init_array_end = .);
	} >flash AT>flash :flash

	.fini_array : {
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
		KEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))
		KEEP (*(SORT_BY_INIT_PRIORITY(.metal.fini_array.*)));
		KEEP (*(.metal.fini_array));
		PROVIDE_HIDDEN (__fini_array_end = .);
	} >flash AT>flash :flash

	.ctors : {
		KEEP (*crtbegin.o(.ctors))
		KEEP (*crtbegin?.o(.ctors))
		KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
		KEEP (*(SORT(.ctors.*)))
		KEEP (*(.ctors))
		KEEP (*(.metal.ctors metal.ctors.*));
	} >flash AT>flash :flash

	.dtors : {
		KEEP (*crtbegin.o(.dtors))
		KEEP (*crtbegin?.o(.dtors))
		KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
		KEEP (*(SORT(.dtors.*)))
		KEEP (*(.dtors))
		KEEP (*(.metal.dtors metal.dtors.*));
	} >flash AT>flash :flash



	.itim : ALIGN(8) {
		*(.itim .itim.*)
	} >itim AT>flash :itim_init

	PROVIDE( metal_segment_itim_source_start = LOADADDR(.itim) );
	PROVIDE( metal_segment_itim_target_start = ADDR(.itim) );
	PROVIDE( metal_segment_itim_target_end = ADDR(.itim) + SIZEOF(.itim) );


	.data : ALIGN(8) {
		*(.data .data.*)
		*(.gnu.linkonce.d.*)
		. = ALIGN(8);
		PROVIDE( __global_pointer$ = . + 0x800 );
		*(.sdata .sdata.* .sdata2.*)
		*(.gnu.linkonce.s.*)
		. = ALIGN(8);
		*(.srodata.cst16)
		*(.srodata.cst8)
		*(.srodata.cst4)
		*(.srodata.cst2)
		*(.srodata .srodata.*)
		. = ALIGN(8);
		*(.rdata)
		*(.rodata .rodata.*)
		*(.gnu.linkonce.r.*)
	} >ram AT>flash :ram_init

	.tdata : {
		PROVIDE( __tls_base = . );
		*(.tdata .tdata.* .gnu.linkonce.td.*)
	} >ram AT>flash :tls :ram_init

	PROVIDE( metal_segment_data_source_start = LOADADDR(.data) );
	PROVIDE( __data_source = LOADADDR(.data) );
	PROVIDE( metal_segment_data_target_start = ADDR(.data) );
	PROVIDE( __data_start = ADDR(.data) );
	PROVIDE( __data_size = . - ADDR(.data) );
	PROVIDE( __tdata_source = LOADADDR(.tdata) );
	PROVIDE( __tdata_size = SIZEOF(.tdata) );
	PROVIDE( _edata = . );
	PROVIDE( edata = . );
	PROVIDE( metal_segment_data_target_end = . );


	.tbss : {
		*(.tbss .tbss.* .gnu.linkonce.tb.*)
		*(.tcommon .tcommon.*)
		PROVIDE( __tls_end = . );
	} >ram AT>ram :tls :ram
	PROVIDE( __tbss_size = SIZEOF(.tbss));
	PROVIDE( __tls_size = __tls_end - __tls_base );

	.tbss_space : {
		. = . + __tbss_size;
	} >ram AT>ram :ram

	.bss : ALIGN(8) {
		*(.sbss*)
		*(.gnu.linkonce.sb.*)
		*(.bss .bss.*)
		*(.gnu.linkonce.b.*)
		*(COMMON)
	} >ram AT>ram :ram
	PROVIDE( metal_segment_bss_target_start = ADDR(.tbss) );
	PROVIDE( __bss_start = ADDR(.tbss) );
	PROVIDE( metal_segment_bss_target_end = . );
	PROVIDE( __bss_size = . - ADDR(.tbss) );

	.heap : {
		PROVIDE( metal_segment_heap_target_start = . );
		PROVIDE( metal_segment_heap_target_end = ORIGIN(ram) + LENGTH(ram) - (DEFINED(__stack_size) ? __stack_size : 0x400) );
		PROVIDE( _heap_end = . );
	} >ram AT>ram :ram

	.stack : {
		PROVIDE(metal_segment_stack_begin = metal_segment_heap_target_end);
		. = ORIGIN(ram) + LENGTH(ram) ;
		PROVIDE( _sp = . );
		PROVIDE( __stack = . );
		PROVIDE(metal_segment_stack_end = .);
		PROVIDE(__stack_size = metal_segment_stack_end - metal_segment_stack_begin);
	} >ram AT>ram :ram
}


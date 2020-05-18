MEMORY
{
    flash (rx)  : org = 0x00000000, len = 32k
    ram   (xrw) : org = 0x10000000, len = 8k
}
  
SECTIONS
{
	. = ORIGIN(flash);
        .text : 
        {
                FILL(0xff)
                KEEP (*(.isr_vector));
                KEEP (*(.after_vectors));
                . = 0x200;
                *libgcc.a:*(*); /* .text* .text.* */
                . = 0x1700;
                EXCLUDE_FILE (*/pid.o */motor.o) *(.text*);
                *(.keepinflash*);
                *(.rodata*);
                . = ALIGN(4);
        } >flash
        
	. = ORIGIN(ram);
        .reloadtext : 
        {
                . = 0x0; /* + 0xa00 */
                */pid.o (.text*)
                */motor.o (.text*)
                . = 0xa00; /* + 0xa00 */
        } >ram AT>flash

        .data : 
        {
                . = 0xa00;
                *(.data*);
                *(.sdata*);
                . = ALIGN(8);
        } >ram AT>flash

        PROVIDE(_etext = LOADADDR(.data));
        PROVIDE(_data = ADDR(.data));
        PROVIDE(_edata = ADDR(.data) + SIZEOF(.data));

        .bss (NOLOAD) : ALIGN(4)
        {
                *(.bss*);
                *(.sbss*);
                . = ALIGN(8);	  
        } > ram

        PROVIDE(_bss = ADDR(.bss));
        PROVIDE(_ebss = ADDR(.bss) + SIZEOF(.bss));
        PROVIDE(__bss_start__ = ADDR(.bss));
        PROVIDE(__bss_end__ = ADDR(.bss) + SIZEOF(.bss));

	_vStackTop = 0x10000000 + 0x2000 -16;
}

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
                EXCLUDE_FILE (*/pid.o */motor.o */motor_ct.o) *(.text*);
                *(.keepinflash*);
                */pid.o (.text*)
                */motor.o (.text*)
                */motor_ct.o (.text*)
                *(.rodata*);
                . = ALIGN(4);
        } >flash
        
	. = ORIGIN(ram);
	/* LOADATSLOT2 = 1; */
        .reloadtext : 
        {
                . = 0x0 + (DEFINED(LOADATSLOT2) ? 0xa00 : 0x0);
                . = 0xa00 + (DEFINED(LOADATSLOT2) ? 0xa00 : 0x0);
        } >ram AT>flash

        PROVIDE(_fromreloadtext = LOADADDR(.reloadtext));
        PROVIDE(_reloadtext = ADDR(.reloadtext));
        PROVIDE(_ereloadtext = ADDR(.reloadtext) + SIZEOF(.reloadtext));

        .bufferspace : 
        {
                . = (DEFINED(LOADATSLOT2) ? 0x0 : 0xa00);
        } >ram

        .data : 
        {
                /* . = 0xa00; */
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

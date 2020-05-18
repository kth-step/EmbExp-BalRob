.syntax unified
.globl motor_prep_input_ct

motor_prep_input_ct:
	asrs	r3, r0, #31
	adds	r0, r0, r3
	eors	r0, r3
	ldr	r3, =0x00001770
	cmp	r0, r3
	ble.n	motor_prep_input_ct+0xe
	movs	r0, r3
	ldr	r3, =0x00001770
	subs	r0, r3, r0
	bx	lr


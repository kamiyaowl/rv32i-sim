	.file	"hello.c"
	.option nopic
	.text
	.align	2
	.globl	uart_tx
	.type	uart_tx, @function
uart_tx:
	addi	sp,sp,-48
	sw	s0,44(sp)
	addi	s0,sp,48
	mv	a5,a0
	sb	a5,-33(s0)
	li	a5,268435456
	sw	a5,-20(s0)
	lw	a5,-20(s0)
	lbu	a4,-33(s0)
	sb	a4,0(a5)
	nop
	lw	s0,44(sp)
	addi	sp,sp,48
	jr	ra
	.size	uart_tx, .-uart_tx
	.align	2
	.globl	print
	.type	print, @function
print:
	addi	sp,sp,-48
	sw	ra,44(sp)
	sw	s0,40(sp)
	addi	s0,sp,48
	sw	a0,-36(s0)
	sw	zero,-20(s0)
	j	.L3
.L4:
	lw	a5,-20(s0)
	lw	a4,-36(s0)
	add	a5,a4,a5
	lbu	a5,0(a5)
	mv	a0,a5
	call	uart_tx
	lw	a5,-20(s0)
	addi	a5,a5,1
	sw	a5,-20(s0)
.L3:
	lw	a5,-20(s0)
	lw	a4,-36(s0)
	add	a5,a4,a5
	lbu	a5,0(a5)
	bnez	a5,.L4
	nop
	lw	ra,44(sp)
	lw	s0,40(sp)
	addi	sp,sp,48
	jr	ra
	.size	print, .-print
	.section	.rodata
	.align	2
.LC0:
	.string	"Hello RISC-V! \n"
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-32
	sw	ra,28(sp)
	sw	s0,24(sp)
	addi	s0,sp,32
	lui	a5,%hi(.LC0)
	addi	a5,a5,%lo(.LC0)
	sw	a5,-20(s0)
	lw	a0,-20(s0)
	call	print
	li	a5,0
	mv	a0,a5
	lw	ra,28(sp)
	lw	s0,24(sp)
	addi	sp,sp,32
	jr	ra
	.size	main, .-main
	.ident	"GCC: (GNU) 8.3.0"

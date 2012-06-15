.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
	li $v0, 4	
la $a0, _prompt
	syscall
	li $v0, 5
	syscall
	jr $ra

write:
	li $v0, 1
	syscall
	li $v0, 4
	la $a0, _ret
	syscall
	move $v0, $0
	jr $ra


main:
	addi $sp, $sp, -52
	addi $t1, $t2, 0
	sw $t1, 8($sp)
	lw $t1, 0($sp)
	lw $t2, 8($sp)
	move $t1, $t2
	sw $t1, 0($sp)
	sw $t2, 8($sp)
	lw $t1, 4($sp)
	li $t1, 1
	sw $t1, 4($sp)
	addi $t1, $t2, 0
	sw $t1, 20($sp)
	lw $t1, 16($sp)
	lw $t2, 20($sp)
	move $t1, $t2
	sw $t1, 16($sp)
	sw $t2, 20($sp)
	lw $t1, 36($sp)
	lw $t2, 16($sp)
	move $t1, $t2
	sw $t1, 36($sp)
	sw $t2, 16($sp)
	lw $t1, 12($sp)
	lw $t2, 36($sp)
	move $t1, $t2
	sw $t1, 12($sp)
	sw $t2, 36($sp)
	lw $t1, 24($sp)
	lw $t2, 36($sp)
	move $t1, $t2
	sw $t1, 24($sp)
	sw $t2, 36($sp)
	lw $t1, 24($sp)
	move $a0, $t1
	addi $sp, $sp, -4
	sw, $ra 0($sp)
	jal write
	lw $ra, 0($sp)
	addi $sp, $sp, 4

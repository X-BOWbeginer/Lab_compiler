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


fact:
  addi $sp, $sp, -4
  sw $fp, 0($sp)
  move $fp, $sp
  addi $sp, $sp, -88
  lw $t0, 8($fp)
  sw $t0, -8($fp)
  lw $t1, -8($fp)
  move $t0, $t1
  sw $t0, -20($fp)
  li $t1, 1
  move $t0, $t1
  sw $t0, -24($fp)
  lw $t1, -20($fp)
  lw $t2, -24($fp)
  beq $t1, $t2, label1
  j label2
label1:
  lw $t1, -8($fp)
  move $t0, $t1
  sw $t0, -28($fp)
  lw $v0, -28($fp)
  addi $sp, $fp, 4
  lw $fp, 0($fp)
  jr $ra
  j label3
label2:
  lw $t1, -8($fp)
  move $t0, $t1
  sw $t0, -36($fp)
  lw $t1, -8($fp)
  move $t0, $t1
  sw $t0, -48($fp)
  li $t1, 1
  move $t0, $t1
  sw $t0, -52($fp)
  lw $t1, -48($fp)
  lw $t2, -52($fp)
  sub $t0, $t1, $t2
  sw $t0, -44($fp)
  addi $sp, $sp, -4
  lw $t0, -44($fp)
  sw $t0, 0($sp)
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal fact
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  sw $v0, -40($fp)
  addi $sp, $sp, 4
  lw $t1, -36($fp)
  lw $t2, -40($fp)
  mul $t0, $t1, $t2
  sw $t0, -32($fp)
  lw $v0, -32($fp)
  addi $sp, $fp, 4
  lw $fp, 0($fp)
  jr $ra
label3:

main:
  addi $sp, $sp, -4
  sw $fp, 0($sp)
  move $fp, $sp
  addi $sp, $sp, -88
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal read
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  sw $v0, -56($fp)
  lw $t1, -56($fp)
  move $t0, $t1
  sw $t0, -4($fp)
  lw $t1, -4($fp)
  move $t0, $t1
  sw $t0, -60($fp)
  li $t1, 1
  move $t0, $t1
  sw $t0, -64($fp)
  lw $t1, -60($fp)
  lw $t2, -64($fp)
  bgt $t1, $t2, label4
  j label5
label4:
  lw $t1, -4($fp)
  move $t0, $t1
  sw $t0, -72($fp)
  addi $sp, $sp, -4
  lw $t0, -72($fp)
  sw $t0, 0($sp)
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal fact
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  sw $v0, -68($fp)
  addi $sp, $sp, 4
  lw $t1, -68($fp)
  move $t0, $t1
  sw $t0, -12($fp)
  j label6
label5:
  li $t1, 1
  move $t0, $t1
  sw $t0, -76($fp)
  lw $t1, -76($fp)
  move $t0, $t1
  sw $t0, -12($fp)
label6:
  lw $t1, -12($fp)
  move $t0, $t1
  sw $t0, -80($fp)
  lw $a0, -80($fp)
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  li $t1, 0
  move $t0, $t1
  sw $t0, -84($fp)
  lw $v0, -84($fp)
  addi $sp, $fp, 4
  lw $fp, 0($fp)
  jr $ra

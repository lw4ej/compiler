	# standard Decaf preamble 
	  .text
	  .align 2
	  .globl main
	# _fib:
  _fib:
	# BeginFunc 68
	  subu $sp, $sp, 8	# decrement sp to make space to save ra, fp
	  sw $fp, 8($sp)	# save fp
	  sw $ra, 4($sp)	# save ra
	  addiu $fp, $sp, 8	# set up new fp
	  subu $sp, $sp, 68	# decrement sp to make space for locals/temps
	# _tmp0 = 1
	  li $t2, 1		# load constant value 1 into $t2
	  sw $t2, -8($fp)	# spill _tmp0 from $t2 to $fp-8
	# _tmp1 = base < _tmp0
	  lw $t0, 4($fp)	# fill base to $t0 from $fp+4
	  lw $t1, -8($fp)	# fill _tmp0 to $t1 from $fp-8
	  slt $t2, $t0, $t1	
	  sw $t2, -12($fp)	# spill _tmp1 from $t2 to $fp-12
	# _tmp2 = base == _tmp0
	  lw $t0, 4($fp)	# fill base to $t0 from $fp+4
	  lw $t1, -8($fp)	# fill _tmp0 to $t1 from $fp-8
	  seq $t2, $t0, $t1	
	  sw $t2, -16($fp)	# spill _tmp2 from $t2 to $fp-16
	# _tmp3 = _tmp1 || _tmp2
	  lw $t0, -12($fp)	# fill _tmp1 to $t0 from $fp-12
	  lw $t1, -16($fp)	# fill _tmp2 to $t1 from $fp-16
	  or $t2, $t0, $t1	
	  sw $t2, -20($fp)	# spill _tmp3 from $t2 to $fp-20
	# IfZ _tmp3 Goto _L0
	  lw $t0, -20($fp)	# fill _tmp3 to $t0 from $fp-20
	  beqz $t0, _L0	# branch if _tmp3 is zero 
	# Return base
	  lw $t2, 4($fp)	# fill base to $t2 from $fp+4
	  move $v0, $t2		# assign return value into $v0
	  move $sp, $fp		# pop callee frame off stack
	  lw $ra, -4($fp)	# restore saved ra
	  lw $fp, 0($fp)	# restore saved fp
	  jr $ra		# return from function
	# Goto _L1
	  b _L1		# unconditional branch
	# _L0:
  _L0:
	# _tmp4 = 0
	  li $t2, 0		# load constant value 0 into $t2
	  sw $t2, -40($fp)	# spill _tmp4 from $t2 to $fp-40
	# f0 = _tmp4
	  lw $t2, -40($fp)	# fill _tmp4 to $t2 from $fp-40
	  sw $t2, -28($fp)	# spill f0 from $t2 to $fp-28
	# _tmp5 = 1
	  li $t2, 1		# load constant value 1 into $t2
	  sw $t2, -44($fp)	# spill _tmp5 from $t2 to $fp-44
	# f1 = _tmp5
	  lw $t2, -44($fp)	# fill _tmp5 to $t2 from $fp-44
	  sw $t2, -32($fp)	# spill f1 from $t2 to $fp-32
	# _tmp6 = 2
	  li $t2, 2		# load constant value 2 into $t2
	  sw $t2, -48($fp)	# spill _tmp6 from $t2 to $fp-48
	# i = _tmp6
	  lw $t2, -48($fp)	# fill _tmp6 to $t2 from $fp-48
	  sw $t2, -24($fp)	# spill i from $t2 to $fp-24
	# _L2:
  _L2:
	# _tmp7 = i < base
	  lw $t0, -24($fp)	# fill i to $t0 from $fp-24
	  lw $t1, 4($fp)	# fill base to $t1 from $fp+4
	  slt $t2, $t0, $t1	
	  sw $t2, -52($fp)	# spill _tmp7 from $t2 to $fp-52
	# _tmp8 = i == base
	  lw $t0, -24($fp)	# fill i to $t0 from $fp-24
	  lw $t1, 4($fp)	# fill base to $t1 from $fp+4
	  seq $t2, $t0, $t1	
	  sw $t2, -56($fp)	# spill _tmp8 from $t2 to $fp-56
	# _tmp9 = _tmp7 || _tmp8
	  lw $t0, -52($fp)	# fill _tmp7 to $t0 from $fp-52
	  lw $t1, -56($fp)	# fill _tmp8 to $t1 from $fp-56
	  or $t2, $t0, $t1	
	  sw $t2, -60($fp)	# spill _tmp9 from $t2 to $fp-60
	# IfZ _tmp9 Goto _L3
	  lw $t0, -60($fp)	# fill _tmp9 to $t0 from $fp-60
	  beqz $t0, _L3	# branch if _tmp9 is zero 
	# _tmp10 = f0 + f1
	  lw $t0, -28($fp)	# fill f0 to $t0 from $fp-28
	  lw $t1, -32($fp)	# fill f1 to $t1 from $fp-32
	  add $t2, $t0, $t1	
	  sw $t2, -64($fp)	# spill _tmp10 from $t2 to $fp-64
	# f2 = _tmp10
	  lw $t2, -64($fp)	# fill _tmp10 to $t2 from $fp-64
	  sw $t2, -36($fp)	# spill f2 from $t2 to $fp-36
	# f0 = f1
	  lw $t2, -32($fp)	# fill f1 to $t2 from $fp-32
	  sw $t2, -28($fp)	# spill f0 from $t2 to $fp-28
	# f1 = f2
	  lw $t2, -36($fp)	# fill f2 to $t2 from $fp-36
	  sw $t2, -32($fp)	# spill f1 from $t2 to $fp-32
	# _tmp11 = 1
	  li $t2, 1		# load constant value 1 into $t2
	  sw $t2, -68($fp)	# spill _tmp11 from $t2 to $fp-68
	# _tmp12 = i + _tmp11
	  lw $t0, -24($fp)	# fill i to $t0 from $fp-24
	  lw $t1, -68($fp)	# fill _tmp11 to $t1 from $fp-68
	  add $t2, $t0, $t1	
	  sw $t2, -72($fp)	# spill _tmp12 from $t2 to $fp-72
	# i = _tmp12
	  lw $t2, -72($fp)	# fill _tmp12 to $t2 from $fp-72
	  sw $t2, -24($fp)	# spill i from $t2 to $fp-24
	# Goto _L2
	  b _L2		# unconditional branch
	# _L3:
  _L3:
	# Return f2
	  lw $t2, -36($fp)	# fill f2 to $t2 from $fp-36
	  move $v0, $t2		# assign return value into $v0
	  move $sp, $fp		# pop callee frame off stack
	  lw $ra, -4($fp)	# restore saved ra
	  lw $fp, 0($fp)	# restore saved fp
	  jr $ra		# return from function
	# _L1:
  _L1:
	# EndFunc
	# (below handles reaching end of fn body with no explicit return)
	  move $sp, $fp		# pop callee frame off stack
	  lw $ra, -4($fp)	# restore saved ra
	  lw $fp, 0($fp)	# restore saved fp
	  jr $ra		# return from function
	# main:
  main:
	# BeginFunc 56
	  subu $sp, $sp, 8	# decrement sp to make space to save ra, fp
	  sw $fp, 8($sp)	# save fp
	  sw $ra, 4($sp)	# save ra
	  addiu $fp, $sp, 8	# set up new fp
	  subu $sp, $sp, 56	# decrement sp to make space for locals/temps
	# _tmp13 = "\nThis program computes Fibonacci numbers (slowly..."
	  .data			# create string constant marked with label
	  _string1: .asciiz "\nThis program computes Fibonacci numbers (slowly.. but correctly!)\n"
	  .text
	  la $t2, _string1	# load label
	  sw $t2, -12($fp)	# spill _tmp13 from $t2 to $fp-12
	# PushParam _tmp13
	  subu $sp, $sp, 4	# decrement sp to make space for param
	  lw $t0, -12($fp)	# fill _tmp13 to $t0 from $fp-12
	  sw $t0, 4($sp)	# copy param value to stack
	# LCall _PrintString
	  jal _PrintString   	# jump to function
	# PopParams 4
	  add $sp, $sp, 4	# pop params off stack
	# _L4:
  _L4:
	# _tmp14 = 1
	  li $t2, 1		# load constant value 1 into $t2
	  sw $t2, -16($fp)	# spill _tmp14 from $t2 to $fp-16
	# IfZ _tmp14 Goto _L5
	  lw $t0, -16($fp)	# fill _tmp14 to $t0 from $fp-16
	  beqz $t0, _L5	# branch if _tmp14 is zero 
	# _tmp15 = "\nEnter the fibonacci number you want: (-1 to qui..."
	  .data			# create string constant marked with label
	  _string2: .asciiz "\nEnter the fibonacci number you want: (-1 to quit) "
	  .text
	  la $t2, _string2	# load label
	  sw $t2, -20($fp)	# spill _tmp15 from $t2 to $fp-20
	# PushParam _tmp15
	  subu $sp, $sp, 4	# decrement sp to make space for param
	  lw $t0, -20($fp)	# fill _tmp15 to $t0 from $fp-20
	  sw $t0, 4($sp)	# copy param value to stack
	# LCall _PrintString
	  jal _PrintString   	# jump to function
	# PopParams 4
	  add $sp, $sp, 4	# pop params off stack
	# _tmp16 = LCall _ReadInteger
	  jal _ReadInteger   	# jump to function
	  move $t2, $v0		# copy function return value from $v0
	  sw $t2, -24($fp)	# spill _tmp16 from $t2 to $fp-24
	# n = _tmp16
	  lw $t2, -24($fp)	# fill _tmp16 to $t2 from $fp-24
	  sw $t2, -8($fp)	# spill n from $t2 to $fp-8
	# _tmp17 = 0
	  li $t2, 0		# load constant value 0 into $t2
	  sw $t2, -28($fp)	# spill _tmp17 from $t2 to $fp-28
	# _tmp18 = 1
	  li $t2, 1		# load constant value 1 into $t2
	  sw $t2, -32($fp)	# spill _tmp18 from $t2 to $fp-32
	# _tmp19 = _tmp17 - _tmp18
	  lw $t0, -28($fp)	# fill _tmp17 to $t0 from $fp-28
	  lw $t1, -32($fp)	# fill _tmp18 to $t1 from $fp-32
	  sub $t2, $t0, $t1	
	  sw $t2, -36($fp)	# spill _tmp19 from $t2 to $fp-36
	# _tmp20 = n == _tmp19
	  lw $t0, -8($fp)	# fill n to $t0 from $fp-8
	  lw $t1, -36($fp)	# fill _tmp19 to $t1 from $fp-36
	  seq $t2, $t0, $t1	
	  sw $t2, -40($fp)	# spill _tmp20 from $t2 to $fp-40
	# IfZ _tmp20 Goto _L6
	  lw $t0, -40($fp)	# fill _tmp20 to $t0 from $fp-40
	  beqz $t0, _L6	# branch if _tmp20 is zero 
	# Goto _L5
	  b _L5		# unconditional branch
	# _L6:
  _L6:
	# _tmp21 = "Fib("
	  .data			# create string constant marked with label
	  _string3: .asciiz "Fib("
	  .text
	  la $t2, _string3	# load label
	  sw $t2, -44($fp)	# spill _tmp21 from $t2 to $fp-44
	# PushParam _tmp21
	  subu $sp, $sp, 4	# decrement sp to make space for param
	  lw $t0, -44($fp)	# fill _tmp21 to $t0 from $fp-44
	  sw $t0, 4($sp)	# copy param value to stack
	# LCall _PrintString
	  jal _PrintString   	# jump to function
	# PopParams 4
	  add $sp, $sp, 4	# pop params off stack
	# PushParam n
	  subu $sp, $sp, 4	# decrement sp to make space for param
	  lw $t0, -8($fp)	# fill n to $t0 from $fp-8
	  sw $t0, 4($sp)	# copy param value to stack
	# LCall _PrintInt
	  jal _PrintInt      	# jump to function
	# PopParams 4
	  add $sp, $sp, 4	# pop params off stack
	# _tmp22 = ") = "
	  .data			# create string constant marked with label
	  _string4: .asciiz ") = "
	  .text
	  la $t2, _string4	# load label
	  sw $t2, -48($fp)	# spill _tmp22 from $t2 to $fp-48
	# PushParam _tmp22
	  subu $sp, $sp, 4	# decrement sp to make space for param
	  lw $t0, -48($fp)	# fill _tmp22 to $t0 from $fp-48
	  sw $t0, 4($sp)	# copy param value to stack
	# LCall _PrintString
	  jal _PrintString   	# jump to function
	# PopParams 4
	  add $sp, $sp, 4	# pop params off stack
	# PushParam n
	  subu $sp, $sp, 4	# decrement sp to make space for param
	  lw $t0, -8($fp)	# fill n to $t0 from $fp-8
	  sw $t0, 4($sp)	# copy param value to stack
	# _tmp23 = LCall _fib
	  jal _fib           	# jump to function
	  move $t2, $v0		# copy function return value from $v0
	  sw $t2, -52($fp)	# spill _tmp23 from $t2 to $fp-52
	# PopParams 4
	  add $sp, $sp, 4	# pop params off stack
	# PushParam _tmp23
	  subu $sp, $sp, 4	# decrement sp to make space for param
	  lw $t0, -52($fp)	# fill _tmp23 to $t0 from $fp-52
	  sw $t0, 4($sp)	# copy param value to stack
	# LCall _PrintInt
	  jal _PrintInt      	# jump to function
	# PopParams 4
	  add $sp, $sp, 4	# pop params off stack
	# _tmp24 = "\n"
	  .data			# create string constant marked with label
	  _string5: .asciiz "\n"
	  .text
	  la $t2, _string5	# load label
	  sw $t2, -56($fp)	# spill _tmp24 from $t2 to $fp-56
	# PushParam _tmp24
	  subu $sp, $sp, 4	# decrement sp to make space for param
	  lw $t0, -56($fp)	# fill _tmp24 to $t0 from $fp-56
	  sw $t0, 4($sp)	# copy param value to stack
	# LCall _PrintString
	  jal _PrintString   	# jump to function
	# PopParams 4
	  add $sp, $sp, 4	# pop params off stack
	# Goto _L4
	  b _L4		# unconditional branch
	# _L5:
  _L5:
	# _tmp25 = "Goodbye!\n"
	  .data			# create string constant marked with label
	  _string6: .asciiz "Goodbye!\n"
	  .text
	  la $t2, _string6	# load label
	  sw $t2, -60($fp)	# spill _tmp25 from $t2 to $fp-60
	# PushParam _tmp25
	  subu $sp, $sp, 4	# decrement sp to make space for param
	  lw $t0, -60($fp)	# fill _tmp25 to $t0 from $fp-60
	  sw $t0, 4($sp)	# copy param value to stack
	# LCall _PrintString
	  jal _PrintString   	# jump to function
	# PopParams 4
	  add $sp, $sp, 4	# pop params off stack
	# EndFunc
	# (below handles reaching end of fn body with no explicit return)
	  move $sp, $fp		# pop callee frame off stack
	  lw $ra, -4($fp)	# restore saved ra
	  lw $fp, 0($fp)	# restore saved fp
	  jr $ra		# return from function
_PrintInt:
        subu $sp, $sp, 8
        sw $fp, 8($sp)
        sw $ra, 4($sp)
        addiu $fp, $sp, 8
        li   $v0, 1
        lw   $a0, 4($fp)
        syscall
        move $sp, $fp
        lw $ra, -4($fp)
        lw $fp, 0($fp)
        jr $ra
                                
_PrintString:
        subu $sp, $sp, 8
        sw $fp, 8($sp)
        sw $ra, 4($sp)
        addiu $fp, $sp, 8
        li   $v0, 4
        lw $a0, 4($fp)
        syscall
        move $sp, $fp
        lw $ra, -4($fp)
        lw $fp, 0($fp)
        jr $ra
        
_PrintBool:
	subu $sp, $sp, 8
	sw $fp, 8($sp)
	sw $ra, 4($sp)
        addiu $fp, $sp, 8
	lw $t1, 4($fp)
	blez $t1, fbr
	li   $v0, 4		# system call for print_str
	la   $a0, TRUE		# address of str to print
	syscall			
	b end
fbr:	li   $v0, 4		# system call for print_str
	la   $a0, FALSE		# address of str to print
	syscall				
end:	move $sp, $fp
	lw $ra, -4($fp)
	lw $fp, 0($fp)
	jr $ra

_Alloc:
        subu $sp, $sp, 8
        sw $fp, 8($sp)
        sw $ra, 4($sp)
        addiu $fp, $sp, 8
        li   $v0, 9
        lw $a0, 4($fp)
	syscall
        move $sp, $fp
        lw $ra, -4($fp)
        lw $fp, 0($fp) 
        jr $ra


_StringEqual:
	subu $sp, $sp, 8      # decrement sp to make space to save ra, fp
	sw $fp, 8($sp)        # save fp
	sw $ra, 4($sp)        # save ra
	addiu $fp, $sp, 8     # set up new fp
	subu $sp, $sp, 4      # decrement sp to make space for locals/temps

	li $v0,0

	#Determine length string 1
	lw $t0, 4($fp)       
	li $t3,0
bloop1: 
	lb $t5, ($t0) 
	beqz $t5, eloop1	
	addi $t0, 1
	addi $t3, 1
	b bloop1
eloop1:
	
	#Determine length string 2
	lw $t1, 8($fp)
	li $t4,0
bloop2: 
	lb $t5, ($t1) 
	beqz $t5, eloop2	
	addi $t1, 1
	addi $t4, 1
	b bloop2
eloop2:
	bne $t3,$t4,end1       #Check String Lengths Same

	lw $t0, 4($fp)	
	lw $t1, 8($fp)
	li $t3, 0     		
bloop3:	
	lb $t5, ($t0) 
	lb $t6, ($t1) 
	bne $t5, $t6, end1
	beqz $t5, eloop3       # if zero, then we hit the end of both strings
	addi $t3, 1
	addi $t0, 1
	addi $t1, 1
	#bne $t3,$t4,bloop3
	b bloop3
eloop3:	li $v0,1

end1:	move $sp, $fp         # pop callee frame off stack
	lw $ra, -4($fp)       # restore saved ra
	lw $fp, 0($fp)        # restore saved fp
	jr $ra                # return from function

_Halt:
        li $v0, 10
        syscall

_ReadInteger:
	subu $sp, $sp, 8      # decrement sp to make space to save ra, fp
	sw $fp, 8($sp)        # save fp
	sw $ra, 4($sp)        # save ra
	addiu $fp, $sp, 8     # set up new fp
	subu $sp, $sp, 4      # decrement sp to make space for locals/temps
	li $v0, 5
	syscall
	move $sp, $fp         # pop callee frame off stack
	lw $ra, -4($fp)       # restore saved ra
	lw $fp, 0($fp)        # restore saved fp
	jr $ra
        

_ReadLine:
	subu $sp, $sp, 8      # decrement sp to make space to save ra, fp
	sw $fp, 8($sp)        # save fp
	sw $ra, 4($sp)        # save ra
	addiu $fp, $sp, 8     # set up new fp
	subu $sp, $sp, 4      # decrement sp to make space for locals/temps
	# allocate space to store memory
	li $a0, 128           # request 128 bytes
	li $v0, 9	      # syscall "sbrk" for memory allocation
	syscall               # do the system call
	# read in the new line
	li $a1, 128	      # size of the buffer
	#la $a0, SPACE        
	move $a0, $v0	      # location of the buffer	
	li $v0, 8 
	syscall

	#la $t1, SPACE  
	move $t1, $a0
bloop4: 
	lb $t5, ($t1) 
	beqz $t5, eloop4	
	addi $t1, 1
	b bloop4
eloop4:
	addi $t1,-1
	li $t6,0
	sb $t6, ($t1)

	#la $v0, SPACE
	move $v0, $a0	      # save buffer location to v0 as return value	
	move $sp, $fp         # pop callee frame off stack
	lw $ra, -4($fp)       # restore saved ra
	lw $fp, 0($fp)        # restore saved fp
	jr $ra
	

	.data
TRUE:.asciiz "true"
FALSE:.asciiz "false"
SPACE:.asciiz "Making Space For Inputed Values Is Fun."
SPACE2:.asciiz "AAA.\n"

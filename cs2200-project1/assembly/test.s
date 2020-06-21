//
//  test.s
//  
//
//  Created by Ben Melnick on 1/23/20.
//

//====testing ADDI====
//expected behavior: R6 <- R0 + 1 --> R6 = 1 WORKS
addi $t0, $zero, 1

//====testing ADD====
//expected behavior: R6 <- R6 + R6 --> R6 = 2 WORKS
add  $t0, $t0, $t0
//expected behavior: R7 <- R6 + R6 --> R7 = 4 WORKS
add  $t1, $t0, $t0

//======testing NAND=======
//NAND into $zero - nothing should happen
nand $zero, $zero, $zero
//NAND $zero into R3; expected behavior: R3 <- NOT 0
nand $a0, $zero, $zero

//========testing SW========
//expected behavior: MEM[0x14] <- R7 WORKS
sw   $t1, 20($zero)

//=======testing LW==========
//expected behavior: R8 <- MEM[0x14]  WORKS
lw   $t2, 20($zero)

//=========testing LEA=======
//expected behavior: R13 <- address of 'stack'  WORKS
lea  $sp, stack

stack: .word 0xFFFF


//
//  beqtest.s
//  
//
//  Created by Ben Melnick on 1/25/20.
//

//unsuccessful BEQ branch
addi    $a0, $zero, 1     ! a0 <- 1
beq     $zero, $a0, 16

//successful BEQ branch
//expected behavior: PC <- 17 = 0x11 WORKS
beq     $zero, $zero, 16

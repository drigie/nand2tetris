// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)

// Put your code here.

// ===== Pseudocode =====
// sum = 0
// i = 0
// WHILE i < R1     
//     sum = sum + R0
//     i = i + 1
// ENDFOR
// R2 = sum
// -----------------------

@result // Initialize sum = 0
M=0

@i  // Initialize i = 0
M=0

(LOOP)
    @i 
    D=M
    @R1
    D=D-M 
    @RETURN 
    D ; JGE // if i >= R1 EXIT loop

    @R0
    D=M
    @result
    M=M+D // sum += R0 

    @i // i++
    M=M+1
    @LOOP
    0 ; JMP

(RETURN)
    @result
    D=M
    @R2
    M=D // assign R2 <-- sum

(END)
    @END
    0;JMP // Infinite loop

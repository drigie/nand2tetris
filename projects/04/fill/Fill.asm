// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.

(LOOP)
    // Check the keyboard
    @KBD
    D = M
    
    
    @BLACK
    D ; JGT
    // ELSE
        @screenval 
        M=0 // default to 0
        @ENDIF
        0 ; JMP
    (BLACK)
        @screenval 
        M=-1
        
    (ENDIF)
    @8191 // initialize loop counter at max offset
    D=A
    @i
    M=D
    (SCREENLOOP)
        @i
        D=M
        @LOOP
        D ; JLT // termination condition

        @SCREEN
        D=D+A // address points to correct pixel group
        @screenpointer
        M=D
        @screenval
        D=M
        @screenpointer
        A=M
        M=D      

        @i
        M=M-1 // decrement i
        
        @SCREENLOOP
        0 ; JMP
            
@LOOP
0 ; JMP // infinite loop

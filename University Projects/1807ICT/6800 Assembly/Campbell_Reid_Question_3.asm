; Name: Campbell Reid
; Student number: s5220037
; Description: Reads a digit (n)
; 	between 2 and 9 (inclusive)
;	and prints that digit n / 2
;	times (rounded down)
; Address output: display buffer
;	($FB00 up to $FB03)

; Pseduo Code:

; Poll the user for a number
; between 2 and 9

; Given the number is its
; ascii variant, subtract $31,
; then compare if it less than
; equal to zero, it was not
; in the 2-9 range

; If not in range, poll for
; input again

; Given the number is its
; ascii variant, subtract $39,
; then compare if it is greater
; than zero, it was not
; in the 2-9 range

; If not in range, poll for
; input again

; Copy the input to another
; location in memory to
; preserve the ascii
; representation of the num

; Subtract $30 from the input
; to convert it from ascii to
; integer representation and
; divide by 2. This is the
; number of times the number
; should be printed (counter)

; Set the index to the start
; of the display buffer

; If the counter has reached
; zero, exit the program

; If not:
;	Print the inputted num

;	Decrease the counter
;	by one

;	Increase the index to
;	the next display position

;	Check the counter again
;	and try to print the num

; Assembler:

; Wait for user input
userInput	wai

; Load the input into acc a
			ldaa $FFFF

; Compare compare a to $31 so
; it checks whether the char
; is <= 0 when the char
; it converted to its integer
; representation and the range
; is adjusted from 2-9 to 1-8
			cmpa #$31

; Check if acc a is less
; than or equal to zero,
; indicating it was a character
; preceeding the ascii for 2
			ble userInput

; Compare compare a to $39 so
; it checks whether the char
; is less than 0 when the char
; it converted to its integer
; representation and the range
; is adjusted from 2-9 to 1-8,
; then adjusts it so only chars
; which are above the range are
; greater than 0
			cmpa #$39

; Check if acc b is greater than
; zero, indicating the character
; was outside the range
			bgt userInput

; Transfer a to b to
; make b the counter
			tab

; Subtract $30 from b to convert
; it to integer representation
			subb #$30

; Divide number by 2, floored
; (logical right shift)
; This makes acc b the number
; of times to display the number
			lsrb

; Set x to start of display
			ldx #$FB00

; Compare acc b (counter) to 0
; to see if it has finished
checkPrint	cmpb #0

; If acc b has reached 0, then
; branch to finish
			beq finish

; Store the character at the
; location in the display buffer
; defined by x
			staa 0,x

; Decrease counter (acc b)
			decb

; Increase x to next display
; memory location
			inx

; Go back to check whether to
; print character again
			jmp checkPrint

; Finish with program with no
; operation
finish		nop

; Mark end of the code
			.end
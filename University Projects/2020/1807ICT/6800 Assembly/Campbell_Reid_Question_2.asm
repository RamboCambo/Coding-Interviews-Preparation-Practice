; Name: Campbell Reid
; Student number: s5220037
; Description: Fills entire
;	display with reversed
;	alphabet
; Description of variables:
;	startChar: the ASCII value
;		for the first char to be
;		displayed (Z character)
;	stopChar: the previous data
;		value before the A char
;		(the @ character)
;	dispEnd: the next memory
;		location that isn't the
;		display buffer
; Address output: display buffer
;	($FB00 -> $FF37)

; Pseudo Code:

; Load the start of the display
; to the index

; Load the last letter of the
; alphabet into memory (Z)

; If the index has reached the
; the end of the display, exit
; the program

; If it hasn't:
;	If the current alphabet
;	ascii character is before
;	'A', then reset it to 'Z'

;	Print the current alphabet
;	to the screen

;	Point to the preceeding
;	alphabet character

;	Point to the next display
;	location and check display
;	location again

; Assembler:

; Set x to the beginning of the
; display buffer ($FB00)
			ldx #$FB00

; Load the first character into
; accumulator b (char Z)
			ldab startChar

; Check if x has reached the end
; of the display
cmpDispPos	cpx dispEnd

; If it hasn't reached the end,
; branch to increase to next
; display location
			bne incDispPos

; If it has reached the end,
; branch to finish the program
			bra finish

; Check if the current character
; is still in the alphabet
; (is it 
incDispPos	cmpb stopChar

; If isn't in the alphabet,
; reset the character
			beq resetChar

; Store the current character
; to the current display
; location
displayChar stab 0,x

; Increase x to point to the
; next display location
			inx

; Decrease b to make it the
; preceeding alphabet character
			decb

; Jump back to loop to check
; and print alphabet again
			jmp cmpDispPos

; Load Z into accumulator b
resetChar 	ldab startChar

; Jump back to display the char
			jmp displayChar

; Finish the program with no
; operation to be done
finish		nop

; Mark where the variables will
; be stored in memory
			.org $50
startChar	.byte $5A
stopChar	.byte $40
dispEnd		.word $FF38

; Mark end of code
			.end
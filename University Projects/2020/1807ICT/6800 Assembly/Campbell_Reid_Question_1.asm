; Name: Campbell Reid
; Student number: s5220037
; Description: Fills entire
;	display with forward slash
;	characters
; Description of variables:
;	slash: the ASCII value for
;		the slash char
;	dispEnd: the next memory
;		location that isn't the
;		display buffer
; Address output: display buffer
;	($FB00 -> $FF37)

; Pseudo Code:

; Set index register to keep
; track of where to print to
; screen

; Load slash ascii value into
; memory so it can be printed

; Check if index has reached
; the end of the display

; If it hasn't, print the slash
; and point the index to the
; next display location and go
; back to check index again

; If it has, exit the program

; Assembler:

; Set X to the start of the
; display buffer
			ldx #$FB00

; Load the the slash ascii value
; into Acc B so it can be saved
; to the display when needed
			ldab slash

; Check if x is equal to the
; next location after the
; display buffer
cmpDispPos	cpx dispEnd

; If it isn't, branch to
; write a slash character and
; increase x
			bne incDispPos

; If it reached this point, it
; meant x had reached the end of
; the display buffer, so branch
; to finish
			bra finish

; Store the slash character at
; at the memory location x (in
; the display buffer)
incDispPos	stab 0,x

; Increase x to point to the
; next memory location in the
; display buffer
			inx

; Jump back to checking the
; x against the end of the
; display buffer
			jmp cmpDispPos

; No operation to stop executing
; the the program
finish		nop

; Set the memory location of the
; variables to $50
			.org $50
slash		.byte $2F
dispEnd		.word $FF38

; Mark end of program
			.end
; LEX_REG_TEST
; Test program for lexer special registers 
;
; Stefan Wong 2019

; Set the delay timer 
LD DT 0xF
; Set the sound timer 
LD ST 0xF
; Put the delay timer value into V9
LD V9 DT
; Also put that value in V8
LD V8 DT

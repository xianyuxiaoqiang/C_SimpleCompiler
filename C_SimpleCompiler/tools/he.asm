

.MODEL SMALL
.STACK 200h

CR equ 13
LF equ 10
TERMINATOR equ '$'

.DATA

Message DB 'Hello, World !',0AH,0DH,'$'

.CODE

Main PROC
;lea ax, Message
;mov ds, ax
mov ax, DGROUP
mov ds, ax
lea ax, Message
mov dx, ax
mov ah, 9
int 21h

mov ax, 4c00h
int 21h
Main ENDP

END main

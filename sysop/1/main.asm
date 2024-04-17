;
; Author: Michał Płachta
;

org 0x7c00
jmp 0:start

start:
mov ax, cs
mov ds, ax
mov es, ax
mov ss, ax
mov sp, 0x8000

load_text:
mov bx, 0x7e00
mov ah, 0x02
mov al, 1
mov ch, 0
mov cl, 2
mov dh, 0
mov dl, 0x80
int 0x13

; Initial best time - worst possible.
push WORD 0xFFFF

main_loop:
; Save clock ticks
xor ax, ax
int 0x1A        ; Puts 32bit number of ticks in cx:dx
push dx
xor ax, ax
push ax ; Lines counter. At start 0.

; Clear the screen
mov dx, 25
loop_newlines_init:
mov ah, 0xE
mov al, 10
xor bx, bx
int 0x10
dec dx
test dx, dx
jnz loop_newlines_init

; Print carriage return
mov ah, 0xE
mov al, 13
xor bx, bx
int 0x10
xor di, di

xor si, si
typing_loop:
xor di, di

write_textline_loop:
; Load the next character and print it
mov ah, 0xE
mov bx, 0x7e00
add bx, di
add bx, si
mov al, byte [bx]
xor bx, bx

; Check if we reached new line. If so don't print it and quit loop.
cmp al, 10
je textline_written
test al, al
jz textline_written

int 0x10
inc di
jmp write_textline_loop

textline_written:

; Print carriage return
mov ah, 0xE
mov al, 13
xor bx, bx
int 0x10
xor di, di

type_textline_loop:
xor ax, ax
int 0x16

cmp al, 27
jne after_if_reset
; Restart the test
pop ax ; Line counter
pop ax ; Tick counter
jmp main_loop

after_if_reset:

mov bx, 0x7e00
add bx, di
add bx, si
mov ah, byte [bx]
cmp ah, 10
je if_newline
test ah, ah
jz if_newline ; If file doesn't end in newline.

; Not a new line. Simply compare input with expected value.

cmp ah, al
je correct_input

incorrect_input:
mov ah, 0xE
xor di, di
xor bx, bx
mov al, 13
int 0x10
jmp type_textline_loop

correct_input:
inc di
; Move the cursor once to the right:
; Load cursor position.
mov ah, 0x3
xor bx, bx
int 0x10
; Cursor position in dx (row, column). Increment and set.
inc dl
mov ah, 0x2
int 0x10

jmp type_textline_loop

; Check if input is ENTER(Carriage return). If so go to next line or end the
; test depening on what row we are on. Else ?
if_newline:
cmp al, 13
jne incorrect_input
add si, di
inc si ; For the new line character

; Print newline
mov ah, 0xE
mov al, 10
xor bx, bx
int 0x10

; Print carriage return
mov al, 13
int 0x10

pop ax
cmp ax, 6
je end_of_test

inc ax
push ax
jmp typing_loop


end_of_test:
; End of the test. Display the times.

; 17 times new line
mov dx, 17
loop_newlines:
mov ah, 0xE
mov al, 10
xor bx, bx
int 0x10
dec dx
test dx, dx
jnz loop_newlines

xor ax, ax
int 0x1A
pop ax
sub dx, ax
mov ax, dx

; Divide by 18 as 18 ticks = 1 second
xor dx, dx
mov bx, 18
div bx
; ax now stores time of current run in seconds

mov di, ax ; preserve the value
call bin_to_dec_to_stack
mov cx, si
call write_num_from_stack
; Print 's'
mov ah, 0xE
mov al, 115
xor bx, bx
int 0x10

; now stack is (best_value)
pop ax
cmp ax, di
ja new_best_time

; Old time is better
mov di, ax
mov bp, cx
call bin_to_dec_to_stack

; Fill the line
mov ax, 77
sub ax, bp
sub ax, si
mov cx, si
mov si, ax
loop_spaces:
test si, si
jz after_loop_spaces
mov ah, 0xE
mov al, 45
xor bx, bx
int 0x10
dec si
jmp loop_spaces
after_loop_spaces:

mov si, cx
call write_num_from_stack

; Print 's'
mov ah, 0xE
mov al, 115
xor bx, bx
int 0x10

push di ; Push the best time
jmp result_written


new_best_time:
; di - new best time, cx - new time decimal length

; Fill the line
mov ax, 77
sub ax, cx
sub ax, cx
mov si, ax
loop_spaces_2:
cmp si, 0
je after_loop_spaces_2
mov ah, 0xE
mov al, 45
xor bx, bx
int 0x10
dec si
jmp loop_spaces_2
after_loop_spaces_2:

mov ax, di
call bin_to_dec_to_stack
call write_num_from_stack
; Push the best time
push di

; Print 's'
mov ah, 0xE
mov al, 115
xor bx, bx
int 0x10

result_written:
; Wait for ESCAPE and then start again

; Print carriage return
mov ah, 0xE
xor bx, bx
mov al, 13
int 0x10

escape_waiting_loop:
xor ax, ax
int 0x16
cmp al, 27
jne escape_waiting_loop

mov ah, 0x9
mov al, 10
mov bh, 0x7
mov cx, 23
int 0x10

jmp main_loop


;
; Functions used in main program: 
;


write_num_from_stack:
; si - length of number
; doesn't preserve si
; uses ax, bx, dx
pop dx
loop_wnfs:
cmp si, 0
je end_wnfs

dec si
pop ax
mov ah, 0xE
xor bx, bx
int 0x10
jmp loop_wnfs

end_wnfs:

push dx
ret


bin_to_dec_to_stack:
; Changes bin number to dec and puts the digits on stack.
; ax - input number
; output:
; si - number length in decimal
pop cx
xor si, si
loop_time_dec:
xor dx, dx
mov bx, 10
div bx

inc si
add dl, 48
push dx

cmp ax, 0
jne loop_time_dec

push cx
ret

times 510 - ($ - $$) db 0
dw 0xaa55
%include "macro_print.asm"
global core
extern get_value
extern put_value

section .data

n: dq N

section .bss

sync: resq N            ;; Na kogo czeka i-ty wątek.
exchange: resq N        ;; Jaką liczbę daje i-ty wątek.
waiting: resb N         ;; Czy i-ty wątek czeka na wymianę. 0 - nie, 1 - tak
                        ;; Pełni jednocześnie rolę blokady.
done: resb N            ;; Do synchronizacji maszyn przy wymianie.

section .text

;; uint64_t core(uint64_t n, char const *p)
;; rdi - numer procesu
;; rsi - adres ciągu instrukcji
;; r8 - do iterowania po instrukcjach
;; r9 - liczba wartości na stosie
core:
        xor     r9, r9
        xor     r8, r8

        loop:
        xor     rax, rax
        mov     al, [rsi + r8]
        cmp     al, 0
        je      end_loop
        cmp     al, '+'
        je      plus
        cmp     al, '*'
        je      multiply
        cmp     al, '-'
        je      minus
        cmp     al, 'n'
        je      core_num
        cmp     al, 'B'
        je      shift
        cmp     al, 'C'
        je      discard
        cmp     al, 'D'
        je      duplicate
        cmp     al, 'E'
        je      swap
        cmp     al, 'G'
        je      gv_call
        cmp     al, 'P'
        je      pv_call
        cmp     al, 'S'
        je      synchronise
        ;; Znak jest cyfrą.
        jmp     digit

        plus:
        pop     rax
        add     [rsp], rax
        dec     r9
        jmp     loop_inc

        multiply:
        pop     rax
        imul    qword [rsp]
        mov     [rsp], rax
        dec     r9
        jmp     loop_inc

        minus:
        neg     qword [rsp]
        jmp     loop_inc

        core_num:
        push    rdi
        inc     r9
        jmp     loop_inc

        shift:
        dec     r9
        pop     rax
        cmp     qword [rsp], 0
        je      loop_inc
        add     r8, rax
        jmp     loop_inc

        discard:
        dec     r9
        pop     rax
        jmp     loop_inc

        duplicate:
        mov     rax, [rsp]
        push    rax
        inc     r9
        jmp     loop_inc

        swap:
        mov     rax, [rsp]
        mov     rdx, [rsp + 8]
        mov     [rsp + 8], rax
        mov     [rsp], rdx
        jmp     loop_inc

        gv_call:
        push    rdi
        push    rsi
        push    r8
        push    r9
        mov     rdx, rsp
        and     rdx, 0x8
        test    rdx, rdx
        jnz     gv_call_fix_rsp                 ;; rsp mod 16 != 0, trzeba to naprawić.
        call    get_value
        gv_call_after_fix:
        pop     r9
        pop     r8
        pop     rsi
        pop     rdi
        push    rax
        inc     r9
        jmp     loop_inc

        gv_call_fix_rsp:
        sub     rsp, 8
        call    get_value
        add     rsp, 8
        jmp     gv_call_after_fix

        pv_call:
        pop     rax
        push    rdi
        push    rsi
        push    r8
        push    r9
        mov     rsi, rax
        mov     rdx, rsp
        and     rdx, 0x8
        test    rdx, rdx
        jnz     pv_call_fix_rsp                 ;; rsp mod 16 != 0, trzeba to naprawić.
        call    put_value
        pv_call_after_fix:
        pop     r9
        pop     r8
        pop     rsi
        pop     rdi
        dec     r9
        jmp     loop_inc

        pv_call_fix_rsp:
        sub     rsp, 8
        call    get_value
        add     rsp, 8
        jmp     pv_call_after_fix

        digit:
        sub     al, '0'
        push    rax
        inc     r9
        jmp     loop_inc

        synchronise:
        pop     rax
        lea     r10, [rel sync]
        mov     [r10 + 8 * rdi], rax
        pop     rdx
        lea     r10, [rel exchange]
        mov     [r10 + 8 * rdi], rdx
        lea     r10, [rel waiting]
        mov     byte [r10 + rdi], 1
        ;; Próbujemy zdobyć na wyłączność drugą maszynę. Ona również musi
        ;; czekać (waiting[j] == 1) i musi czekać na nas (sync[j] == i).
        busy_wait:
        xor     dl, dl
        lea     r10, [rel waiting]
        lock xchg    [r10 + rax], dl
        test    dl, dl
        jz      busy_wait                       ;; Blokada była zamknięta.
        lea     r10, [rel sync]
        mov     rcx, [r10 + 8 * rax]
        cmp     rcx, rdi                        ;; Czy na nas czeka maszyna j-ta?
        je      synchronisation
        lea     r10, [rel waiting]
        mov     [r10 + rax], dl                 ;; Przywróć poprzedni stan blokady.
        jmp     busy_wait

        synchronisation:
        lea     r10, [rel exchange]
        mov     rdx, [r10 + 8 * rax]            ;; Bierzemy wartość od drugiej maszyny.
        push    rdx
        ;; Synchronizacja z drugą maszyną. Informujemy ją że skończyliśmy, wstawiając
        ;; jedynkę do tabeli done. Następnie czekamy, aż ona też skończy.
        lea     r10, [rel done]
        mov     byte [r10 + rax], 1
        wait_loop:
        lea     r10, [rel done]
        mov     cl, [r10 + rdi]
        test    cl, cl
        jz      wait_loop
        lea     r10, [rel done]
        mov     byte [r10 + rdi], 0
        dec     r9
        jmp     loop_inc

        loop_inc:
        inc     r8
        jmp     loop

        end_loop:
        pop     rax             ;; Wynik funkcji core
        dec     r9
        imul    r9, 8
        add     rsp, r9         ;; Przywracamy stos do pierwotnego stanu.
        ret

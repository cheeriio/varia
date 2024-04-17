nasm -DN=2 -f elf64 -F dwarf -w+all -w+error -o core.o core.asm
gcc -c -g -Wall -Wextra -std=c17 -O2 -o example.o example.c
gcc -z noexecstack -o example core.o example.o -lpthread

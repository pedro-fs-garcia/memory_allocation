#include <stdio.h>

int main() {
    // Um endereço típico de início do Kernel Space em x86-64
    char *kernel_address = (char *)0xFFFF880000000000;

    printf("Tentando ler o endereço do Kernel: %p\n", kernel_address);

    // Esta linha vai causar o erro
    char data = *kernel_address;

    printf("Consegui ler: %c\n", data); // Nunca chegará aqui
    return 0;
}
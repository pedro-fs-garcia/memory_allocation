#include <stdio.h>

int main() {
    char user_var = 'U';
    char *user_address = &user_var;

    printf("Tentando ler o endereço do User Space: %p\n", user_address);

    char user_data = *user_address;
    printf("Consegui ler do User Space: %c\n", user_data);

    // Um endereço típico de início do Kernel Space em x86-64
    char *kernel_address = (char *)0xFFFF880000000000;

    printf("Tentando ler o endereço do Kernel: %p\n", kernel_address);

    char kernel_data = *kernel_address;
    printf("Consegui ler do Kernel Space: %c\n", kernel_data);

    return 0;
}

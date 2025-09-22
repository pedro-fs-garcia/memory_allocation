#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Define 1 Megabyte em bytes para clareza
#define MEGABYTE (1024 * 1024)

int main() {
    int count = 0;
    void *p;

    while (1) {
        // Tenta alocar 1 MB de memória
        p = malloc(MEGABYTE);
        if (p == NULL) {
            printf("Falha ao alocar memória! O sistema pode estar no limite.\n");
            // Continua dormindo para podermos observar o estado final
            sleep(10);
            return 1;
        }

        // Preenche o bloco de memória alocado com zeros.
        // Este passo é VITAL! Ele "suja" a página, forçando o kernel
        // a realmente providenciar uma página de memória física para ela.
        memset(p, 0, MEGABYTE);
        
        count++;
        printf("Total alocado: %d MB\n", count);

        // Espera 100 milissegundos (0.1 segundos)
        usleep(100000);
    }

    return 0;
}
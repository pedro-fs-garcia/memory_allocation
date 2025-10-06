#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MEGABYTE (1024 * 1024)

int main() {
    printf("Iniciando ataque a memoria... Processo ID: %d\n", getpid());
    printf("Abra outro terminal e execute 'dmesg -T -w' para observar o OOM Killer.\n");

    // Pausa para dar tempo ao apresentador de preparar o outro terminal
    sleep(5); 

    while (1) {
        // Aloca blocos de 100MB de uma vez para ser mais agressivo
        void *p = malloc(100 * MEGABYTE);
        if (p == NULL) {
            // Se malloc falhar, o OOM Killer provavelmente já está agindo
            sleep(1);
            continue;
        }
        // "Suja" a memória para forçar a alocação física
        memset(p, 0, 100 * MEGABYTE);
        printf(".\n");
        fflush(stdout); // Garante que o ponto seja impresso imediatamente
    }

    return 0;
}
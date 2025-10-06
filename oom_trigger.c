#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MEGABYTE (1024 * 1024)

int main() {
    printf("execute 'sudo dmesg -T -w' para observar o OOM Killer.\n");
    printf("Iniciando ataque a memoria... Processo ID: %d\n", getpid());

    sleep(5); 

    while (1) {
        void *p = malloc(100 * MEGABYTE);
        if (p == NULL) {
            printf("\nFalha ao alocar memória! O sistema pode estar no limite.\n");
            sleep(1);
            continue;
        }

        memset(p, 0, 100 * MEGABYTE);
        printf(".");
        fflush(stdout);
    }

    return 0;
}
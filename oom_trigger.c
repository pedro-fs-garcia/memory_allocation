#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void) {
    for (int i = 0; i < 10000; ++i) {
        void *p = malloc(1024 * 1024 * 10); // Aloca 10MB
        if (p == NULL) break;
        memset(p, 0, 1024 * 1024 * 10);
        printf("Alocado bloco %d\n", i);
        sleep(1);
    }
    return 0;
}
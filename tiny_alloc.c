#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

int main() {
    // Descobre o tamanho da página
    long page_size = sysconf(_SC_PAGESIZE);
    printf("Tamanho da página: %ld bytes\n\n", page_size);

    // Aloca 1 byte
    void *p = malloc(1);

    printf("Alocado 1 byte em: %p\n", p);
    printf("PID: %d\n", getpid());

    uintptr_t addr = (uintptr_t)p;
    uintptr_t page_base = addr & ~(page_size - 1);
    uintptr_t offset = addr & (page_size - 1);

    printf("Endereço base da página: %p\n", (void *)page_base);
    printf("Offset dentro da página: %lu bytes\n", offset);

    printf("\nVerifique no outro terminal:\n");
    printf("cat /proc/%d/maps | grep heap\n", getpid());

    char filename[64];
    snprintf(filename, sizeof(filename), "/proc/%d/maps", getpid());

    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("fopen");
        free(p);
        return 1;
    }

    char line[256];
    uintptr_t heap_start = 0, heap_end = 0;
    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, "[heap]")) {
            sscanf(line, "%lx-%lx", &heap_start, &heap_end);
            break;
        }
    }
    fclose(f);

    if (heap_start && heap_end) {
        size_t heap_size = heap_end - heap_start;
        size_t num_pages = (heap_size + page_size - 1) / page_size;
        printf("\nHeap começa em: %p\n", (void *)heap_start);
        printf("Heap termina em: %p\n", (void *)heap_end);
        printf("Tamanho do heap: %zu bytes\n", heap_size);
        printf("Número de páginas alocadas: %zu\n", num_pages);
    } else {
        printf("Não foi possível determinar a região do heap.\n");
    }

    printf("\nMantendo o programa ativo por 60 segundos...\n");
    sleep(60);

    free(p);
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>

// --- Definicoes de Cores para o Terminal ---
#define COLOR_RESET   "\x1b[0m"
#define COLOR_BOLD    "\x1b[1m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_CYAN    "\x1b[36m"


#define ALLOC_MB 1024

#define PAGES_PER_UPDATE 8192

/**
 * @brief Exibe informacoes de memoria de um mapeamento especifico.
 */
void print_memory_info(pid_t pid, const char* label, const void* target_addr) {
    char smaps_path[64];
    snprintf(smaps_path, sizeof(smaps_path), "/proc/%d/smaps", pid);

    FILE *fp = fopen(smaps_path, "r");
    if (!fp) {
        perror("fopen smaps");
        return;
    }

    char addr_str[32];
    snprintf(addr_str, sizeof(addr_str), "%lx", (unsigned long)target_addr);
    char line[256];
    int found_mapping = 0;

    printf(COLOR_BOLD "%s:\n" COLOR_RESET, label);

    while (fgets(line, sizeof(line), fp)) {
        if (!found_mapping && strstr(line, addr_str) != NULL) {
            found_mapping = 1;
        }

        if (found_mapping) {
            unsigned long value_kb;
            if (sscanf(line, "Size: %lu kB", &value_kb) == 1) {
                 printf("  - Tamanho Virtual (Size): " COLOR_YELLOW "%lu KiB\n" COLOR_RESET, value_kb);
            }
            if (sscanf(line, "Rss: %lu kB", &value_kb) == 1) {
                printf("  - Memoria Fisica (Rss):  " COLOR_GREEN "%lu KiB\n" COLOR_RESET, value_kb);
                break; 
            }
        }
    }
    fclose(fp);
    printf("----------------------------------------\n");
}

int main() {
    long page_size = sysconf(_SC_PAGESIZE);
    pid_t pid = getpid();
    
    size_t total_size = ALLOC_MB * 1024 * 1024;
    long num_pages = total_size / page_size;

    printf(COLOR_BOLD "--- Demonstracao de Paginacao Gradual em Larga Escala ---\n" COLOR_RESET);
    printf("PID do Processo: " COLOR_YELLOW "%d\n" COLOR_RESET, pid);
    printf("Acompanhe o progresso em outro terminal com htop -p %d:\n", pid);
    printf("Alocando " COLOR_CYAN "%d MB" COLOR_RESET " (%ld paginas de %ld bytes).\n\n", ALLOC_MB, num_pages, page_size);

    void *mem = mmap(NULL, total_size,
                     PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (mem == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    
    print_memory_info(pid, "Estado Inicial (Apos mmap)", mem);
    
    printf(COLOR_CYAN "\nPressione ENTER para iniciar o acesso gradual as paginas...\n" COLOR_RESET);
    getchar();

    printf(COLOR_YELLOW "Acessando todas as %ld paginas. Status sera exibido a cada %d paginas...\n\n" COLOR_RESET, num_pages, PAGES_PER_UPDATE);

    for (long i = 0; i < num_pages; i++) {
        char *page_addr = (char *)mem + i * page_size;

        page_addr[0] = (char)i;

        if ((i + 1) % PAGES_PER_UPDATE == 0 || (i + 1) == num_pages) {
            char status_label[100];
            snprintf(status_label, sizeof(status_label), "Status apos acessar %ld / %ld paginas", i + 1, num_pages);
            print_memory_info(pid, status_label, mem);
        }
        usleep(100);
    }
    
    printf(COLOR_GREEN "\n\nAcesso a todas as paginas concluido.\n" COLOR_RESET);
    
    printf(COLOR_CYAN "\nDemonstracao finalizada. Pressione ENTER para liberar a memoria.\n" COLOR_RESET);
    getchar();

    munmap(mem, total_size);
    printf("Memoria liberada.\n");

    return 0;
}
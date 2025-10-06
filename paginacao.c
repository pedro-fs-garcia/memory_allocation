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

#define NUM_PAGES 10 // Numero de paginas virtuais a reservar

/**
 * @brief Exibe informacoes de memoria de um mapeamento especifico.
 *
 * Esta funcao le /proc/[pid]/smaps e procura pelo bloco de memoria
 * que comeca no endereco fornecido por 'target_addr'.
 * @param pid O ID do processo a ser inspecionado.
 * @param label Um rotulo para descrever o momento da medicao.
 * @param target_addr O endereco de memoria exato a ser procurado.
 */
void print_memory_info(pid_t pid, const char* label, const void* target_addr) {
    char smaps_path[64];
    snprintf(smaps_path, sizeof(smaps_path), "/proc/%d/smaps", pid);

    FILE *fp = fopen(smaps_path, "r");
    if (!fp) {
        perror("fopen smaps");
        return;
    }

    // Converte o endereco de memoria para uma string para busca
    char addr_str[32];
    snprintf(addr_str, sizeof(addr_str), "%lx", (unsigned long)target_addr);

    char line[256];
    int found_mapping = 0;

    printf(COLOR_BOLD "%s:\n" COLOR_RESET, label);

    while (fgets(line, sizeof(line), fp)) {
        // MODIFICADO: Procura pela linha que contem o endereco exato do nosso mapeamento
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
                // Uma vez que encontramos o Rss, ja temos toda a info deste bloco
                break; 
            }
        }
    }

    fclose(fp);
    printf("----------------------------------------\n");
}

/**
 * @brief Pausa a execucao e espera que o usuario pressione ENTER.
 */
void wait_for_enter() {
    printf(COLOR_CYAN "\n[Pressione ENTER para continuar...]" COLOR_RESET);
    // Limpa o buffer de entrada antes de esperar pelo proximo caracter
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    if (c == '\n') { // Se o ultimo getchar nao foi o \n, le de novo
        getchar();
    }
}

int main() {
    long page_size = sysconf(_SC_PAGESIZE);
    pid_t pid = getpid();
    size_t total_size = NUM_PAGES * page_size;

    printf(COLOR_BOLD "--- Demonstracao de Paginacao Sob Demanda no Linux ---\n" COLOR_RESET);
    printf("PID do Processo: " COLOR_YELLOW "%d\n" COLOR_RESET, pid);
    printf("Tamanho da Pagina do Sistema: " COLOR_YELLOW "%ld bytes\n" COLOR_RESET, page_size);
    printf("Reservando " COLOR_CYAN "%d" COLOR_RESET " paginas (" COLOR_CYAN "%zu" COLOR_RESET " bytes) de memoria virtual.\n\n", NUM_PAGES, total_size);

    void *mem = mmap(NULL, total_size,
                     PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (mem == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    printf("Memoria virtual reservada no endereco: " COLOR_BLUE "%p\n" COLOR_RESET, mem);
    
    print_memory_info(pid, "Estado Inicial (Apos mmap)", mem);
    printf(COLOR_YELLOW "--> Nota: O Rss (memoria fisica) nao e 0 pois o kernel pre-aloca a primeira pagina por otimizacao.\n" COLOR_RESET);
    
    printf(COLOR_CYAN "\n[Pressione ENTER para comecar...]" COLOR_RESET);
    getchar();

    for (int i = 0; i < NUM_PAGES; i++) {
        char *page_addr = (char *)mem + i * page_size;

        printf("\n==> Acessando Pagina %d em " COLOR_BLUE "%p" COLOR_RESET " <==\n", i, page_addr);
        printf("Escrevendo o valor " COLOR_YELLOW "%d" COLOR_RESET " no inicio da pagina...\n", i);
        
        page_addr[0] = (char)i; 

        print_memory_info(pid, "Estado Apos Acesso", mem);

        if (i < NUM_PAGES - 1) {
            printf(COLOR_CYAN "\n[Pressione ENTER para a proxima pagina...]" COLOR_RESET);
            getchar();
        }
    }

    printf("\n\n" COLOR_BOLD "--- Fase de Verificacao ---\n" COLOR_RESET);
    printf("Lendo os dados de cada pagina para provar que a memoria persiste.\n");
    getchar(); // Espera um ENTER final

    int errors = 0;
    for (int i = 0; i < NUM_PAGES; i++) {
        char *page_addr = (char *)mem + i * page_size;
        printf("Lendo da pagina %d... ", i);
        if (page_addr[0] == (char)i) {
            printf(COLOR_GREEN "OK" COLOR_RESET " (valor encontrado: %d)\n", page_addr[0]);
        } else {
            printf(COLOR_YELLOW "ERRO" COLOR_RESET "\n");
            errors++;
        }
    }

    printf("\nVerificacao concluida com " COLOR_YELLOW "%d" COLOR_RESET " erros.\n", errors);

    printf("\n" COLOR_BOLD "Demonstracao finalizada. Liberando memoria com munmap...\n" COLOR_RESET);
    munmap(mem, total_size);

    return 0;
}
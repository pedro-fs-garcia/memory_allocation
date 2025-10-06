    #include <stdio.h>
    #include <stdlib.h>

    void recursao(int nivel) {
        int stack_var_recursao = nivel;
        printf("Nível %d da Stack: %p\n", nivel, &stack_var_recursao);
        if (nivel > 0) {
            recursao(nivel - 1);
        }
    }

    int main() {
        int stack_var_main = 0;
        int *heap_var = malloc(sizeof(int));

        printf("Endereço da Stack (main): %p\n", &stack_var_main);
        printf("Endereço do Heap:         %p\n\n", heap_var);
        
        recursao(3);

        printf("\nPID do processo: %d. Aperte Enter para sair e ver o mapa de memória.\n", getpid());
        getchar(); // Pausa o programa

        free(heap_var);
        return 0;
    }
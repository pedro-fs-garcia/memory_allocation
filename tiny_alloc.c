    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>

    int main() {
        // A menor alocação possível
        void *p = malloc(1);

        printf("Alocado 1 byte. PID: %d\n", getpid());
        printf("Vá para outro terminal e execute: cat /proc/%d/maps | grep heap\n", getpid());
        
        sleep(60); // Dorme por 60s
        
        free(p);
        return 0;
    }
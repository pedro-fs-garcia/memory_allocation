# **Estrutura do Seminário: Gerenciamento de Memória no Linux (30 minutos)**

**Objetivo:** Explicar os conceitos fundamentais do gerenciamento de memória no Linux, desde a teoria da memória virtual até as ferramentas práticas de monitoramento e o comportamento do sistema sob pressão.

#### **Divisão de Tarefas e Conteúdo**

A apresentação será dividida em 4 blocos principais, que podem ser distribuídos para 3 ou 4 integrantes do grupo.

  * **Bloco 1: Introdução e Conceitos Fundamentais** (Apresentador 1)
  * **Bloco 2: Mecanismos Internos do Kernel** (Apresentador 2)
  * **Bloco 3: Demonstrações Práticas** (Apresentador 3 - o mais "hands-on" do grupo)
  * **Bloco 4: Situações Extremas e Conclusão** (Apresentador 3)

-----

### **Bloco 1: Introdução e Conceitos Fundamentais (Aprox. 7 minutos)**

**Responsável:** Apresentador 1

**Objetivo:** Nivelar o conhecimento da turma, introduzindo os conceitos abstratos que são a base de todo o sistema.

**Slides/Tópicos:**

2.  **Intodução:**

      * Breve roteiro da apresentação.
      * Por que Gerenciar Memória é Crucial? (Isolamento de processos, eficiência, abstração para o programador).

3.  **Memória Física vs. Memória Virtual (O Grande Truque):**

      * **Conceito:** Explicar que cada processo "pensa" que tem toda a memória RAM para si. Este é o espaço de endereçamento virtual.
      * **Analogia:** Use a analogia de um livro. O número da página no índice (endereço virtual) não é o mesmo número da página física do papel onde o conteúdo está impresso. O Kernel é o bibliotecário que faz essa tradução.
      * **Diagrama:** Um diagrama simples mostrando múltiplos processos, cada um com seu espaço de memória virtual (de 0 a N), todos mapeados para diferentes partes da memória RAM física.

4.  **Kernel Space vs. User Space:**

      * **Conceito:** Explicar que o espaço de memória virtual é dividido. Uma parte é exclusiva para o Kernel (gerenciamento de drivers, chamadas de sistema) e outra para a aplicação do usuário.
      * **Importância:** Proteção\! Uma aplicação de usuário não pode acessar ou corromper a memória do Kernel diretamente.
      * **Diagrama:** Mostrar um único espaço de memória virtual com uma linha dividindo a área do Kernel (Kernel Space) e a área do Usuário (User Space).

-----

### **Bloco 2: Mecanismos Internos do Kernel (Aprox. 8 minutos)**

**Responsável:** Apresentador 2

**Objetivo:** Detalhar *como* o Linux implementa os conceitos do Bloco 1.

**Slides/Tópicos:**

1.  **Paginação (Paging): A Unidade de Gerenciamento:**

      * **Conceito:** A memória (virtual e física) não é gerenciada byte a byte, mas em blocos de tamanho fixo chamados **Páginas** (geralmente 4KB).
      * **Page Tables:** Explicar que o Kernel mantém uma "tabela de tradução" (Page Table) para cada processo, mapeando as páginas virtuais para os *frames* físicos na RAM.
      * **MMU (Memory Management Unit):** Mencionar que essa tradução é acelerada por hardware (a MMU na CPU).

2.  **Demand Paging (Paginação sob Demanda):**

      * **Conceito:** O Linux é "preguiçoso". Ele só carrega uma página da aplicação (do disco para a RAM) no momento exato em que ela é necessária (quando o código tenta acessá-la).
      * **Page Fault:** Explicar que um *Page Fault* não é necessariamente um erro. É o mecanismo que sinaliza ao Kernel: "Ei, preciso da página X que ainda não está na memória". O Kernel então a busca no disco.

3.  **Swapping: Quando a RAM não é suficiente:**

      * **Conceito:** Se a memória RAM física estiver cheia, o Kernel pode pegar páginas que não estão sendo usadas recentemente e movê-las para uma área no disco chamada **Swap Space**.
      * **Impacto:** Isso libera RAM para novas aplicações, mas o acesso a uma página que está em swap é muito mais lento (I/O de disco).
      * **Diagrama:** Mostrar uma página sendo movida da RAM para o Swap no HD e outra sendo trazida de volta.

-----

### **Bloco 3: Demonstrações Práticas (Aprox. 10 minutos)**

**Responsável:** Apresentador 3

**Objetivo:** Mostrar os conceitos em ação usando o terminal. **Esta é a parte mais importante para engajar a turma.** O ideal é fazer ao vivo em uma máquina virtual Linux.

**Roteiro da Demonstração:**

1.  **Ferramenta 1: `free` - A Visão Geral:**

      * **Comando:** `free -h` (`-h` para "human-readable").
      * **O que mostrar:** Explicar as colunas `total`, `used`, `free`, `shared`, `buff/cache`, `available`.
      * **Desmistificar:** Focar na diferença entre `free` e `available`. Explicar que o `buff/cache` é memória usada pelo Kernel para acelerar o sistema, mas que pode ser liberada se uma aplicação precisar (*"Linux ate my RAM\!" is a myth*).

2.  **Ferramenta 2: `/proc/meminfo` - A Fonte da Verdade:**

      * **Comando:** `cat /proc/meminfo | head -n 5`
      * **O que mostrar:** Mostrar que ferramentas como o `free` leem as informações deste arquivo virtual. Apontar para `MemTotal`, `MemFree`, `MemAvailable`.

3.  **Ferramenta 3: `top` / `htop` - A Visão por Processo:**

      * **Comando:** `htop` (mais visual) ou `top`.
      * **O que mostrar:** Explicar as colunas de memória:
          * `VIRT`: Memória Virtual total que o processo solicitou.
          * `RES`: Memória Residente (o quanto está *realmente* na RAM física agora).
          * `SHR`: Memória Compartilhada (usada por bibliotecas, etc.).
      * **Ação:** Iniciar um programa qualquer (ex: Firefox) e ver como os valores de `RES` e `VIRT` mudam.

-----

#### **4. Demonstração de Swap em Ação (Versão em C)**

  * **Preparação:** Ter um script simples em C que aloca e utiliza muita memória lentamente. O ato de utilizar a memória com `memset` é crucial para garantir que o Sistema Operacional de fato mapeie a memória virtual alocada para páginas de memória física.

  * **Execução:** Compilar o código. Abra um terminal e rode o seguinte comando:

    ```bash
    gcc stress_mem.c -o stress_mem
    ```

  * **Execução:**

    1.  Abra dois terminais.
    2.  No primeiro, rode `watch -n 1 free -h` para ver a memória em tempo real.
    3.  No segundo, execute o programa compilado: `./stress_mem`.
    4.  Narrar o que está acontecendo: a memória `used` aumentando, `available` diminuindo e, eventualmente, o `swap used` começando a crescer.
-----

### **Bloco 4: Situações Extremas e Conclusão (Aprox. 5 minutos)**

**Responsável:** Apresentador 1 ou 2

**Objetivo:** Finalizar mostrando o que acontece quando todos os recursos se esgotam e resumir os pontos-chave.

**Slides/Tópicos:**

1.  **OOM Killer (Out of Memory Killer): O Recurso Final:**

      * **Conceito:** O que acontece quando a RAM e o Swap acabam? O sistema travaria. Para evitar isso, o Kernel ativa o OOM Killer.
      * **O que ele faz:** Ele escolhe um processo "sacrificável" (baseado em um score de "maldade") e o mata para liberar memória e salvar o sistema.
      * **Como ver:** Mencionar que o "sacrifício" é logado no kernel.
      * **Comando (para mostrar o log):** `dmesg -T | grep "Out of memory"`

2.  **Demonstração (Opcional, mas impactante): Provocando o OOM Killer:**

      * **Aviso:** Fazer isso em uma VM, pois pode deixar o sistema instável.

      * **Execução:** Compilar (`gcc oom_trigger.c -o oom_trigger`), rodar e, em outro terminal, executar `dmesg -T -w`. Em algum momento, o programa será morto ("Killed") e a mensagem do OOM aparecerá no `dmesg`.

3.  **Resumo e Conclusão:**

      * Recapitular os pontos principais: Memória Virtual abstrai a RAM física.
      * O Kernel usa Paginação para gerenciar a memória de forma eficiente.
      * Swap é uma extensão lenta da RAM.
      * Ferramentas como `free` e `htop` nos permitem observar esse sistema em ação.
      * O OOM Killer é o mecanismo de defesa do sistema.

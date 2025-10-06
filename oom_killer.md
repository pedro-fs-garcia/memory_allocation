# OOM Killer (Out of Memory Killer)

* O OOM Killer é um mecanismo de autoproteção do Kernel Linux que é ativado como último recurso quando o sistema entra em uma condição crítica de falta de memória (Out of Memory)

* função do Kernel que é chamada quando uma alocação de memória falha de forma crítica. 

* RAM e Swap completamente esgotadas

* Seu objetivo é prevenir um travamento total do sistema (kernel panic), selecionando e terminando de forma sacrificial um ou mais processos para liberar memória e permitir que o sistema continue funcionando.


## Etapas de execução

### 1. Gatilho
Um processo pede uma nova página de memória, e o Kernel, após tentar de tudo, não tem nenhuma página livre na RAM ou no Swap para oferecer.

### 2. Seleção da vítima (oom_score)
O Kernel não escolhe um processo aleatoriamente. Ele executa um algoritmo para encontrar o "pior".

 * Ele calcula uma pontuação para cada processo, conhecida como oom_score.

 * Processos que consomem muita memória (RAM + Swap) recebem uma pontuação mais alta.

 * Processos vitais do sistema ou do Kernel têm sua pontuação reduzida para serem protegidos.

* O processo com a maior oom_score é escolhido como a vítima. 

#### **oom_score_adj**
Administradores podem influenciar o oom_score através de um parâmetro chamado `oom_score_adj`.

* Faixa de Valores: Ele vai de -1000 a +1000.

* Efeito: Este valor é somado à pontuação final, influenciando drasticamente a escolha.

* -1000: Significa "nunca matar". Isso efetivamente desativa o OOM Killer para este processo, protegendo-o a todo custo. É usado para serviços críticos como o sshd (que mantém sua conexão SSH ativa).

* 0: É o valor padrão, sem ajuste manual.

* +1000: Torna o processo o alvo principal, garantindo que ele seja o primeiro a ser morto em uma situação de OOM.

**A pontuação final que o OOM Killer usa é uma combinação do oom_score (baseado no uso de memória) e do oom_score_adj (o ajuste manual).**



### 3. Execução
Uma vez que a vítima é escolhida, o Kernel envia o sinal SIGKILL (sinal 9) para ela. 
* Este sinal não pode ser capturado, ignorado ou bloqueado pelo processo. 
* É uma ordem de terminação imediata e inegociável, garantindo que a memória do processo seja liberada.



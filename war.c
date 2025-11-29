css
main.c
/ (raiz do projeto)
 ├── src/
 │    └── main.c
 ├── .vscode/
 │    ├── tasks.json
 │    └── launch.json
 └── README.md (opcional)
 {
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Compilar C",
            "type": "shell",
            "command": "gcc",
            "args": [
                "src/main.c",
                "-o",
                "war"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "problemMatcher": ["$gcc"]
        }
    ]
}
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Executar WAR",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/war",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "miDebuggerPath": "/usr/bin/gdb",
            "preLaunchTask": "Compilar C"
        }
    ]
}
src/main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_TERRITORIOS 6
#define MAX_JOGADORES   2
#define TAM_MAX_MISSAO  100

// ------------------------------------------------------------------------
// Estruturas
// ------------------------------------------------------------------------

typedef struct {
    char nome[30];
    char cor[10];
    int tropas;
} Territorio;

// ------------------------------------------------------------------------
// Variável global auxiliar
// Usada em verificarMissao, para saber de qual cor de jogador estamos falando
// sem alterar a assinatura pedida no enunciado.
// ------------------------------------------------------------------------
char corEmVerificacao[10];

// ------------------------------------------------------------------------
// Assinaturas das funções
// ------------------------------------------------------------------------

void atribuirMissao(char* destino, char* missoes[], int totalMissoes);
void exibirMissao(const char* missao);
int  verificarMissao(char* missao, Territorio* mapa, int tamanho);
void atacar(Territorio* atacante, Territorio* defensor);
void exibirMapa(Territorio* mapa, int tamanho);
void liberarMemoria(Territorio* mapa, char** missoesJogadores, int totalJogadores);

// ------------------------------------------------------------------------
// Função que sorteia uma missão e copia para o destino
// destino: ponteiro para área já alocada com malloc
// missoes: vetor de strings pré-definidas
// totalMissoes: quantidade de missões cadastradas
// ------------------------------------------------------------------------
void atribuirMissao(char* destino, char* missoes[], int totalMissoes) {
    int indice = rand() % totalMissoes;
    strcpy(destino, missoes[indice]);
}

// ------------------------------------------------------------------------
// Exibe a missão do jogador (passagem por "valor" do ponteiro)
// ------------------------------------------------------------------------
void exibirMissao(const char* missao) {
    printf("Sua missão: %s\n\n", missao);
}

// ------------------------------------------------------------------------
// Verifica se a missão foi cumprida.
// A lógica é simples e baseada em comparação de strings.
// Usa a variável global corEmVerificacao para saber qual jogador está sendo
// checado.
// ------------------------------------------------------------------------
int verificarMissao(char* missao, Territorio* mapa, int tamanho) {
    int i;

    // 1) Conquistar 3 territórios seguidos (todos da cor do jogador)
    if (strcmp(missao, "Conquistar 3 territorios seguidos") == 0) {
        int cont = 0;
        for (i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, corEmVerificacao) == 0) {
                cont++;
                if (cont >= 3)
                    return 1;
            } else {
                cont = 0;
            }
        }
        return 0;
    }

    // 2) Eliminar todas as tropas da cor vermelha
    if (strcmp(missao, "Eliminar todas as tropas da cor vermelha") == 0) {
        for (i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, "vermelho") == 0 && mapa[i].tropas > 0) {
                return 0; // ainda existe tropa vermelha
            }
        }
        return 1; // não há mais tropas vermelhas
    }

    // 3) Controlar todos os territorios
    if (strcmp(missao, "Controlar todos os territorios") == 0) {
        for (i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, corEmVerificacao) != 0) {
                return 0;
            }
        }
        return 1;
    }

    // 4) Ter pelo menos 20 tropas no total
    if (strcmp(missao, "Ter pelo menos 20 tropas no total") == 0) {
        int soma = 0;
        for (i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, corEmVerificacao) == 0) {
                soma += mapa[i].tropas;
            }
        }
        return (soma >= 20);
    }

    // 5) Ter 2 territorios com 5 ou mais tropas
    if (strcmp(missao, "Ter 2 territorios com 5 ou mais tropas") == 0) {
        int cont = 0;
        for (i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, corEmVerificacao) == 0 &&
                mapa[i].tropas >= 5) {
                cont++;
            }
        }
        return (cont >= 2);
    }

    // Missão não reconhecida
    return 0;
}

// ------------------------------------------------------------------------
// Função de ataque entre territórios
// Usa rand() para simular rolagem de dados (1 a 6).
// Se o atacante vencer: defensor muda de cor e recebe metade das tropas do atacante.
// Se perder: atacante perde 1 tropa.
// ------------------------------------------------------------------------
void atacar(Territorio* atacante, Territorio* defensor) {
    if (atacante->tropas <= 0) {
        printf("O territorio atacante nao possui tropas suficientes.\n");
        return;
    }

    int dadoAtacante = rand() % 6 + 1;
    int dadoDefensor = rand() % 6 + 1;

    printf("Dado atacante: %d | Dado defensor: %d\n", dadoAtacante, dadoDefensor);

    if (dadoAtacante > dadoDefensor) {
        int tropasTransferidas = atacante->tropas / 2;
        if (tropasTransferidas == 0) tropasTransferidas = 1;

        printf("Ataque bem-sucedido! %s conquista %s.\n",
               atacante->nome, defensor->nome);

        strcpy(defensor->cor, atacante->cor);
        defensor->tropas = tropasTransferidas;
        atacante->tropas -= tropasTransferidas;
    } else {
        if (atacante->tropas > 0)
            atacante->tropas--;

        printf("Ataque falhou! %s perde 1 tropa.\n", atacante->nome);
    }
}

// ------------------------------------------------------------------------
// Exibe o mapa na tela
// ------------------------------------------------------------------------
void exibirMapa(Territorio* mapa, int tamanho) {
    int i;
    printf("\n--- MAPA ---\n");
    for (i = 0; i < tamanho; i++) {
        printf("[%d] %-15s | Cor: %-8s | Tropas: %d\n",
               i, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
    printf("------------\n\n");
}

// ------------------------------------------------------------------------
// Libera toda a memória alocada dinamicamente
// ------------------------------------------------------------------------
void liberarMemoria(Territorio* mapa, char** missoesJogadores, int totalJogadores) {
    int i;

    if (mapa != NULL)
        free(mapa);

    if (missoesJogadores != NULL) {
        for (i = 0; i < totalJogadores; i++) {
            if (missoesJogadores[i] != NULL) {
                free(missoesJogadores[i]);
            }
        }
    }
}

// ------------------------------------------------------------------------
// Função principal
// ------------------------------------------------------------------------
int main() {
    srand((unsigned int)time(NULL));

    // -----------------------------
    // Criação do vetor de missões
    // -----------------------------
    char* missoesBase[] = {
        "Conquistar 3 territorios seguidos",
        "Eliminar todas as tropas da cor vermelha",
        "Controlar todos os territorios",
        "Ter pelo menos 20 tropas no total",
        "Ter 2 territorios com 5 ou mais tropas"
    };
    int totalMissoes = sizeof(missoesBase) / sizeof(missoesBase[0]);

    // -----------------------------
    // Alocação dinâmica do mapa
    // -----------------------------
    int totalTerritorios = MAX_TERRITORIOS;
    Territorio* mapa = (Territorio*)calloc(totalTerritorios, sizeof(Territorio));
    if (mapa == NULL) {
        printf("Erro ao alocar memoria para o mapa.\n");
        return 1;
    }

    // Exemplo de inicialização de territórios
    // (em um trabalho maior isso viria de arquivo ou entrada do usuário)
    strcpy(mapa[0].nome, "Territorio A");
    strcpy(mapa[0].cor,  "azul");
    mapa[0].tropas = 5;

    strcpy(mapa[1].nome, "Territorio B");
    strcpy(mapa[1].cor,  "azul");
    mapa[1].tropas = 3;

    strcpy(mapa[2].nome, "Territorio C");
    strcpy(mapa[2].cor,  "azul");
    mapa[2].tropas = 4;

    strcpy(mapa[3].nome, "Territorio D");
    strcpy(mapa[3].cor,  "vermelho");
    mapa[3].tropas = 4;

    strcpy(mapa[4].nome, "Territorio E");
    strcpy(mapa[4].cor,  "vermelho");
    mapa[4].tropas = 5;

    strcpy(mapa[5].nome, "Territorio F");
    strcpy(mapa[5].cor,  "vermelho");
    mapa[5].tropas = 3;

    // Cores dos jogadores (apenas exemplo simples)
    const char* coresJogadores[MAX_JOGADORES] = { "azul", "vermelho" };

    // -----------------------------------------------
    // Alocação dinâmica das missões de cada jogador
    // -----------------------------------------------
    char* missoesJogadores[MAX_JOGADORES];
    int i;
    for (i = 0; i < MAX_JOGADORES; i++) {
        missoesJogadores[i] = (char*)malloc(TAM_MAX_MISSAO * sizeof(char));
        if (missoesJogadores[i] == NULL) {
            printf("Erro ao alocar memoria para a missao do jogador %d.\n", i + 1);
            liberarMemoria(mapa, missoesJogadores, MAX_JOGADORES);
            return 1;
        }
        atribuirMissao(missoesJogadores[i], missoesBase, totalMissoes);
    }

    // Exibir a missão de cada jogador apenas uma vez (início do jogo)
    for (i = 0; i < MAX_JOGADORES; i++) {
        printf("Jogador %d (%s):\n", i + 1, coresJogadores[i]);
        exibirMissao(missoesJogadores[i]);
    }

    // -----------------------------
    // Loop principal do "jogo"
    // -----------------------------
    int jogadorAtual = 0;
    int vencedor = -1;
    int opcao;

    do {
        printf("------- Turno do Jogador %d (cor %s) -------\n",
               jogadorAtual + 1, coresJogadores[jogadorAtual]);

        exibirMapa(mapa, totalTerritorios);

        printf("Escolha uma opcao:\n");
        printf("1 - Atacar\n");
        printf("0 - Sair do jogo\n");
        printf("Opcao: ");
        scanf("%d", &opcao);

        if (opcao == 1) {
            int idxAtacante, idxDefensor;

            printf("Indice do territorio ATACANTE (0 a %d): ", totalTerritorios - 1);
            scanf("%d", &idxAtacante);

            printf("Indice do territorio DEFENSOR (0 a %d): ", totalTerritorios - 1);
            scanf("%d", &idxDefensor);

            // Valida indices
            if (idxAtacante < 0 || idxAtacante >= totalTerritorios ||
                idxDefensor < 0 || idxDefensor >= totalTerritorios) {
                printf("Indice invalido.\n");
            }
            else if (idxAtacante == idxDefensor) {
                printf("Nao e possivel atacar o mesmo territorio.\n");
            }
            // Jogador só pode atacar com território da sua cor
            else if (strcmp(mapa[idxAtacante].cor, coresJogadores[jogadorAtual]) != 0) {
                printf("Voce so pode atacar usando territorios da sua cor (%s).\n",
                       coresJogadores[jogadorAtual]);
            }
            // Só pode atacar território inimigo
            else if (strcmp(mapa[idxAtacante].cor, mapa[idxDefensor].cor) == 0) {
                printf("Voce nao pode atacar territorios da mesma cor.\n");
            }
            else {
                atacar(&mapa[idxAtacante], &mapa[idxDefensor]);
            }
        }

        // Após o turno, verifica silenciosamente se o jogador cumpriu a missão
        strcpy(corEmVerificacao, coresJogadores[jogadorAtual]);
        if (verificarMissao(missoesJogadores[jogadorAtual], mapa, totalTerritorios)) {
            vencedor = jogadorAtual;
            break;
        }

        // Troca de jogador
        jogadorAtual = (jogadorAtual + 1) % MAX_JOGADORES;

    } while (opcao != 0);

    // -----------------------------
    // Resultado final
    // -----------------------------
    if (vencedor != -1) {
        printf("\n*** Jogador %d (cor %s) CUMPRIU SUA MISSAO! ***\n",
               vencedor + 1, coresJogadores[vencedor]);
        printf("Missao: %s\n", missoesJogadores[vencedor]);
    } else {
        printf("\nJogo encerrado sem vencedor por missao.\n");
    }

    // -----------------------------
    // Liberação da memória
    // -----------------------------
    liberarMemoria(mapa, missoesJogadores, MAX_JOGADORES);

    return 0;
}



// ============================================================================
//         PROJETO WAR ESTRUTURADO - DESAFIO DE CÓDIGO
// ============================================================================
//        
// ============================================================================
//
// OBJETIVOS:
// - Modularizar completamente o código em funções especializadas.
// - Implementar um sistema de missões para um jogador.
// - Criar uma função para verificar se a missão foi cumprida.
// - Utilizar passagem por referência (ponteiros) para modificar dados e
//   passagem por valor/referência constante (const) para apenas ler.
// - Foco em: Design de software, modularização, const correctness, lógica de jogo.
//
// ============================================================================

// Inclusão das bibliotecas padrão necessárias para entrada/saída, alocação de memória, manipulação de strings e tempo.

// --- Constantes Globais ---
// Definem valores fixos para o número de territórios, missões e tamanho máximo de strings, facilitando a manutenção.

// --- Estrutura de Dados ---
// Define a estrutura para um território, contendo seu nome, a cor do exército que o domina e o número de tropas.

// --- Protótipos das Funções ---
// Declarações antecipadas de todas as funções que serão usadas no programa, organizadas por categoria.
// Funções de setup e gerenciamento de memória:
// Funções de interface com o usuário:
// Funções de lógica principal do jogo:
// Função utilitária:

// --- Função Principal (main) ---
// Função principal que orquestra o fluxo do jogo, chamando as outras funções em ordem.
int main() {
    // 1. Configuração Inicial (Setup):
    // - Define o locale para português.
    // - Inicializa a semente para geração de números aleatórios com base no tempo atual.
    // - Aloca a memória para o mapa do mundo e verifica se a alocação foi bem-sucedida.
    // - Preenche os territórios com seus dados iniciais (tropas, donos, etc.).
    // - Define a cor do jogador e sorteia sua missão secreta.

    // 2. Laço Principal do Jogo (Game Loop):
    // - Roda em um loop 'do-while' que continua até o jogador sair (opção 0) ou vencer.
    // - A cada iteração, exibe o mapa, a missão e o menu de ações.
    // - Lê a escolha do jogador e usa um 'switch' para chamar a função apropriada:
    //   - Opção 1: Inicia a fase de ataque.
    //   - Opção 2: Verifica se a condição de vitória foi alcançada e informa o jogador.
    //   - Opção 0: Encerra o jogo.
    // - Pausa a execução para que o jogador possa ler os resultados antes da próxima rodada.

    // 3. Limpeza:
    // - Ao final do jogo, libera a memória alocada para o mapa para evitar vazamentos de memória.

    return 0;
}

// --- Implementação das Funções ---

// alocarMapa():
// Aloca dinamicamente a memória para o vetor de territórios usando calloc.
// Retorna um ponteiro para a memória alocada ou NULL em caso de falha.

// inicializarTerritorios():
// Preenche os dados iniciais de cada território no mapa (nome, cor do exército, número de tropas).
// Esta função modifica o mapa passado por referência (ponteiro).

// liberarMemoria():
// Libera a memória previamente alocada para o mapa usando free.

// exibirMenuPrincipal():
// Imprime na tela o menu de ações disponíveis para o jogador.

// exibirMapa():
// Mostra o estado atual de todos os territórios no mapa, formatado como uma tabela.
// Usa 'const' para garantir que a função apenas leia os dados do mapa, sem modificá-los.

// exibirMissao():
// Exibe a descrição da missão atual do jogador com base no ID da missão sorteada.

// faseDeAtaque():
// Gerencia a interface para a ação de ataque, solicitando ao jogador os territórios de origem e destino.
// Chama a função simularAtaque() para executar a lógica da batalha.

// simularAtaque():
// Executa a lógica de uma batalha entre dois territórios.
// Realiza validações, rola os dados, compara os resultados e atualiza o número de tropas.
// Se um território for conquistado, atualiza seu dono e move uma tropa.

// sortearMissao():
// Sorteia e retorna um ID de missão aleatório para o jogador.

// verificarVitoria():
// Verifica se o jogador cumpriu os requisitos de sua missão atual.
// Implementa a lógica para cada tipo de missão (destruir um exército ou conquistar um número de territórios).
// Retorna 1 (verdadeiro) se a missão foi cumprida, e 0 (falso) caso contrário.

// limparBufferEntrada():
// Função utilitária para limpar o buffer de entrada do teclado (stdin), evitando problemas com leituras consecutivas de scanf e getchar.

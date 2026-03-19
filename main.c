#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LIMPAR_TELA system("cls")
/* ===================== CONSTANTES ===================== */
#define MAX_CPF 12
#define MAX_NOME 100
#define MAX_CODIGO 10

#define MAX_DISCENTES 100
#define MAX_CURSOS 30
#define MAX_TURMAS 100

#define ARQ_DISCENTES "discentes.txt"
#define ARQ_CURSOS "cursos.txt"
#define ARQ_TURMAS "turmas.txt"

/* ===================== ESTRUTURAS ===================== */
typedef struct {
    char cpf[MAX_CPF];
    char nome[MAX_NOME];
    int idade;
} Discente;

typedef struct {
    char codigo[MAX_CODIGO];
    char nome[MAX_NOME];
    int horas;
    int vagas;
    int participantes;
} Curso;

typedef struct {
    int numero;
    char cpf[MAX_CPF];
    char codigo_curso[MAX_CODIGO];
    int ano;
    float nota;
    int hora_participacao;
} Turma;

/* ========== ARRAY GLOBAIS ========== */
Discente discentes[MAX_DISCENTES];
int total_discentes = 0;

Curso cursos[MAX_CURSOS];
int total_cursos = 0;

Turma turmas[MAX_TURMAS];
int total_turmas = 0;

/* ================= FUNCOES AUXILIARES ================= */
void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void pausar() {
    printf("\n  Pressione ENTER para continuar...");
    limpar_buffer();

    getchar();
}

void limpar_tela() { system("cls"); }

void cabecalho(const char *titulo) {
    limpar_tela();
    printf("   ESCOLA TOP-ART-BARE  |  %s\n", titulo);
    printf("\n");
}

/* =============== VALIDACOES ========================= */
int cpf_valido(const char *cpf) {
    int i;
    if (strlen(cpf) != 11) return 0;

    for (i = 0; i < 11; i++) {
        if (!isdigit(cpf[i])) {
            return 0;
        }
    }
    return 1; // Passou em todos os testes
}

/* ===================== LEITURA SEGURA ===================== */

// ler entrada estabelecendo uma legenda e um limite de números
void ler_string(const char legenda[], char dest[], int max) {
    printf("%s", legenda);
    fgets(dest, max, stdin);
    dest[strcspn(dest, "\n")] = '\0';
}

int ler_inteiro(const char legenda[], int min, int max_val) {
    int valor;
    char buf[50];
    while (1) {
        printf("%s", legenda);
        fgets(buf, sizeof(buf), stdin);
        if (sscanf(buf, "%d", &valor) == 1 && valor >= min && valor <= max_val)
            return valor;
        printf("  [!] Valor inválido. Digite entre %d e %d.\n", min, max_val);
    }
}

/* ===================== DISCENTES - ARQUIVO ===================== */
int contar_discentes() {
    FILE *f = fopen(ARQ_DISCENTES, "r");
    if (!f) return 0;

    int cont = 0;
    char linha[200];
    while (fgets(linha, sizeof(linha), f)) cont++;

    fclose(f);
    return cont;
}

void carregar_discentes() {
    FILE *f = fopen(ARQ_DISCENTES, "r");
    if (!f) { printf("Error ao carregar"); pausar(); return; }

    total_discentes = 0;
    while (total_discentes < MAX_DISCENTES &&
        fscanf(f,
            "%14[^|]|%99[^|]|%d\n",
            discentes[total_discentes].cpf,
            discentes[total_discentes].nome,
            &discentes[total_discentes].idade) == 3) {
        total_discentes++;
    }
    fclose(f);
}

void salvar_discentes() {
    FILE *f = fopen(ARQ_DISCENTES, "w");
    if (!f) { printf("  [!] Erro ao salvar discentes.\n"); return; }

    int i;
    for (i = 0; i < total_discentes; i++)
        fprintf(f, "%s|%s|%d\n",
            discentes[i].cpf,
            discentes[i].nome,
            discentes[i].idade);
    fclose(f);
}

int cpf_discente_existe(const char *cpf) {
    //verificar depois
    /* int total;
    Discente lista[] = carregar_discentes(&total);
    int encontrado = 0;
    int i;

    for (i = 0; i < total; i++) {
        if (strcmp(lista[i].cpf, cpf) == 0) { encontrado = 1; break; }
    }

    free(lista);
    return encontrado; */
}

/* ===================== FUNÇÕES BUSCA AUXILIAR ===================== */

int buscar_discente_por_cpf(Discente *lista, int total, const char *cpf,
    Discente *dest) {
    int i;
    for (i = 0; i < total; i++) {
        if (strcmp(lista[i].cpf, cpf) == 0) {
            if (dest) *dest = lista[i];
            return i;
        }
    }
    return -1;
}

/* ============== MENU DISCENTE ================*/
void inserir_discente() {
    cabecalho("DISCENTES > INSERIR");

    // formulário
    Discente novo;
    ler_string("  CPF (apenas dígitos): ", novo.cpf, MAX_CPF);
    limpar_buffer();

    ler_string("  Nome: ", novo.nome, MAX_NOME);
    if (strlen(novo.nome) < 3) {
        printf("  [!] Nome muito curto.\n");
        pausar(); return;
    }

    novo.idade = ler_inteiro("  Idade: ", 5, 120);

    discentes[total_discentes] = novo;
    total_discentes++;
    salvar_discentes();

    printf("\n  [OK] Discente cadastrado com sucesso!\n");
    pausar();
}

void menu_discentes() {
    int op;
    do {
        cabecalho("DISCENTES");
        printf("  [1] Inserir\n");
        printf("  [2] Editar\n");
        printf("  [3] Excluir\n");
        printf("  [4] Pesquisar\n");
        printf("  [0] Voltar\n\n");
        op = ler_inteiro("  Opcao: ", 0, 4);
        switch (op) {
            case 1:
                inserir_discente();
                break;
                // case 2: editar_discente(); break; //TODO
                // case 3: excluir_discente(); break;
                // case 4: pesquisar_discente(); break;
        }
    } while (op != 0);
}

void menu_principal() {
    int op;
    do {
        cabecalho("MENU PRINCIPAL");
        printf(" [1] Discentes\n");
        printf(" [2] Cursos\n");
        printf(" [3] Turmas\n");
        printf(" [4] Relatorio\n");
        printf(" [0] Sair\n");

        /* Validando numeros inteiros */
        op = ler_inteiro("Opcao: ", 0, 4);

        switch (op) {
            case 1:
                menu_discentes();
                break;
            case 2:
                printf("Vc esta na op2");
                break;
            case 3:
                printf("Vc esta na op3");
                break;
            case 4:
                printf("Vc esta na op4");
                break;
            case 0:
                cabecalho("ATÉ LOGO!");
                printf("  Sistema encerrado.\n\n");
                break;
        }
    } while (op != 0);
}

int main() {
    system("chcp 65001 > nul");
    carregar_discentes();

    menu_principal();

    return 0;
}

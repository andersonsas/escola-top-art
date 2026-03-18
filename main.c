#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define LIMPAR_TELA system("cls")
/* ===================== CONSTANTES ===================== */
#define MAX_CPF 15
#define MAX_NOME 100
#define MAX_CODIGO 10

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
} Turmas;

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

void limpar_tela() {
    system("cls");
}

void cabecalho(const char *titulo) {
    limpar_tela();
    printf("   ESCOLA TOP-ART-BARE  |  %s\n", titulo);
    printf("\n");
}

/* =============== VALIDACOES =========================   */
int cpf_valido(const char *cpf) {
    int i;
    if (strlen(cpf) != 11) return 0;

    for (i = 0; i < 11; i++) {
        if (!isdigit((unsigned char)cpf[i])) {
            return 0;
        }
    }
    return 1; // Passou em todos os testes
}

/* ===================== LEITURA SEGURA ===================== */

void ler_string(const char *legenda, char *dest, int max) {
    printf("%s", legenda);
    fgets(dest, max, stdin);
    dest[strcspn(dest, "\n")] = '\0';
}

int ler_inteiro(const char *legenda, int min, int max_val) {
    int val;
    char buf[50];
    while (1) {
        printf("%s", legenda);
        fgets(buf, sizeof(buf), stdin);
        if (sscanf(buf, "%d", &val) == 1 && val >= min && val <= max_val) return val;
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

Discente *carregar_discentes(int *total) {
    *total = contar_discentes();
    if (*total == 0) return NULL;
    Discente *lista = (Discente *)malloc(*total * sizeof(Discente));
    if (!lista) { *total = 0; return NULL; }
    FILE *f = fopen(ARQ_DISCENTES, "r");
    if (!f) { free(lista); *total = 0; return NULL; }
    int i = 0;
    while (i < *total && fscanf(f, "%11[^|]|%99[^|]|%d\n",
        lista[i].cpf, lista[i].nome, &lista[i].idade) == 3)
        i++;
    *total = i;
    fclose(f);
    return lista;
}

void salvar_discentes(Discente *lista, int total) {
    FILE *f = fopen(ARQ_DISCENTES, "w");
    if (!f) { printf("  [!] Erro ao salvar discentes.\n"); return; }
    int i;
    for (i = 0; i < total; i++)
        fprintf(f, "%s|%s|%d\n", lista[i].cpf, lista[i].nome, lista[i].idade);
    fclose(f);
}

int cpf_discente_existe(const char *cpf) {
    int total;
    Discente *lista = carregar_discentes(&total);
    int encontrado = 0;
    int i;
    for (i = 0; i < total; i++) {
        if (strcmp(lista[i].cpf, cpf) == 0) { encontrado = 1; break; }
    }
    free(lista);
    return encontrado;
}

/* ===================== FUNÇÕES BUSCA AUXILIAR ===================== */

int buscar_discente_por_cpf(Discente *lista, int total, const char *cpf, Discente *dest) {
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
    Discente novo;

    ler_string("  CPF (apenas dígitos): ", novo.cpf, MAX_CPF);
    if (!cpf_valido(novo.cpf)) { printf("  [!] CPF inválido.\n"); pausar(); return; }
    if (cpf_discente_existe(novo.cpf)) { printf("  [!] CPF já cadastrado.\n"); pausar(); return; }

    ler_string("  Nome: ", novo.nome, MAX_NOME);
    if (strlen(novo.nome) < 3) { printf("  [!] Nome muito curto.\n"); pausar(); return; }

    novo.idade = ler_inteiro("  Idade: ", 5, 120);

    int total;
    Discente *lista = carregar_discentes(&total);
    Discente *nova_lista = (Discente *)realloc(lista, (total + 1) * sizeof(Discente));
    if (!nova_lista) { printf("  [!] Erro de memória.\n"); free(lista); pausar(); return; }
    lista = nova_lista;
    lista[total] = novo;
    salvar_discentes(lista, total + 1);
    free(lista);
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
            case 1: inserir_discente(); break;
                //case 2: editar_discente(); break; //TODO
                //case 3: excluir_discente(); break;
                //case 4: pesquisar_discente(); break;
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
            case 1: menu_discentes(); break;
            case 2: printf("Vc esta na op2"); break;
            case 3: printf("Vc esta na op3"); break;
            case 4: printf("Vc esta na op4"); break;
            case 0:
                cabecalho("ATÉ LOGO!");
                printf("  Sistema encerrado.\n\n");
                break;
        }
    } while (op != 0);
}

int main() {
    system("chcp 65001 > nul");
    menu_principal();

    return 0;
}

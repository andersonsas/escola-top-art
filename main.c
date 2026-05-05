#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <windows.h>
#include <stdbool.h>

/* ===================== CONSTANTES ===================== */

#define MAX_CPF 20
#define MAX_NOME 100
#define MAX_CODIGO 5

#define MAX_DISCENTES 100
#define MAX_CURSOS 16
#define MAX_TURMAS 100 // MAX_LOG: M†ximo de registro de cadastro de alunos nas turmas
#define MAX_SALAS 20
#define MIN_ANO 2000
#define MAX_ANO 2026

#define ARQ_DISCENTES "discentes.txt"
#define ARQ_CURSOS "cursos.txt"
#define ARQ_TURMAS "turmas.txt"

#define MARGEM 5
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"
#define RESET   "\033[0m"
#define BOLD    "\033[1m"

/* ===================== ESTRUTURAS ===================== */

typedef struct _discente {
    char cpf[MAX_CPF];
    char nome[MAX_NOME];
    int idade;
} Discente;

typedef struct _curso {
    char codigo[MAX_CODIGO];
    char nome[MAX_NOME];
    int horas;
    int vagas;
    int participantes;
} Curso;

typedef struct _turma {
    int numero;
    char cpf[MAX_CPF];
    char codigo_curso[MAX_CODIGO];
    int ano;
    float nota;
    int hora_participacao;
} Turma;

/* ================== VARIAVEIS GLOBAIS ================= */

Discente discentes[MAX_DISCENTES];
int total_discentes = 0;

Curso cursos[MAX_CURSOS];
int total_cursos = 0;

Turma turmas[MAX_TURMAS]; // turmas_log[]
int total_turmas = 0; // 

COORD CursorPosition;

/* ================= FUNCOES AUXILIARES ================= */

void gotoxy(int x, int y) {
    CursorPosition.X = x;
    CursorPosition.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), CursorPosition);
}

/* Converte string para maisculo numa copia */
void para_maiusculo(char *dest, const char *frase) {
    int i;
    for (i = 0; frase[i] != '\0'; i++)
        dest[i] = (char)toupper((unsigned char)frase[i]);
    dest[i] = '\0';
}

/* Verifica se 'busca' aparece dentro de 'texto' */
int contem_palavra(const char texto[], const char busca[]) {
    char _t[MAX_NOME], _b[MAX_NOME];
    para_maiusculo(_t, texto);
    para_maiusculo(_b, busca);
    return strstr(_t, _b) != NULL;
}

void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void limpar_buffer_v2(char str[]) {
    int c;
    if (strchr(str, '\n') == NULL)
        while ((c = getchar()) != '\n' && c != EOF);
}

void linha_separadora(const int n, const char c[]) {
    for (int i = 0; i < n; i++) {
        printf("%s", c);
    }
}

// Conta quantos discentes tem em uma turma
int contar_discente_turma(const int numero) {
    int cont_aluno = 0;
    for (int k = 0; k < total_turmas; k++) {
        if (turmas[k].numero == numero) cont_aluno++;
    }
    return cont_aluno;
}

void pausar() {
    //printf("\n%*sPressione ENTER para continuar...", MARGEM, "");
    //limpar_buffer();
    //int x = getchar();
    printf("\n%*s", MARGEM, "");
    system("pause");
}

void cabecalho(const char titulo[]) {
    system("cls");
    gotoxy(5, 0);
    printf("ESCOLA TOP-ART-BARE  |  %s\n", titulo);
    printf("\n");
}

/* =============== VALIDACOES ========================= */

void formatarCPF(char cpf[]) {
    int i = 0, j = 0;
    while (cpf[i]) {
        if (isdigit(cpf[i])) {
            cpf[j++] = cpf[i];
        }
        i++;
    }
    cpf[j] = '\0';
}

int cpf_valido(char cpf[]) {
    int i, len = strlen(cpf);
    if (len != 11 && len != 14) return 0; // O de '14' digitos desabilitado
    //if (len != 11) return 0; // Aceitar somente com 11 digitos

    for (i = 0; i < len; i++) {
        if (len == 14 && (i == 3 || i == 7)) {  // case1
            if (cpf[i] != '.') return 0;
        } else if (len == 14 && i == 11) {      //case2
            if (cpf[i] != '-') return 0;
        } else {                                //default
            if (!isdigit(cpf[i])) return 0;
        }
    }
    if (len == 14) formatarCPF(cpf);
    return 1;  // Passou em todos os testes
}

/* ===================== LEITURA SEGURA ===================== */

// Ler uma string estabelecendo uma legenda e um limite de leitura
void ler_string(const char legenda[], char dest[], int max) {
    printf("%*s%s", MARGEM, "", legenda);
    fgets(dest, max, stdin); limpar_buffer_v2(dest);
    dest[strcspn(dest, "\n")] = '\0';
}

// Ler um n£mero inteiro estabelendo uma legenda e intervalo
int ler_inteiro(const char legenda[], int min, int max_val) {
    int valor; char buf[50];

    while (1) {
        printf("%*s%s", MARGEM, "", legenda);
        fgets(buf, sizeof(buf), stdin);
        if (sscanf(buf, "%d", &valor) == 1 && valor >= min && valor <= max_val) {
            printf("\033[J"); return valor;
        }
        printf(RED"\n%*s[!] Valor inv†lido."RESET" Digite entre %d e %d.\n",
                                                    MARGEM, "", min, max_val);
        printf("\033[3A\033[2K");
    }
}

float ler_float(const char legenda[], const float minf, const float maxf) {
    char buf[10]; float valor = 0;
    while (1) {
        printf("%*s%s", MARGEM, "", legenda);
        fgets(buf, sizeof(buf), stdin);
        if (sscanf(buf, "%f", &valor) == 1 && valor >= minf && valor <= maxf)
            return valor;
        printf("%*s[!] Valor invalido, Digite entre %.2f e %.2f.\n",
            MARGEM, "", minf, maxf);
    }
}

/* ===================== DISCENTES - ARQUIVO ===================== */

void carregar_discentes() {
    FILE *f = fopen(ARQ_DISCENTES, "r");
    if (!f) {
        printf("Error ao carregar discentes.\nArquivo ser† criado no pr¢ximo salvamento.");
        pausar(); return;
    }

    total_discentes = 0;
    while (total_discentes < MAX_DISCENTES &&
           fscanf(f, "%14[^|]|%99[^|]|%d\n",
               discentes[total_discentes].cpf,
               discentes[total_discentes].nome,
               &discentes[total_discentes].idade) == 3) total_discentes++;
    fclose(f);
}

void salvar_discentes() {
    FILE *f = fopen(ARQ_DISCENTES, "w");
    if (!f) {
        printf("%*s[!] Erro ao salvar discentes.\n", MARGEM, ""); return;
    }

    int i;
    for (i = 0; i < total_discentes; i++)
        fprintf(f, "%s|%s|%d\n",
                discentes[i].cpf,
                discentes[i].nome,
                discentes[i].idade);
    fclose(f);
}

/* ======================== CURSO - ARQUIVO ======================== */

void carregar_cursos() {
    FILE *f = fopen(ARQ_CURSOS, "r");
    if (!f) {
        printf("Erro ao carregar cursos.\nArquivo ser† criado no pr¢ximo salvamento.");
        pausar(); return;
    }

    total_cursos = 0;
    char linha[100];
    while (total_cursos < MAX_CURSOS && fgets(linha, sizeof(linha), f) != NULL) {
        char *token = strtok(linha, "|");
        if (token) strncpy(cursos[total_cursos].codigo, token, MAX_CODIGO);

        token = strtok(NULL, "|");
        if (token) strcpy(cursos[total_cursos].nome, token);

        token = strtok(NULL, "|");
        if (token) cursos[total_cursos].horas = atoi(token);

        token = strtok(NULL, "|");
        if (token) cursos[total_cursos].vagas = atoi(token);

        token = strtok(NULL, "|");
        if (token) cursos[total_cursos].participantes = atoi(token);

        total_cursos++;
    }

    fclose(f);
}

void salvar_cursos() {
    FILE *f = fopen(ARQ_CURSOS, "w"); if (!f) { pausar(); return; }

    int i;
    for (i = 0; i < total_cursos; i++) {
        fprintf(f, "%s|%s|%d|%d|%d\n", cursos[i].codigo,
                                       cursos[i].nome,
                                       cursos[i].horas,
                                       cursos[i].vagas,
                                       cursos[i].participantes);
    }
    fclose(f);
}

/* ======================== TURMA - ARQUIVO ========================*/

void carregar_turma() {
    FILE *f = fopen(ARQ_TURMAS, "r");
    if (!f) {
        printf("Erro ao carregar turma.\nArquivo ser† criado no pr¢ximo salvamento.");
        pausar(); return;
    }

    char linha[128];
    total_turmas = 0;
    while (total_turmas < MAX_TURMAS && fgets(linha, sizeof(linha), f) != NULL) {
        char *token = strtok(linha, "|");
        if (token) turmas[total_turmas].numero = atoi(token);

        token = strtok(NULL, "|");
        if (token) strcpy(turmas[total_turmas].cpf, token);

        token = strtok(NULL, "|");
        if (token) strcpy(turmas[total_turmas].codigo_curso, token);

        token = strtok(NULL, "|");
        if (token) turmas[total_turmas].ano = atoi(token);

        token = strtok(NULL, "|");
        if (token) turmas[total_turmas].nota = atof(token);

        token = strtok(NULL, "|\n");
        if (token) turmas[total_turmas].hora_participacao = atoi(token);

        total_turmas++;
    }
    fclose(f);
}

void salvar_turma() {
    FILE *f = fopen(ARQ_TURMAS, "w");
    if (!f) { printf("  [!] Salvar turma falhou"); pausar(); return; }

    for (int i = 0; i < total_turmas; i++)
        fprintf(f, "%d|%s|%s|%d|%.2f|%d\n", turmas[i].numero,
                                            turmas[i].cpf,
                                            turmas[i].codigo_curso,
                                            turmas[i].ano,
                                            turmas[i].nota,
                                            turmas[i].hora_participacao);
    fclose(f);
}

/* ===================== FUNÄÂES BUSCA AUXILIAR ===================== */

/* @brief Verifica a existància de um discente no sistema atravÇs do CPF.
 Opcionalmente retorna sua posiá∆o no array de discentes global  */
bool discente_existe(const char cpf[], int *indice) {
    for (int i = 0; i < total_discentes; i++)
        if (strcmp(discentes[i].cpf, cpf) == 0) {
            if (indice != NULL) *indice = i;
            return true;
        }
    return false;
}

/* @brief Verifica se h† curso pelo c¢digo e pode, opcionalmente, retornar seu °ndice.
    Use NULL ao °ndice caso n∆o queira seu °ndice.*/
bool curso_existe(const char codigo[], int *indice) {
    for (int i = 0; i < total_cursos; i++)
        if (strcmp(cursos[i].codigo, codigo) == 0) {
            if (indice != NULL) *indice = i;
            return true;
        }
    return false;
}

// Busca o primeiro °ndice da turma pelo n£mero
int buscar_turma_numero(const int numero) {
    int i;
    for (i = 0; i < total_turmas; i++)
        if (turmas[i].numero == numero) return i;
    return -1;
}

// Retorna o °ndice da turma onde o discente Ç matriculado.
int buscar_turma_numero_cpf(const int numero, const char cpf[]) {
    int i; // o discente est† na turma? -> retorna o indice do arrays de turma
    for (i = 0; i < total_turmas; i++)
        if (turmas[i].numero == numero && strcmp(turmas[i].cpf, cpf) == 0) return i;
    return -1;
}

/* ======================== *LISTAGEM* =================================  */

void listar() {
    int i;
    printf("%-15s%-30s%-10s\n", "CPF", "NOME", "IDADE");
    for (i = 0; i < total_discentes; i++) {
        printf("%-15s%-30s%-10d\n",
            discentes[i].cpf, discentes[i].nome, discentes[i].idade);
    }
    puts("----------------------------------------------------------");
}

/* ======================== MENU DISCENTE - CRUD ========================= */

void inserir_discente() {
    cabecalho("DISCENTES > INSERIR");

    Discente novo;
    ler_string("CPF: ", novo.cpf, MAX_CPF);

    if (!cpf_valido(novo.cpf)) {
        printf("\n%*s%s", MARGEM, "", "CPF Inv†lido"); pausar(); return;
    }

    if (discente_existe(novo.cpf, NULL)) {
        printf("\n%*s%s\n", MARGEM, "", "[!] Discente j† est† registrado!");
        pausar(); return;
    }

    ler_string("Nome: ", novo.nome, MAX_NOME);
    if (strlen(novo.nome) < 3) {
        printf("\n%*s[!] Nome muito curto.\n", MARGEM, "");
        pausar(); return;
    }

    novo.idade = ler_inteiro("Idade: ", 5, 120);

    discentes[total_discentes] = novo;
    total_discentes++;
    salvar_discentes();

    printf("\n%*s[OK] Discente cadastrado com sucesso!\n", MARGEM, "");
    pausar();
}

void editar_discente() {
    cabecalho(" DISCENTE >" BOLD YELLOW " EDITAR" RESET);

    char cpf[MAX_CPF]; int idx;
    ler_string("CPF do discente: ", cpf, MAX_CPF);

    if (!cpf_valido(cpf)) {
        printf("\n%*s%s", MARGEM, "", "CPF Inv†lido"); pausar(); return;
    }

    if (!discente_existe(cpf, &idx)) {
        cabecalho("DISCENTE > EDITAR");
        printf("\n%*s[!] Discente n∆o existe", MARGEM, ""); pausar(); return;
    }

    cabecalho("DISCENTE > EDITAR >" BOLD YELLOW " SELECIONAR" RESET);

    gotoxy(MARGEM + 00, 2); printf(BOLD BLUE "[1] CPF");
    gotoxy(MARGEM + 20, 2); printf("[2] NOME");
    gotoxy(MARGEM + 60, 2); printf("[3] IDADE" RESET);
    gotoxy(MARGEM + 04, 3); printf(discentes[idx].cpf);
    gotoxy(MARGEM + 24, 3); printf(discentes[idx].nome);
    gotoxy(MARGEM + 64, 3); printf("%d", discentes[idx].idade);

    gotoxy(0, 6);
    int opt = ler_inteiro("Escolha o que editar: ", 0, 3);

    char novo_cpf[MAX_CPF], novo_nome[MAX_NOME]; int nova_idade;

    switch (opt) {
        case 1:
            ler_string("Novo CPF: ", novo_cpf, MAX_CPF);
            if (!cpf_valido(novo_cpf)) {
                printf("\n%*s[!] CPF inv†lido.", MARGEM, "");
                pausar(); return;
            }

            if (discente_existe(novo_cpf, NULL)) {
                printf("\n%*s[!] CPF j† existe. ", MARGEM, "");
                pausar(); return;
            }
            strcpy(discentes[idx].cpf, novo_cpf);

            /* Se modificar o valor de um aluno, ser† necess†rio
            atualizar a turma em que ele est† registrado. */
            for (int i = 0; i < total_turmas; i++) {
                if (strcmp(turmas[i].cpf, cpf) == 0) {
                    strcpy(turmas[i].cpf, novo_cpf);
                }
            }
            break;
        case 2:
            ler_string("Novo Nome: ", novo_nome, MAX_NOME);
            if (strlen(novo_nome) > 3)
                strcpy(discentes[idx].nome, novo_nome);
            break;
        case 3:
            nova_idade = ler_inteiro("Nova idade: ", 5, 120);
            discentes[idx].idade = nova_idade;
            break;
    }

    if (opt != 0) salvar_discentes();
    if (opt == 1) salvar_turma();
    puts(""); pausar();
}

void excluir_discente() {
    cabecalho("DISCENTE > EXCLUIR");

    char cpf[MAX_CPF]; // entrada do usu†rio
    ler_string("CPF do discente: ", cpf, MAX_CPF);

    if (!cpf_valido(cpf)) {
        printf("\n%*s%s", MARGEM, "", "CPF Inv†lido"); pausar(); return;
    }

    int idx;
    if (!discente_existe(cpf, &idx)) { // Caso de discente n∆o registrado
        printf("%*s[!] Discente n∆o est† registrado.\n", MARGEM, "");
        pausar(); return;
    }

    char sn[3];
    printf("\n%*sExcluir \"%s\"?\n", MARGEM, "", discentes[idx].nome);
    ler_string("(S/N) : ", sn, 3);
    if (sn[0] != 's' && sn[0] != 'S') {
        printf("\n%*sOperaá∆o cancelada.\n", MARGEM, ""); pausar(); return;
    }

    // Exclui todo registro do discentes em turmas
    int i, j, ic, x = 0, cont = 0;
    for (i = 1; i <= MAX_SALAS; i++) {
        x = buscar_turma_numero_cpf(i, cpf);
        if (x == -1) continue;

        curso_existe(turmas[x].codigo_curso, &ic);
        for (j = x; j < total_turmas; j++) {
            turmas[j] = turmas[j + 1];
        }
        cont++; cursos[ic].participantes--;
    }

    // Rearranjando
    for (i = idx; i < total_discentes; i++) {
        discentes[i] = discentes[i + 1];
    }

    total_turmas -= cont;
    total_discentes--;

    salvar_discentes();
    salvar_turma(); salvar_cursos();
    printf("\n%*s[!] Discente Exclu°do.", MARGEM, ""); pausar(); return;
}

void pesquisar_discente() {
    cabecalho("DISCENTES >" BOLD YELLOW " PESQUISAR" RESET);
    char cpf[MAX_CPF] = {}; int idx; Discente d;

    while (1) {
        ler_string("CPF DO DISCENTE: ", cpf, MAX_CPF); printf("\033[J");

        if (strlen(cpf) == 1 && cpf[0] == '0')  return;
        if (!cpf_valido(cpf)) {
            printf(RED"\n%*s%s"RESET, MARGEM, "", "[X] CPF INVµLIDO.");
            printf(YELLOW"\n%*s%s\n"RESET, MARGEM, "", "[!] DIGITE NOVAMENTE OU ZERO PARA VOLTAR.");
            gotoxy(0, 2); printf("\033[K"); continue;
        }

        if ((!discente_existe(cpf, &idx))) {
            printf(RED"\n%*s[!] DISCENTE N«O REGISTRADO NO SISTEMA.\n"RESET, MARGEM, "");
            printf(YELLOW "%*s[!] VERIFIQUE O NÈMERO OU CADASTRE-O PRIMEIRO.\n" RESET, MARGEM, "");
            gotoxy(0, 2); printf("\033[K"); continue;
        }

        printf(YELLOW"\n%*s%s\n"RESET, MARGEM, "", "[!] PESQUISE OUTRO OU DIGITE 0 PARA SAIR");

        d = discentes[idx];
        gotoxy(MARGEM, 7);
        printf(CYAN BOLD "DADOS DO DISCENTE" RESET);

        gotoxy(MARGEM, 9);
        printf(BOLD "%-16s %-40s %-6s" RESET, "CPF", "NOME COMPLETO", "IDADE");
        gotoxy(MARGEM, 10);
        linha_separadora(65, "ƒ");
        gotoxy(MARGEM, 11);
        printf("%-16s %-40s %-6d\n", d.cpf, d.nome, d.idade);

        gotoxy(0, 2); printf("\033[K");
    }
}

void menu_discentes() {
    int op;
    do {
        cabecalho("DISCENTES");
        gotoxy(MARGEM + 0, 2); printf("[1] Inserir");
        gotoxy(MARGEM + 20, 2); printf("[2] Editar");
        gotoxy(MARGEM + 0, 3); printf("[3] Excluir");
        gotoxy(MARGEM + 20, 3); printf("[4] Pesquisar");
        gotoxy(MARGEM + 40, 4); printf("[0] Voltar");
        gotoxy(MARGEM + 0, 5);
        gotoxy(MARGEM + 10, 10); printf("Controle cadastral de alunos e suas informaá‰es");
        gotoxy(MARGEM + 0, 6);

        op = ler_inteiro("OPÄ«O: ", 0, 4);
        switch (op) {
            case 1: inserir_discente(); break;
            case 2: editar_discente(); break;
            case 3: excluir_discente(); break;
            case 4: pesquisar_discente(); break;
        }
    } while (op != 0);
}

/* ================ MENU CURSO - CRUD ================== */

void inserir_curso() {
    cabecalho("CURSO > INSERIR");

    if (total_cursos == MAX_CURSOS) { // Verificando o limite de inserá∆o.
        printf("%*sQuantidade de Curso est† no limite da escola\n", MARGEM, "");
        pausar(); return;
    }

    Curso novo;

    ler_string("C¢digo: ", novo.codigo, MAX_CODIGO);
    if (curso_existe(novo.codigo, NULL)) {
        printf("\n%*s%s\n", MARGEM, "", "[!] Curso j† existe.");
        pausar(); return;
    }

    ler_string("Nome: ", novo.nome, MAX_NOME);
    novo.horas = ler_inteiro("Horas: ", 1, 1000);
    novo.vagas = ler_inteiro("Vagas: ", 1, 100);
    novo.participantes = 0;

    cursos[total_cursos] = novo;
    total_cursos++;

    salvar_cursos();
    printf("\n%*s[OK] Curso cadastrado!\n", MARGEM, "");

    pausar();
}

void editar_curso() {
    cabecalho("CURSO > " BOLD YELLOW " EDITAR" RESET);
    int ic; Curso curso;

    while (true) {
        ler_string(YELLOW "C¢digo do Curso: " RESET, curso.codigo, MAX_CODIGO);
        if (!curso_existe(curso.codigo, &ic)) {
            printf(RED "\n%*s[!] Curso n∆o encontrado." RESET " [0 para voltar]", MARGEM, "");
            if (curso.codigo[0] == '0') return;
            gotoxy(0, 2); printf("\033[2K"); continue;
        }
        break;
    }

    curso = cursos[ic]; // <- copiando da variavel global para local
    cabecalho("CURSO > EDITAR >" BOLD YELLOW " SELECIONAR" RESET);
    printf(BOLD YELLOW "%*s%s\n\n" RESET, MARGEM, "", curso.nome);

    // Desenha a tabela
    printf("%*s%-10s%-30s%-10s%-10s\n", MARGEM, "", "CODIGO", "NOME", "HORAS", "VAGAS");
    printf("%*s%-10s%-30s%-10d%-10d", MARGEM, "", curso.codigo, curso.nome, curso.horas, curso.vagas);

    // Editando valores
    printf("\n\n%*s( Deixe em branco para manter )\n", MARGEM, "");

    char novo[MAX_NOME]; int valor; char str[MAX_NOME];

    printf("\n%*sNovo c¢digo: ", MARGEM, "");
    fgets(novo, MAX_CODIGO, stdin);
    if (sscanf(novo, "%s", str) == 1) { // se v†lido
        if (curso_existe(str, NULL)) { // se existe, ent∆o
            printf("%*s[!] C¢digo Existente", MARGEM, ""); pausar(); return;
        }

        strcpy(curso.codigo, str);
        for (int i = 0; i < total_turmas; i++) {
            if (strcmp(cursos[ic].codigo, turmas[i].codigo_curso) == 0) {
                strcpy(turmas[i].codigo_curso, curso.codigo);
            }
        }
        salvar_turma();
    }

    printf("%*sNovo nome: ", MARGEM, "");
    fgets(novo, sizeof(novo), stdin);
    if (sscanf(novo, "%32[^\n]", str) == 1) strcpy(curso.nome, str);

    printf("%*sNova hora: ", MARGEM, "");
    fgets(novo, sizeof(novo), stdin);
    if (sscanf(novo, "%d", &valor) == 1) curso.horas = valor;

    printf("%*sNova Vaga: ", MARGEM, "");
    fgets(novo, sizeof(novo), stdin);
    if (sscanf(novo, "%d", &valor) == 1) curso.vagas = valor;

    cursos[ic] = curso;

    salvar_cursos(); // Salva para o arquivo

    puts("\n"); pausar();
}

void excluir_curso() {
    cabecalho("CURSO > EXCLUIR");

    char codigo_procurado[MAX_CODIGO];
    ler_string("C¢digo: ", codigo_procurado, MAX_CODIGO);

    int cdx;
    if (!curso_existe(codigo_procurado, &cdx)) {
        printf("\n%*s[!] Curso n∆o existe\n", MARGEM, ""); pausar(); return;
    }

    if (cursos[cdx].participantes > 0) {
        printf("%*sO curso tem discente. N∆o pode excluir.", MARGEM, "");
        pausar(); return;
    }
    /*
    for (i = 0; i < total_turmas; i++) {
        if (strcmp(turmas[i].codigo_curso, codigo_procurado) == 0) {
            printf("%*sO curso tem discente. N∆o pode excluir.", MARGEM, "");
            pausar(); return;
        }
    }
    */

    int i;
    // executando a exclus∆o (reescrevendo no °ndice)
    for (i = cdx; i < total_cursos; i++) { cursos[i] = cursos[i + 1]; }

    total_cursos--;
    salvar_cursos();
    printf("%*s%s", MARGEM, "", "[OK] Curso removido.\n");
    pausar();
}

void pesquisar_curso() {
    cabecalho("CURSO >"BOLD YELLOW" PESQUISAR"RESET);
    int ic; char codigo[MAX_CURSOS];

    while (1) {
        ler_string("C¢digo do curso (0 para sair): ", codigo, MAX_CURSOS);
        printf("\033[J");

        if (strcmp(codigo, "0") == 0) return;

        if (!curso_existe(codigo, &ic)) {
            printf(RED "\n%*s[!] Curso %s n∆o encontrado.\n" RESET, MARGEM, "", codigo);
            printf(YELLOW "%*s[!] TENTE NOVAMENTE OU DIGITE 0 PARA SAIR\n" RESET, MARGEM, "");
            gotoxy(0, 2); printf("\033[K"); continue;
        }

        Curso c = cursos[ic];
        printf(YELLOW "\n%*s[!] PESQUISE OUTRO OU DIGITE 0 PARA SAIR\n" RESET, MARGEM, "");

        gotoxy(MARGEM, 7);
        printf(CYAN BOLD "DETALHES DO CURSO: %s" RESET, c.nome);

        gotoxy(MARGEM, 9);
        printf(BOLD "%-14s %-26s %-10s %-10s %-10s" RESET,
                    "CODIGO", "NOME", "HORAS", "VAGAS", "PARTIC.");

        gotoxy(MARGEM, 10);
        printf("%-14s %-26s %-10d %-10d %-10d\n",
                    c.codigo, c.nome, c.horas, c.vagas, c.participantes);

        // Listagem de Turmas/Salas vinculadas
        int salas[MAX_SALAS] = { 0 }; // Inicializa array de controle
        int encontrou_sala = 0;

        gotoxy(MARGEM, 12);
        printf(BOLD "Turmas / Salas Ativas: " RESET);

        for (int i = 0; i < total_turmas; i++) {
            if (strcmp(turmas[i].codigo_curso, c.codigo) == 0) {
                // Se for a primeira vez que essa sala aparece para este curso
                if (salas[turmas[i].numero] == 0) {
                    printf("[%d] ", turmas[i].numero);
                    salas[turmas[i].numero] = 1;
                    encontrou_sala = 1;
                }
            }
        }

        if (!encontrou_sala) printf(RED "Nenhuma turma vinculada." RESET);

        // Limpa a linha de entrada no topo para a pr¢xima pesquisa
        printf("\n");
        gotoxy(0, 2); printf("\033[K");
    }
}

void menu_cursos() {
    int op;
    do {
        cabecalho("CURSO");
        gotoxy(MARGEM + 0, 2);  printf("[1] Inserir");
        gotoxy(MARGEM + 20, 2); printf("[2] Editar");
        gotoxy(MARGEM + 0, 3);  printf("[3] Excluir");
        gotoxy(MARGEM + 20, 3); printf("[4] Pesquisar");
        gotoxy(MARGEM + 40, 4); printf("[0] Voltar");
        gotoxy(MARGEM + 0, 5);
        gotoxy(MARGEM + 10, 10); printf("Controle acesso e disponibilidade de cursos");
        gotoxy(MARGEM + 0, 6);

        op = ler_inteiro("OPÄ«O: ", 0, 4);
        switch (op) {
            case 1:  inserir_curso();  break;
            case 2:   editar_curso();  break;
            case 3:  excluir_curso();  break;
            case 4: pesquisar_curso(); break;
            default:
                break;
        }

    } while (op != 0);

}

/* ================ TURMA - CRUD =================== */

void inserir_turma() {
    cabecalho("INSERIR > TURMA");
    int it, ic; Turma novo; Curso curso = {};

    novo.numero = ler_inteiro("N£mero da Turma: ", 1, MAX_SALAS);

    if ((it = buscar_turma_numero(novo.numero)) != -1) {
        curso_existe(turmas[it].codigo_curso, &ic);
        curso = cursos[ic];
        strcpy(novo.codigo_curso, curso.codigo);
        para_maiusculo(curso.codigo, curso.codigo);
        para_maiusculo(curso.nome, curso.nome);
        printf("\n%*sTurma de %s - %s\n", MARGEM, "", curso.codigo, curso.nome);

        if (contar_discente_turma(novo.numero) >= curso.vagas) {
            printf("\n%*s%s\n", MARGEM, "", "[!] N∆o tem vaga"); pausar(); return;
        }
    }

    // se n∆o h†, ent∆o insere um c¢digo de curso na nova turma/sala
    if (strcmp(curso.codigo, "\0") == 0) {
        ler_string("C¢digo do curso: ", novo.codigo_curso, MAX_CODIGO);

        if (!curso_existe(novo.codigo_curso, &ic)) { // O curso existe?
            printf("\n%*s[!] Curso n∆o registrado.\n", MARGEM, ""); pausar(); return;
        }
    }

    ler_string("CPF: ", novo.cpf, MAX_CPF);

    if (!cpf_valido(novo.cpf)) {
        printf("\n%*s%s", MARGEM, "", "CPF Inv†lido"); pausar(); return;
    }

    if (!discente_existe(novo.cpf, NULL)) {
        printf("\n%*s[!] Discente n∆o est† registrado na escola.\n", MARGEM, "");
        pausar(); return;
    }

    if (buscar_turma_numero_cpf(novo.numero, novo.cpf) != -1) {
        printf("\n%*s[!] Discente j† est† matriculado na turma.\n", MARGEM, "");
        pausar(); return;
    }

    novo.ano = ler_inteiro("Ano: ", MIN_ANO, MAX_ANO);
    novo.nota = ler_float("Nota: ", 0, 10);
    novo.hora_participacao = ler_inteiro("Hora de participaá∆o: ", 0, 100);

    turmas[total_turmas] = novo; total_turmas++;

    //curso_existe(novo.codigo_curso, &ic); // 
    ++cursos[ic].participantes;

    salvar_turma(); salvar_cursos();
    printf("\n%*s[OK] Turma criada e/ou discente adicionado na turma!\n", MARGEM, "");
    pausar();
}

void editar_turma() {
    cabecalho("TURMA > EDITAR");
    int n_turma = ler_inteiro("N£mero da turma: ", 1, MAX_TURMAS);
    if (buscar_turma_numero(n_turma) == -1) { // verifica se o numero da turma existe.
        printf("\n%*s[!] Turma n∆o encontrada!", MARGEM, ""); pausar(); return;
    }

    int i; // visual
    printf("\n  %-10s %-15s %-10s %-10s %-10s %-10s",
            "TURMA", "CPF", "CURSO", "ANO", "NOTA", "HORAS");
    puts("\n  ----------------------------------------------------------------------");
    // lista os matriculados
    for (i = 0; i < total_turmas; i++) {
        if (turmas[i].numero == n_turma) { //<--- s¢ lista o numero escolhido
            printf("  %-10d %-15s %-10s %-10d %-10.2f %-10d\n",
                turmas[i].numero, turmas[i].cpf, turmas[i].codigo_curso,
                turmas[i].ano, turmas[i].nota, turmas[i].hora_participacao);
        }
    }

    char cpf_discente[MAX_CPF]; // pede o cpf do discente e verifica
    puts(""); ler_string("CPF do Discente: ", cpf_discente, MAX_CPF);

    if (!cpf_valido(cpf_discente)) {
        printf("\n%*s%s", MARGEM, "", "CPF Inv†lido"); pausar(); return;
    }

    int it = buscar_turma_numero_cpf(n_turma, cpf_discente);
    if (it == -1) {
        printf("%*s[!] Aluno n∆o est† matriculado na turma.", MARGEM, "");
        pausar(); return;
    }
    cabecalho("TURMA > EDITAR > PERFIL");

    // mostra as legendas das opcao para escolher
    int idx;
    discente_existe(cpf_discente, &idx);
    printf("\n\t%s\n", discentes[idx].nome);
    printf("\n  %-10s%-10s%-10s\n", "NOTA", "HORA", "ANO");
    printf("  %-10.2f%-10d%-10d\n", turmas[it].nota,
        turmas[it].hora_participacao, turmas[it].ano);

    printf("\n%*s(Deixe em branco para manter)\n", MARGEM, "");

    char buf[5]; float valor_f; int valor_i;
    printf("\n   Nova nota: "); // solicitaá∆o
    fgets(buf, sizeof(buf), stdin);
    if (sscanf(buf, "%f", &valor_f) == 1 && valor_f >= 0 && valor_f <= 10)
        turmas[it].nota = valor_f; //limpar_buffer();

    printf("   Nova hora: "); // solicitaá∆o
    fgets(buf, sizeof(buf), stdin);
    if (sscanf(buf, "%d", &valor_i) == 1 && valor_i > 0)
        turmas[it].hora_participacao = valor_i;

    printf("   Novo ano [2000 - 2026]: "); // solicitaá∆o
    fgets(buf, sizeof(buf), stdin); limpar_buffer_v2(buf);
    if (sscanf(buf, "%d", &valor_i) == 1 && valor_i >= MIN_ANO && valor_i <= MAX_ANO) {
        turmas[it].ano = valor_i;
    }

    salvar_turma();
    printf("\n%*s[OK]  Turma atualizada!\n", MARGEM, ""); pausar();
}

void excluir_turma() {
    cabecalho("TURMA > EXCLUIR");

    int numero = ler_inteiro("N£mero da Turma: ", 0, total_turmas);

    int i, itx, ic;
    if ((itx = buscar_turma_numero(numero)) == -1) { // verifica se o numero da turma existe.
        printf("\n%*s[!] Turma n∆o encontrada!", MARGEM, ""); pausar(); return;
    }

    curso_existe(turmas[itx].codigo_curso, &ic);
    Curso curso = cursos[ic];

    char cpf_discente[MAX_CPF]; // pede o cpf do discente e verifica
    ler_string("CPF do Discente: ", cpf_discente, MAX_CPF);

    if (!cpf_valido(cpf_discente)) {
        printf("\n%*s%s", MARGEM, "", "CPF Inv†lido"); pausar();
        goto opcao_sala;
    }

    char sn[5];
    int it = buscar_turma_numero_cpf(numero, cpf_discente);
    if (it == -1) {
        printf("\n%*s[!] Aluno n∆o est† matriculado na turma.\n", MARGEM, "");
        pausar();
    } else {
        int idx;
        discente_existe(cpf_discente, &idx);
        printf("\n%*sRemover %s da turma?\n", MARGEM, "", discentes[idx].nome);

        ler_string("(S/N): ", sn, 2);
        if (sn[0] != 's' && sn[0] != 'S') {
            printf("\n%*s[!] Exclus∆o Cancelada.", MARGEM, ""); pausar();
        } else {
            --cursos[ic].participantes;
            for (i = it; i < total_turmas; i++) {
                turmas[i] = turmas[i + 1];
            }
            --total_turmas;
            salvar_turma();
            salvar_cursos();
        }
    }

opcao_sala:
    gotoxy(MARGEM, 3); printf("\33[J");
    int qtd_aluno = 0; // Obs: A contagem de alunos de uma turma (!= N¶) participantes
    for (i = 0; i < total_turmas; i++) {
        if (turmas[i].numero == numero) qtd_aluno++;
    }

    if (qtd_aluno == 0) return;
    // Exclus∆o adicional: turma inteira
    printf("\n%*sTurma tem %d discente(s) matriculado(s)\n", MARGEM, "", qtd_aluno);
    printf("%*sExcluir turma %d de %s?\n", MARGEM, "", numero, curso.nome);

    ler_string("(S/N): ", sn, 2);
    if (sn[0] != 's' && sn[0] != 'S') {
        printf("\n%*s[!] Exclus∆o Cancelada.", MARGEM, "");
    } else {
        // Exclui todos discente da turma a ser removida.
        while ((itx = buscar_turma_numero(numero)) != -1) {
            --cursos[ic].participantes;
            for (i = itx; i < total_turmas; i++)
                turmas[i] = turmas[i + 1];
            total_turmas--;
        }
        salvar_turma();
        salvar_cursos();
    }
    pausar();
}

void pesquisar_turma() {
    cabecalho("TURMA > PESQUISAR");
    int i, ic, it, nt; char temp[MAX_NOME];

    while (1) {
        nt = ler_inteiro("N£mero da turma: ", 0, MAX_SALAS);
        if (nt == 0) return;
        it = buscar_turma_numero(nt);

        if (it == -1) {
            printf(RED"\n%*s%s %d\n"RESET, MARGEM, "", "[X] N«O Hµ DISCENTES NA TURMA", nt);
            printf(YELLOW"\n%*s%s\n"RESET, MARGEM, "", "[!] DIGITE NOVAMENTE OU ZERO PARA VOLTAR");
            gotoxy(0, 2); printf("\033[K"); continue;
        }

        printf(YELLOW"\n%*s%s\n"RESET, MARGEM, "", "[!] DIGITE NOVAMENTE OU ZERO PARA VOLTAR");

        curso_existe(turmas[it].codigo_curso, &ic);
        para_maiusculo(temp, cursos[ic].nome);
        gotoxy(MARGEM, 7); printf("TURMA DE %s", temp);

        gotoxy(MARGEM, 9); // Exibe a tabela na parte inferior
        printf("%-10s %-15s %-10s %-10s %-10s %-10s",
                "TURMA", "CPF", "CURSO", "ANO", "NOTA", "HORAS");
        gotoxy(MARGEM, 10); linha_separadora(70, "ƒ"); puts(""); // Alt+196

        for (i = it; i < total_turmas; i++) {
            if (turmas[i].numero == nt) {
                printf("%*s%-8d %-15s %-10s %-10d %-10.2f %-10d\n", MARGEM + 2, "",
                    turmas[i].numero, turmas[i].cpf, turmas[i].codigo_curso,
                    turmas[i].ano, turmas[i].nota, turmas[i].hora_participacao);
            }
        }
        gotoxy(0, 2); printf("\033[K");
    }
}

void menu_turmas() {
    int op;
    do {
        cabecalho("TURMAS");
        gotoxy(MARGEM + 0, 2); printf("[1] Inserir");
        gotoxy(MARGEM + 20, 2); printf("[2] Editar");
        gotoxy(MARGEM + 0, 3); printf("[3] Excluir");
        gotoxy(MARGEM + 20, 3); printf("[4] Pesquisar");
        gotoxy(MARGEM + 40, 4); printf("[0] Voltar");
        gotoxy(MARGEM + 0, 5);
        gotoxy(MARGEM + 10, 10); printf("Matricule alunos em turmas e acompanhe notas");
        gotoxy(MARGEM + 0, 6);

        switch (op = ler_inteiro("Opá∆o: ", 0, 4)) {
            case 1:  inserir_turma();  break;
            case 2:  editar_turma();   break;
            case 3:  excluir_turma();  break;
            case 4: pesquisar_turma(); break;
        }


    } while (op != 0);

}

/* ====================== RELATORIO - CRUD =========================*/

void relat_A() {
    cabecalho("RELATORIO > A");

    printf("%*s%-20s%-40s%-10s", MARGEM, "", "CPF", "NOME", "IDADE");
    printf("\n%*s-----------------------------------------------------------------\n", MARGEM, "");

    for (int i = 0; i < total_discentes; i++) {
        gotoxy(MARGEM, 4 + i); printf(discentes[i].cpf);
        gotoxy(MARGEM + 20, 4 + i); printf(discentes[i].nome);
        gotoxy(MARGEM + 60, 4 + i); printf("%d", discentes[i].idade);
    }

    puts(""); pausar();
}

void relat_B() {
    cabecalho("RELATORIO > B");
    printf("%*s%-10s%-30s%-10s%-10s", MARGEM, "", "CODIGO", "NOME", "HORAS", "VAGAS");

    for (int i = 0; i < total_cursos; i++) {
        gotoxy(MARGEM, 4 + i); printf(cursos[i].codigo);
        gotoxy(MARGEM + 10, 4 + i); printf(cursos[i].nome);
        gotoxy(MARGEM + 40, 4 + i); printf("%d", cursos[i].horas);
        gotoxy(MARGEM + 50, 4 + i); printf("%d", cursos[i].vagas);
    }

    puts(""); pausar();
}

void relat_C() {
    cabecalho("RELATORIO > C");

    char busca[MAX_NOME];
    ler_string("Palavra-chave: ", busca, MAX_NOME);

    printf("\n%*s%-20s%-40s%-10s", MARGEM, "", "CPF", "NOME", "IDADE");

    int i, maisUM = 0; // encontrado
    for (i = 0; i < total_discentes; i++) {
        if (contem_palavra(discentes[i].nome, busca)) {
            gotoxy(MARGEM, 6 + maisUM); printf(discentes[i].cpf);
            gotoxy(MARGEM + 20, 6 + maisUM); printf(discentes[i].nome);
            gotoxy(MARGEM + 60, 6 + maisUM); printf("%d", discentes[i].idade);
            maisUM++;
        }
    }

    if (maisUM == 0) printf("\n\n%*sNenhum discente encontrado com essa palavra", MARGEM, "");

    puts(""); pausar();
}

void relat_D() {
    cabecalho("RELATORIO > D");

    if (total_turmas == 0) {
        printf("[!] Nenhuma turma cadastrada."); pausar(); return;
    }

    printf("%*s%-10s%-15s%-40s%-10s", MARGEM, "", "TURMA", "CPF", "NOME", "NOTA");
    printf("\n%*s", MARGEM, ""); linha_separadora(75, "ƒ"); // Alt+196

    int i, id, adicional = 0; // id: °ndice de discente
    for (i = 0; i < total_turmas; i++) {
        discente_existe(turmas[i].cpf, &id);

        gotoxy(MARGEM + 02, 4 + adicional); printf("%d", turmas[i].numero);
        gotoxy(MARGEM + 10, 4 + adicional); printf(turmas[i].cpf);
        gotoxy(MARGEM + 25, 4 + adicional); printf(discentes[id].nome);
        gotoxy(MARGEM + 65, 4 + adicional); printf("%.2f", turmas[i].nota);

        adicional++;

        /* printf("  %-10d %-15s %-40s %10.2f \n", turmas[i].numero, turmas[i].cpf,
                                                discentes[id].nome, turmas[i].nota); */
    }
    puts(""); pausar();
}

void relat_E() {
    cabecalho("RELATORIO > E");

    if (total_turmas == 0) {
        printf("[!] Nenhuma turma cadastrada."); pausar(); return;
    }

    printf("%*s%-10s%-15s%-40s%-10s%-6s%-6s", MARGEM, "",
            "TURMA", "CPF", "NOME", "NOTA", "COD", "CURSO");
    printf("\n%*s", MARGEM, ""); linha_separadora(95, "ƒ"); // Alt+196


    int i, id, ic, maisUM = 0; // id: °ndice de discente | ic: °ndice de curso
    for (i = 0; i < total_turmas; i++) {
        discente_existe(turmas[i].cpf, &id);
        curso_existe(turmas[i].codigo_curso, &ic);

        gotoxy(MARGEM + 02, 4 + maisUM); printf("%d", turmas[i].numero);
        gotoxy(MARGEM + 10, 4 + maisUM); printf(turmas[i].cpf);
        gotoxy(MARGEM + 25, 4 + maisUM); printf(discentes[id].nome);
        gotoxy(MARGEM + 65, 4 + maisUM); printf("%.2f", turmas[i].nota);
        gotoxy(MARGEM + 75, 4 + maisUM); printf(turmas[i].codigo_curso);
        gotoxy(MARGEM + 82, 4 + maisUM); printf(cursos[ic].nome);

        maisUM++;

        /* printf("  %-7d %-15s %-40s %10.2f %6s %-s \n",
                                        turmas[i].numero, turmas[i].cpf,
                                      discentes[id].nome, turmas[i].nota,
                                  turmas[i].codigo_curso, cursos[ic].nome); */

    }
    puts(""); pausar();
}

void relat_F() {
    cabecalho("RELATORIO > F");

    int n_turma = ler_inteiro("N£mero da turma: ", 0, 99);

    int i, it;
    gotoxy(MARGEM, 8); // Exibe a tabela na parte inferior
    printf("\n  %-10s %-15s %-10s %-10s %-10s %-10s",
            "TURMA", "CPF", "CURSO", "ANO", "NOTA", "HORAS");
    puts("\n  ----------------------------------------------------------------------");

    if (buscar_turma_numero(n_turma) != -1) { // Imprime se o nß da turma desejada existir
        for (i = 0; i < total_turmas; i++) {
            if (turmas[i].numero == n_turma) {
                printf("  %-10d %-15s %-10s %-10d %-10.2f %-10d\n",
                    turmas[i].numero, turmas[i].cpf, turmas[i].codigo_curso,
                    turmas[i].ano, turmas[i].nota, turmas[i].hora_participacao);
            }
        }

        gotoxy(26, 2); // Exibe o nome do curso na parte superior
        it = buscar_turma_numero(n_turma); int ic;
        curso_existe(turmas[it].codigo_curso, &ic);
        printf("-  %s", cursos[ic].nome);
        gotoxy(MARGEM, 4);
    }
    pausar();
}

void relat_G() {
    cabecalho("RELATORIO > G");

    int n_turma = 1, it, i;
    int qtd_aluno[MAX_SALAS + 1] = {}; // qtd de aluno por sala (°ndice = sala)
    float somatorio_nota[MAX_SALAS + 1] = {};

    while (n_turma <= MAX_SALAS) {
        // verifica se turma existe
        if ((it = buscar_turma_numero(n_turma)) == -1) {
            n_turma++; continue;
        }
        // procurando correspondente no array de turmas
        for (i = it; i < total_turmas; i++) {
            if (turmas[i].numero == n_turma) {
                qtd_aluno[n_turma]++;
                somatorio_nota[n_turma] += turmas[i].nota;
            }
        }
        n_turma++;
    }

    gotoxy(MARGEM, 4); linha_separadora(50, "ƒ"); // Alt+196
    printf("\n%*s%-10s%-10s%-10s%-10s\n", MARGEM, "", "TURMA", "ALUNOS", "SOMA", "MêDIA");
    gotoxy(MARGEM, 6); linha_separadora(50, "ƒ"); // Alt+196

    for (i = 1; i <= MAX_SALAS; i++) {
        if (qtd_aluno[i] == 0) continue;
        printf("\n%*s%3d%10d%12.2f%10.2f", MARGEM, "",
            i, qtd_aluno[i], somatorio_nota[i], somatorio_nota[i] / qtd_aluno[i]);
    }

    gotoxy(4, 1);
    linha_separadora(56, "Õ"); // Alt+205
    pausar();
}

void menu_relatorio() {
    char op[5];
    do {
        cabecalho("RELAT‡RIOS");
        gotoxy(MARGEM + 0, 2); printf("[A] CPF, NOME e IDADE dos discentes");
        gotoxy(MARGEM + 0, 3); printf("[B] C‡DIGO, NOME, HORAS e VAGAS dos cursos");
        gotoxy(MARGEM + 0, 4); printf("[C] Discentes por palavras no nome");
        gotoxy(MARGEM + 0, 5); printf("[D] Turma, CPF, NOME e NOTA");
        gotoxy(MARGEM + 0, 6); printf("[E] Turma, DISCENTES, NOTA e CURSO");
        gotoxy(MARGEM + 0, 7); printf("[F] Discentes por n£mero de turma");
        gotoxy(MARGEM + 0, 8); printf("[G] Turmas com mÇdias das notas");
        gotoxy(MARGEM + 0, 9); printf("[0] Voltar");

        gotoxy(MARGEM + 0, 11); printf("Escolha uma das opá‰es.\n");

        ler_string("OPÄ«O: ", op, 5);
        if (op[0] == '0') break;
        switch (tolower(op[0])) {
            case 'a': relat_A(); break;
            case 'b': relat_B(); break;
            case 'c': relat_C(); break;
            case 'd': relat_D(); break;
            case 'e': relat_E(); break;
            case 'f': relat_F(); break;
            case 'g': relat_G(); break;
            default: printf("\n%*s[!] Inv†lido!", MARGEM, ""); pausar();
        }
    } while (1);


}

/* ================ MENU PRINCIPAL =============== */

void menu_principal() {
    int op;
    do {
        cabecalho("MENU PRINCIPAL");
        gotoxy(MARGEM + 0, 2); printf("[1] Discentes");
        gotoxy(MARGEM + 20, 2); printf("[2] Cursos");
        gotoxy(MARGEM + 0, 3); printf("[3] Turmas");
        gotoxy(MARGEM + 20, 3); printf("[4] Relat¢rio");
        gotoxy(MARGEM + 40, 4); printf("[0] Sair");
        gotoxy(MARGEM + 0, 5);
        gotoxy(MARGEM + 10, 10); printf("Escolha uma das opá‰es.");
        gotoxy(MARGEM + 0, 6);
        /* Validando numeros inteiros */
        op = ler_inteiro("OPÄ«O: ", 0, 4);
        switch (op) {
            case 1: menu_discentes(); break;
            case 2: menu_cursos(); break;
            case 3: menu_turmas(); break;
            case 4: menu_relatorio(); break;
            case 0: cabecalho("ATê LOGO!");
                printf("\n%*sSistema encerrado.\n\n", MARGEM, ""); break;
        }
    } while (op != 0);
}

/* ==================== MAIN =================== */

int main() {
    //SetConsoleOutputCP(CP_UTF8);
    //SetConsoleCP(CP_UTF8);
    //setlocale(LC_ALL, "Portuguese_Brazil.65001");

    //system("chcp 65001>nul");
    setlocale(LC_CTYPE, ".850");
    //setlocale(LC_ALL, "pt_br.UTF-8");
    carregar_discentes();
    carregar_cursos();
    carregar_turma();

    menu_principal();

    return 0;
}

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <windows.h>

/* ===================== CONSTANTES ===================== */

#define MAX_CPF 20
#define MAX_NOME 100
#define MAX_CODIGO 10

#define MAX_DISCENTES 100
#define MAX_CURSOS 16
#define MAX_TURMAS 100 // MAX_LOG: Máximo de registro de cadastro de alunos nas turmas
#define MAX_SALAS 10
#define MIN_ANO 2000
#define MAX_ANO 2026

#define ARQ_DISCENTES "discentes.txt"
#define ARQ_CURSOS "cursos.txt"
#define ARQ_TURMAS "turmas.txt"

#define MARGEM 10

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

void pausar() {
    printf("\n%*sPressione ENTER para continuar...", MARGEM, "");
    //limpar_buffer();
    int x = getchar();
}

void cabecalho(const char titulo[]) {
    system("cls");
    gotoxy(5, 0);
    printf("ESCOLA TOP-ART-BARE  |  %s\n", titulo);
    printf("\n");
}

/* =============== VALIDACOES ========================= */

int cpf_valido(const char cpf[]) {
    int i, len = strlen(cpf);
    //if (len != 11 && len != 14) return 0; // O de '14' digitos desabilitado
    if (len != 11) return 0; // Aceitar somente com 11 digitos

    for (i = 0; i < len; i++) {
        if (len == 14 && (i == 3 || i == 7)) {  // case1
            if (cpf[i] != '.') return 0;
        } else if (len == 14 && i == 11) {      //case2
            if (cpf[i] != '-') return 0;
        } else {                                //default
            if (!isdigit(cpf[i])) return 0;
        }
    }
    return 1;  // Passou em todos os testes
}

/* ===================== LEITURA SEGURA ===================== */

// Ler uma string estabelecendo uma legenda e um limite de leitura
void ler_string(const char legenda[], char dest[], int max) {
    printf("%*s%s", MARGEM, "", legenda);
    fgets(dest, max, stdin); limpar_buffer_v2(dest);
    dest[strcspn(dest, "\n")] = '\0';
}

// Ler um número inteiro estabelendo uma legenda e intervalo
int ler_inteiro(const char legenda[], int min, int max_val) {
    int valor; char buf[50];

    while (1) {
        printf("%*s%s", MARGEM, "", legenda);
        fgets(buf, sizeof(buf), stdin);
        if (sscanf(buf, "%d", &valor) == 1 && valor >= min && valor <= max_val)
            return valor;
        printf("  [!] Valor inválido. Digite entre %d e %d.\n", min, max_val);
    }
}

float ler_float(const char legenda[], const float minf, const float maxf) {
    char buf[10]; float valor = 0;
    while (1) {
        printf("%s", legenda);
        fgets(buf, sizeof(buf), stdin);
        if (sscanf(buf, "%f", &valor) == 1 && valor > minf && valor < maxf)
            return valor;
        printf("%*s[!] Valor invalido, Digite entre %.2f e %.2f.\n",
            MARGEM, "", minf, maxf);
    }
}

/* ===================== DISCENTES - ARQUIVO ===================== */

void carregar_discentes() {
    FILE *f = fopen(ARQ_DISCENTES, "r");
    if (!f) { printf("Error ao carregar"); pausar(); return; }

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
        printf("%*s[!] Erro ao carregar curso.\n", MARGEM, ""); return;
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
    if (!f) { printf(" [!] Carregar turma falhou"); pausar(); return; }

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

/* ===================== FUNÇÕES BUSCA AUXILIAR ===================== */

// Busca o índice do discente, caso contrário -1
int buscar_discente_cpf(char cpf[]) {
    int i;
    for (i = 0; i < total_discentes; i++)
        if (strcmp(discentes[i].cpf, cpf) == 0) return i;
    return -1;
}

// Busca o índice do curso pelo código ou -1
int buscar_curso_codigo(const char codigo[]) {
    int i;
    for (i = 0; i < total_cursos; i++)
        if (strcmp(cursos[i].codigo, codigo) == 0) return i;
    return -1;
}

// Busca o primeiro índice da turma pelo número
int buscar_turma_numero(const int numero) {
    int i;
    for (i = 0; i < total_turmas; i++)
        if (turmas[i].numero == numero) return i;
    return -1;
}

// Retorna o índice da turma onde o discente é matriculado.
int buscar_turma_numero_cpf(const int numero, const char cpf[]) {
    int i; // o discente está na turma? -> retorna o indice do arrays de turma
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
    ler_string("CPF (apenas dígitos): ", novo.cpf, MAX_CPF);

    if (!cpf_valido(novo.cpf)) {
        printf("\n%*s%s", MARGEM, "", "CPF Inválido"); pausar(); return;
    }

    gotoxy(0, 7);
    if (buscar_discente_cpf(novo.cpf) >= 0) {
        printf("\n%*s%s\n", MARGEM, "", "[!] Discente já está registrado!");
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
    cabecalho(" DISCENTE > EDITAR");
    gotoxy(MARGEM, 2);

    char cpf[MAX_CPF];
    ler_string(" CPF do discente: ", cpf, MAX_CPF);
    int idx = buscar_discente_cpf(cpf);
    if (idx < 0) {
        cabecalho("DISCENTE > EDITAR");
        printf("\n%*s[!] Discente não existe", MARGEM, ""); pausar(); return;
    }

    cabecalho(" DISCENTE > EDITAR > SELECIONAR");

    gotoxy(MARGEM, 2); printf("[1] CPF");
    gotoxy(MARGEM + 20, 2); printf("[2] NOME");
    gotoxy(MARGEM + 60, 2); printf("[3] IDADE");
    gotoxy(MARGEM + 4, 3); printf(discentes[idx].cpf);
    gotoxy(MARGEM + 24, 3); printf(discentes[idx].nome);
    gotoxy(MARGEM + 64, 3); printf("%d", discentes[idx].idade);

    gotoxy(0, 6);
    int opt = ler_inteiro("Escolha o que editar: ", 0, 3);

    char novo_cpf[MAX_CPF];
    char novo_nome[MAX_NOME];
    int nova_idade;

    switch (opt) {
        case 1:
            ler_string("Novo CPF: ", novo_cpf, MAX_CPF);
            if (!cpf_valido(novo_cpf)) {
                printf("\n%*s[!] CPF inválido.", MARGEM, "");
                pausar(); return;
            }

            if (buscar_discente_cpf(novo_cpf) != -1) {
                printf("\n%*s[!] CPF já existe. ", MARGEM, "");
                pausar(); return;
            }
            strcpy(discentes[idx].cpf, novo_cpf);

            /* Se modificar o valor de um aluno, será necessário
            atualizar a turma em que ele está registrado. */
            for (int i = 0; i < total_turmas; i++) {
                if (strcmp(turmas[i].cpf, cpf) == 0) {
                    strcpy(turmas[i].cpf, novo_cpf);
                }
            }
            break;
        case 2:
            ler_string("Novo Nome: ", novo_nome, MAX_NOME);
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

    char cpf[MAX_CPF]; // entrada do usuário
    ler_string("CPF do discente: ", cpf, MAX_CPF);

    int idx = buscar_discente_cpf(cpf);
    if (idx < 0) { // Caso de discente não registrado
        printf("%*s[!] Discente não está registrado.\n", MARGEM, "");
        pausar(); return;
    }

    char sn[3];
    printf("\n%*sExcluir \"%s\"?\n", MARGEM, "", discentes[idx].nome);
    ler_string("(s/n) : ", sn, 3);
    if (sn[0] != 's' && sn[0] != 'S') {
        printf("\n%*sOperação cancelada.\n", MARGEM, ""); pausar(); return;
    }

    // O total de sala é quantidade de números distintos 
    // no número da turma do arquivo turmas.txt
    int contar = 0; // void get_totalSalas();
    for (int it = 1; it < total_turmas; it++) {
        if (buscar_turma_numero(it) != -1) contar++;
    }

    // Exclui todo registro do discentes em turmas
    int i, j, x = 0, cont = 0;
    for (i = 1; i <= MAX_SALAS; i++) {
        x = buscar_turma_numero_cpf(i, cpf);
        if (x == -1) continue;

        for (j = x; j < total_turmas; j++) {
            turmas[j] = turmas[j + 1];
        }
        cont++;
    }

    // Rearranjando
    for (i = idx; i < total_discentes; i++) {
        discentes[i] = discentes[i + 1];
    }

    total_turmas -= cont;
    total_discentes--;

    salvar_discentes();
    salvar_turma();
    printf("\n%*s[!] Discente Excluído.", MARGEM, ""); pausar(); return;
}

void pesquisar_discente() {
    cabecalho("DISCENTES > PESQUISAR");

    char cpf[MAX_CPF];
    ler_string("   CPF do discente: ", cpf, MAX_CPF);

    int idx;
    if ((idx = buscar_discente_cpf(cpf)) == -1) {
        printf("\n%*s[!] Discente não é registrado.\n", MARGEM, "");
        pausar(); return;
    }

    printf("\n\t%-14s%-20s%10s", "CPF", "NOME", "IDADE");
    printf("\n\t%-14s%-20s%10d\n", discentes[idx].cpf,
        discentes[idx].nome, discentes[idx].idade);

    pausar();
}



void menu_discentes() {
    int op;
    do {
        cabecalho("DISCENTES");
        gotoxy(MARGEM + 0, 2); printf("[1] Inserir");
        gotoxy(MARGEM + 20, 2); printf("[2] Editar");
        gotoxy(MARGEM + 0, 3); printf("[3] Excluir");
        gotoxy(MARGEM + 20, 3); printf("[4] Pesquisar");
        gotoxy(MARGEM + 40, 3); printf("[0] Voltar");
        gotoxy(MARGEM + 0, 5);
        gotoxy(MARGEM + 10, 10); printf("Em desenvolvimento");
        gotoxy(MARGEM + 0, 6);

        op = ler_inteiro("OPÇÃO: ", 0, 4);
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

    if (total_cursos == MAX_CURSOS) { // Verificando o limite de inserção.
        printf("%*sQuantidade de Curso está no limite da escola\n", MARGEM, "");
        pausar(); return;
    }

    Curso novo;
    novo.participantes = 0;

    ler_string(" Código: ", novo.codigo, MAX_CODIGO);
    ler_string(" Nome: ", novo.nome, MAX_NOME);

    novo.horas = ler_inteiro("  Horas: ", 1, 9999);
    novo.vagas = ler_inteiro(" Vagas: ", 1, 60);

    cursos[total_cursos] = novo;
    total_cursos++;

    salvar_cursos();
    printf("\n  [OK] Curso cadastrado!\n");

    pausar();
}

void editar_curso() {
    cabecalho("CURSO > EDITAR");
    int ic; Curso curso;
digite:
    ler_string("Código do Curso: ", curso.codigo, MAX_CODIGO);
    // Verificando a existência do curso
    if ((ic = buscar_curso_codigo(curso.codigo)) == -1) {
        printf("\n%*s[!] Curso não encontrado. [0 para voltar]", MARGEM, "");
        if (curso.codigo[0] == '0') return;
        gotoxy(0, 2); printf("\033[2K"); // limpa a linha no local do cursor
        goto digite;
    }

    curso = cursos[ic]; // <- copiando da variavel global para local
    cabecalho("CURSO > EDITAR > SELECIONAR");
    printf("%*s%s\n\n", MARGEM, "", curso.nome); // titulando o nome do curso

    // Desenha a tabela
    printf("%*s%-10s%-30s%-10s%-10s\n", MARGEM, "", "CODIGO", "NOME", "HORAS", "VAGAS");
    printf("%*s%-10s%-30s%-10d%-10d", MARGEM, "", curso.codigo, curso.nome, curso.horas, curso.vagas);

    // Editando valores
    printf("\n\n%*s( Deixe em branco para manter )\n", MARGEM, "");

    char novo[MAX_NOME]; int valor; char str[MAX_NOME];

    printf("\n%*sNovo código: ", MARGEM, "");
    fgets(novo, sizeof(novo), stdin);
    if (sscanf(novo, "%s", str) == 1) strcpy(curso.codigo, str);

    printf("%*sNovo nome: ", MARGEM, "");
    fgets(novo, sizeof(novo), stdin);
    if (sscanf(novo, "%32[^\n]", str) == 1) strcpy(curso.nome, str);

    printf("%*sNova hora: ", MARGEM, ""); int x;
    fgets(novo, sizeof(novo), stdin);
    if ((x = sscanf(novo, "%d", &valor) == 1)) curso.horas = valor;

    printf("%*sNova Vaga: ", MARGEM, "");
    fgets(novo, sizeof(novo), stdin);
    if (sscanf(novo, "%d", &valor) == 1) curso.vagas = valor;

    // Atualiza o codigo correspondente nas turmas
    for (int i = 0; i < total_turmas; i++) {
        if (strcmp(cursos[ic].codigo, turmas[i].codigo_curso) == 0) {
            strcpy(turmas[i].codigo_curso, curso.codigo);
        }
    }

    cursos[ic] = curso;

    salvar_cursos(); // Salva para o arquivo
    salvar_turma();

    puts("\n"); pausar();
}

void excluir_curso() {
    cabecalho("   CURSO > EXCLUIR");

    char codigo_procurado[MAX_CODIGO];
    ler_string("  Código: ", codigo_procurado, MAX_CODIGO);

    int cdx = buscar_curso_codigo(codigo_procurado);
    if (cdx < 0) { printf(" [!] Curso não existe"); pausar(); return; }

    int i; // Impedir de deletar curso caso tenha aluno matriculado no tal curso.
    for (i = 0; i < total_turmas; i++) {
        if (strcmp(turmas[i].codigo_curso, codigo_procurado) == 0) {
            printf("%*sO curso tem discente. Não pode excluir.", MARGEM, "");
            pausar(); return;
        }
    }

    // executando a exclusão (reescrevendo no índice)
    for (i = cdx; i < total_cursos; i++) { cursos[i] = cursos[i + 1]; }

    total_cursos--;
    salvar_cursos();
    printf("%*s%s", MARGEM, "", "[OK] Curso removido.\n");
    pausar();

}

void pesquisar_curso() {
    cabecalho("CURSO > PESQUISAR");

    char codigo[MAX_CURSOS];
    ler_string("  Código do curso: ", codigo, MAX_CURSOS);
    int ic;
    if ((ic = buscar_curso_codigo(codigo)) == -1) {
        printf("\n  [!] Curso não encontrado.\n"); pausar(); return;
    }

    Curso c = cursos[ic];
    printf("\n\t%-14s %-26s %-10s %-10s %-10s\n",
                "CODIGO", "NOME", "HORA", "VAGAS", "PARTICIPANTES");
    printf("\t%-14s %-26s %-10d %-10d %-10d\n",
                c.codigo, c.nome, c.horas, c.vagas, c.participantes);
    pausar();
}

void menu_cursos() {
    int op;
    do {
        cabecalho("CURSO");
        gotoxy(MARGEM + 0, 2); printf("[1] Inserir");
        gotoxy(MARGEM + 20, 2); printf("[2] Editar");
        gotoxy(MARGEM + 0, 3); printf("[3] Excluir");
        gotoxy(MARGEM + 20, 3); printf("[4] Pesquisar");
        gotoxy(MARGEM + 40, 3); printf("[0] Voltar");
        gotoxy(MARGEM + 0, 5);
        gotoxy(MARGEM + 10, 10); printf("Em desenvolvimento");
        gotoxy(MARGEM + 0, 6);

        op = ler_inteiro("OPÇÃO: ", 0, 4);
        switch (op) {
            case 1: inserir_curso(); break;
            case 2: editar_curso();  break;
            case 3: excluir_curso(); break;

            default:
                break;
        }

    } while (op != 0);

}

/* ================ TURMA - CRUD =================== */

void inserir_turma() {
    cabecalho("INSERIR > TURMA");

    Turma novo;
    novo.numero = ler_inteiro("Número da Turma: ", 1, MAX_SALAS);

    int i; // verifica se a turma já tem um curso vinculado
    for (i = 0; i < total_turmas; i++) {
        if (turmas[i].numero == novo.numero) { // se já existe ...
            strcpy(novo.codigo_curso, turmas[i].codigo_curso);
            printf("\n%*sTurma de %s - %s\n", MARGEM, "", novo.codigo_curso,
                cursos[buscar_curso_codigo(novo.codigo_curso)].nome);
            i = -1; break;
        }
    }

    // se não há, então insere um código de curso na nova turma/sala
    if (i != -1) ler_string("Código do curso: ", novo.codigo_curso, MAX_CODIGO);
    if (buscar_curso_codigo(novo.codigo_curso) == -1) {
        printf("%*s[!] Curso não registrado.", MARGEM, ""); pausar(); return;
    }

    ler_string("CPF: ", novo.cpf, MAX_CPF);
    if (buscar_discente_cpf(novo.cpf) == -1) {
        printf("%*s[!] Discente não está registrado na escola.", MARGEM, "");
        pausar(); return;
    }

    for (i = 0; i < total_turmas; i++) {
        if (turmas[i].numero == novo.numero && strcmp(turmas[i].cpf, novo.cpf) == 0) {
            printf("%*s[!] Discente já está matriculado na turma.", MARGEM, "");
            pausar(); return;
        }
    }

    novo.ano = ler_inteiro("Ano: ", MIN_ANO, MAX_ANO);
    novo.nota = ler_inteiro("Nota: ", 0, 10);
    novo.hora_participacao = ler_inteiro("Hora de participação: ", 0, 9999);

    turmas[total_turmas] = novo; total_turmas++;
    ++cursos[buscar_curso_codigo(novo.codigo_curso)].participantes;

    salvar_turma(); salvar_cursos();
    printf("%*s[OK] Turma criada e/ou discente adicionado na turma!", MARGEM, "");
    pausar();
}

void editar_turma() {
    cabecalho("TURMA > EDITAR");
    int n_turma = ler_inteiro("Número da turma: ", 1, MAX_TURMAS);
    if (buscar_turma_numero(n_turma) == -1) { // verifica se o numero da turma existe.
        printf("\n%*s[!] Turma não encontrada!", MARGEM, ""); pausar(); return;
    }

    int i; // visual
    printf("\n  %-10s %-15s %-10s %-10s %-10s %-10s",
            "TURMA", "CPF", "CURSO", "ANO", "NOTA", "HORAS");
    puts("\n  ----------------------------------------------------------------------");
    // lista os matriculados
    for (i = 0; i < total_turmas; i++) {
        if (turmas[i].numero == n_turma) { //<--- só lista o numero escolhido
            printf("  %-10d %-15s %-10s %-10d %-10.2f %-10d\n",
                turmas[i].numero, turmas[i].cpf, turmas[i].codigo_curso,
                turmas[i].ano, turmas[i].nota, turmas[i].hora_participacao);
        }
    }

    char cpf_discente[MAX_CPF]; // pede o cpf do discente e verifica
    ler_string("\nCPF do Discente: ", cpf_discente, MAX_CPF);

    int it = buscar_turma_numero_cpf(n_turma, cpf_discente);
    if (it == -1) {
        printf("%*s[!] Aluno não registrado na turma.", MARGEM, "");
        pausar(); return;
    }
    cabecalho("TURMA > EDITAR > PERFIL");

    // mostra as legendas das opcao para escolher
    printf("\n\t%s\n", discentes[buscar_discente_cpf(cpf_discente)].nome);
    printf("\n  %-10s%-10s%-10s\n", "NOTA", "HORA", "ANO");
    printf("  %-10.2f%-10d%-10d\n", turmas[it].nota,
        turmas[it].hora_participacao, turmas[it].ano);

    printf("\n%*s(Deixe em branco para manter)\n", MARGEM, "");

    char buf[5]; float valor_f; int valor_i;
    printf("\n   Nova nota: "); // solicitação
    fgets(buf, sizeof(buf), stdin);
    if (sscanf(buf, "%f", &valor_f) == 1 && valor_f >= 0 && valor_f <= 10)
        turmas[it].nota = valor_f; //limpar_buffer();

    printf("   Nova hora: "); // solicitação
    fgets(buf, sizeof(buf), stdin);
    if (sscanf(buf, "%d", &valor_i) == 1 && valor_i > 0)
        turmas[it].hora_participacao = valor_i;

    printf("   Novo ano [2000 - 2026]: "); // solicitação
    fgets(buf, sizeof(buf), stdin); limpar_buffer_v2(buf);
    if (sscanf(buf, "%d", &valor_i) == 1 && valor_i >= MIN_ANO && valor_i <= MAX_ANO) {
        turmas[it].ano = valor_i;
    }

    salvar_turma();
    printf("%*s[OK]  Turma atualizada!\n", MARGEM, ""); pausar();
}

void excluir_turma() {
    cabecalho("TURMA > EXCLUIR");

    int numero = ler_inteiro("Número da Turma: ", 0, total_turmas);
    if (buscar_turma_numero(numero) == -1) { // verifica se o numero da turma existe.
        printf("\n%*s[!] Turma não encontrada!", MARGEM, ""); pausar(); return;
    }

    int itx = buscar_turma_numero(numero);

    int i, contador = 0; // conta os matriculados
    for (i = 0; i < total_turmas; i++) {
        if (turmas[i].numero == numero) contador++;
    }

    printf("\n%*sTurma tem %d discente(s) matriculado(s)\n", MARGEM, "", contador);
    printf("%*sExcluir turma %d de %s? (s/n): ", MARGEM, "", numero, cursos[itx].nome);

    char sn[2];
    ler_string("", sn, 2);
    if (sn[0] != 's' && sn[0] != 'S') {
        printf("\n%*s[!] Exclusão Cancelada.", MARGEM, ""); pausar(); return;
    }

    // Exclui todos discente da turma a ser removida.
    while (itx != -1) {
        for (int i = itx; i < total_turmas; i++) {
            turmas[i] = turmas[i + 1];
        }
        total_turmas--;
        itx = buscar_turma_numero(numero);
    }

    salvar_turma(); pausar(); return;
}

void pesquisar_turma() {
    cabecalho("TURMA > PESQUISAR");

    int n_turma = ler_inteiro("Número da turma: ", 0, 99);

    int i;
    gotoxy(MARGEM, 8); // Exibe a tabela na parte inferior
    printf("\n  %-10s %-15s %-10s %-10s %-10s %-10s",
            "TURMA", "CPF", "CURSO", "ANO", "NOTA", "HORAS");
    puts("\n  ----------------------------------------------------------------------");

    for (i = 0; i < total_turmas; i++) {
        if (turmas[i].numero == n_turma) {
            printf("  %-10d %-15s %-10s %-10d %-10.2f %-10d\n",
                turmas[i].numero, turmas[i].cpf, turmas[i].codigo_curso,
                turmas[i].ano, turmas[i].nota, turmas[i].hora_participacao);
        }
    }

    gotoxy(30, 2); // Exibe o nome do curso na parte superior
    printf("-  %s", cursos[buscar_curso_codigo(turmas[n_turma].codigo_curso)].nome);
    gotoxy(MARGEM, 4);

    pausar();
}

void menu_turmas() {
    int op;
    do {
        cabecalho("TURMAS");
        gotoxy(MARGEM + 0, 2); printf("[1] Inserir");
        gotoxy(MARGEM + 20, 2); printf("[2] Editar");
        gotoxy(MARGEM + 40, 2); printf("[3] Excluir");
        gotoxy(MARGEM + 0, 3); printf("[4] Pesquisar");
        gotoxy(MARGEM + 40, 3); printf("[0] Voltar");
        gotoxy(MARGEM + 0, 5);
        gotoxy(MARGEM + 10, 10); printf("Em desenvolvimento");
        gotoxy(MARGEM + 0, 6);

        switch (op = ler_inteiro("Opção: ", 0, 4)) {
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

    printf("\n  %-10s %-15s %-40s %-10s", "TURMA", "CPF", "NOME", "NOTA");
    puts("\n  ----------------------------------------------------------------------");

    int i, id; // id: índice de discente
    for (i = 0; i < total_turmas; i++) {
        id = buscar_discente_cpf(turmas[i].cpf);
        printf("  %-10d %-15s %-40s %10.2f \n", turmas[i].numero, turmas[i].cpf,
                                                discentes[id].nome, turmas[i].nota);
    }
    puts(""); pausar();
}

void relat_E() {
    cabecalho("RELATORIO > E");

    if (total_turmas == 0) {
        printf("[!] Nenhuma turma cadastrada."); pausar(); return;
    }

    printf("\n  %-7s %-15s %-40s %10s %6s %-6s", "TURMA", "CPF", "NOME",
                                                  "NOTA", "COD", "CURSO");
    puts("\n  ----------------------------------------------------------------------");

    int i, id, ic; // id: índice de discente | ic: índice de curso
    for (i = 0; i < total_turmas; i++) {
        id = buscar_discente_cpf(turmas[i].cpf);
        ic = buscar_curso_codigo(turmas[i].codigo_curso);
        printf("  %-7d %-15s %-40s %10.2f %6s %-s \n",
                                        turmas[i].numero, turmas[i].cpf,
                                      discentes[id].nome, turmas[i].nota,
                                  turmas[i].codigo_curso, cursos[ic].nome);

    }
    puts(""); pausar();
}

void relat_F() {
    cabecalho("RELATORIO > F");

    int n_turma = ler_inteiro("Número da turma: ", 0, 99);

    int i;
    gotoxy(MARGEM, 8); // Exibe a tabela na parte inferior
    printf("\n  %-10s %-15s %-10s %-10s %-10s %-10s",
            "TURMA", "CPF", "CURSO", "ANO", "NOTA", "HORAS");
    puts("\n  ----------------------------------------------------------------------");

    if (buscar_turma_numero(n_turma) != -1) {
        for (i = 0; i < total_turmas; i++) {
            if (turmas[i].numero == n_turma) {
                printf("  %-10d %-15s %-10s %-10d %-10.2f %-10d\n",
                    turmas[i].numero, turmas[i].cpf, turmas[i].codigo_curso,
                    turmas[i].ano, turmas[i].nota, turmas[i].hora_participacao);
            }
        }

        gotoxy(30, 2); // Exibe o nome do curso na parte superior
        printf("-  %s", cursos[buscar_curso_codigo(turmas[n_turma].codigo_curso)].nome);
        gotoxy(MARGEM, 4);
    }
    pausar();
}

void relat_G() {
    cabecalho("RELATORIO > G");
    // nº da turma | Qtd de Alunos | Soma | Média
    int n_turma = 1, it, i;
    int qtd_aluno[MAX_SALAS] = {}; // qtd de aluno por sala (índice = sala)
    float somatorio_nota[MAX_SALAS] = {};

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

    for (i = 1; i <= MAX_SALAS; i++) {
        if (qtd_aluno[i] == 0) continue;
        printf("\n%d|%d|%.2f|%.2f", i, qtd_aluno[i], somatorio_nota[i], somatorio_nota[i] / qtd_aluno[i]);
    }

    pausar();
}

void menu_relatorio() {
    char op[5];
    do {
        cabecalho("RELATÓRIOS");
        gotoxy(MARGEM + 0, 2); printf("[A] CPF, NOME e IDADE dos discentes");
        gotoxy(MARGEM + 0, 3); printf("[B] CÓDIGO, NOME, HORAS e VAGAS dos cursos");
        gotoxy(MARGEM + 0, 4); printf("[C] Discentes por palavras no nome");
        gotoxy(MARGEM + 0, 5); printf("[D] Turma, CPF, NOME e NOTA");
        gotoxy(MARGEM + 0, 6); printf("[E] Turma, DISCENTES, NOTA e CURSO");
        gotoxy(MARGEM + 0, 7); printf("[F] Discentes por número de turma");
        gotoxy(MARGEM + 0, 8); printf("[G] Turmas com médias das notas");
        gotoxy(MARGEM + 0, 9); printf("[0] Voltar");

        gotoxy(MARGEM + 0, 11); printf("Escolha uma das opções.\n");

        ler_string("OPÇÃO: ", op, 5);
        if (op[0] == '0') break;
        switch (tolower(op[0])) {
            case 'a': relat_A(); break;
            case 'b': relat_B(); break;
            case 'c': relat_C(); break;
            case 'd': relat_D(); break;
            case 'e': relat_E(); break;
            case 'f': relat_F(); break;
            case 'g': relat_G(); break;
            default: printf("\n%*s[!] Inválido!", MARGEM, ""); pausar();
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
        gotoxy(MARGEM + 20, 3); printf("[4] Relatório");
        gotoxy(MARGEM + 40, 4); printf("[0] Sair");
        gotoxy(MARGEM + 0, 5);
        gotoxy(MARGEM + 10, 10); printf("Escolha uma das opções.");
        gotoxy(MARGEM + 0, 6);
        /* Validando numeros inteiros */
        op = ler_inteiro("OPÇÃO: ", 0, 4);
        switch (op) {
            case 1: menu_discentes(); break;
            case 2: menu_cursos(); break;
            case 3: menu_turmas(); break;
            case 4: menu_relatorio(); break;
            case 0: cabecalho("ATÉ LOGO!");
                printf("\n%*sSistema encerrado.\n\n", MARGEM, ""); break;
        }
    } while (op != 0);
}

/* ==================== MAIN =================== */

int main() {
    system("chcp 65001>nul");
    //setlocale(LC_ALL, "Portuguese_Brazil.65001");
    //setlocale(LC_ALL, "Portuguese");
    carregar_discentes();
    carregar_cursos();
    carregar_turma();

    menu_principal();

    return 0;
}

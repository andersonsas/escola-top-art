#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ===================== CONSTANTES ===================== */
#define MAX_CPF 20
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

void cabecalho(const char titulo[]) {
     system("cls");
     printf("   ESCOLA TOP-ART-BARE  |  %s\n", titulo);
     printf("\n");
}

/* =============== VALIDACOES ========================= */
int cpf_valido(const char cpf[]) {
     int i;
     if (strlen(cpf) != 11) return 0;

     for (i = 0; i < 11; i++) {
          if (!isdigit(cpf[i])) {
               return 0;
          }
     }
     return 1;  // Passou em todos os testes
}

/* ===================== LEITURA SEGURA ===================== */

// Ler uma string estabelecendo uma legenda e um limite de leitura
void ler_string(const char legenda[], char dest[], int max) {
     printf("%s", legenda);
     fgets(dest, max, stdin);
     dest[strcspn(dest, "\n")] = '\0';
}

// Ler um número inteiro estabelendo uma legenda e intervalo
int ler_inteiro(const char legenda[], int min, int max_val) {
    int valor; char buf[50];

    while (1) {
         printf("%s", legenda);
         fgets(buf, sizeof(buf), stdin);
         if (sscanf(buf, "%d", &valor) == 1 && valor >= min && valor <= max_val) 
            return valor;
         printf("  [!] Valor inválido. Digite entre %d e %d.\n", min, max_val);
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
     FILE* f = fopen(ARQ_DISCENTES, "w");
     if (!f) { printf("  [!] Erro ao salvar discentes.\n"); return; }

     int i;
     for (i = 0; i < total_discentes; i++)
        fprintf(f, "%s|%s|%d\n", 
                discentes[i].cpf, 
                discentes[i].nome,
                discentes[i].idade);
     fclose(f);
}

/* ======================== CURSO - ARQUIVO ======================== */
void salvar_cursos(){
    FILE *f =fopen(ARQ_CURSOS, "w"); if(!f) {pausar();return;}

    int i;
    for (i = 0; i < total_cursos; i++){
        fprintf(f, "%s|%s|%d|%d|%d\n",  cursos[i].codigo,
                                        cursos[i].nome,
                                        cursos[i].horas,
                                        cursos[i].vagas,
                                        cursos[i].participantes);
    }
    fclose(f);
}

void carregar_cursos(){
    FILE *f = fopen(ARQ_CURSOS, "r"); if(!f) return;

    total_cursos = 0;
    char linha[100];
    while(total_cursos < MAX_CURSOS && fgets(linha, sizeof(linha), f) != NULL){
        char *token = strtok(linha, "|");
        if(token) strcpy(cursos[total_cursos].codigo, token);
        
        token = strtok(NULL, "|");
        if(token) strcpy(cursos[total_cursos].nome, token);
        
        token = strtok(NULL, "|");
        if(token) cursos[total_cursos].horas = atoi(token);
        
        token = strtok(NULL, "|");
        if(token) cursos[total_cursos].vagas = atoi(token);
        
        token = strtok(NULL, "|");
        if(token) cursos[total_cursos].participantes = atoi(token);

        total_cursos++;
    }
    
    fclose(f);
}

/* ===================== FUNÇÕES BUSCA AUXILIAR ===================== */

// Busca o índice do discente, caso contrário -1
int buscar_discente_cpf(char cpf[]) {
     int i;
     for (i = 0; i < total_discentes; i++) {
          if (strcmp(discentes[i].cpf, cpf) == 0) {
               return i;
          }
     }
     return -1;
}

int buscar_curso_codigo(const char busca[]){
    int i;
    for(i = 0; i < total_cursos; i++)    
        if(strcmp(cursos[i].codigo, busca) == 0) return i;
    return -1;
}

void listar(){
    int i;
    for(i = 0; i < total_discentes; i++){
        printf("\t%s ____ %s\n",discentes[i].cpf, discentes[i].nome);
    }
    puts("------------------------------");
}

/* ======================== MENU DISCENTE - CRUD ========================= */
void editar_discente(){
    cabecalho(" DISCENTE > EDITAR");

    listar();

    char cpf[MAX_CPF];
    ler_string(" CPF do discente: ", cpf, MAX_CPF);
    int idx = buscar_discente_cpf(cpf);
    if(idx < 0){
        printf(" Discente não existe"); pausar(); return;
    }
    //printf("--> %s\n", discentes[idx].nome);
    
    printf("%-15s %-40s %s\n", "[1] CPF", "[2] NOME", "[3] IDADE" );
    printf("%-15s %-40s %d", discentes[idx].cpf, 
                             discentes[idx].nome, 
                             discentes[idx].idade);
    
    int opt = ler_inteiro(" \n--> Escolha o que editar: ", 0, 3);

    char novo_cpf[MAX_CPF];
    char novo_nome[MAX_NOME];
    int nova_idade;
    switch (opt) {
        case 1:
            ler_string("  Novo CPF: ",novo_cpf, MAX_CPF);
            strcpy(discentes[idx].cpf, novo_cpf);
            break;
        case 2:
            ler_string("  Novo Nome: ", novo_nome, MAX_NOME);
            strcpy(discentes[idx].nome, novo_nome);
            break;
        case 3:
            nova_idade = ler_inteiro("  Nova idade: ", 5, 120);
            discentes[idx].idade = nova_idade;
            break;                    
    }

    if(opt != 0) salvar_discentes();
    pausar();
}

void excluir_discente(){
    cabecalho("DISCENTE > EXCLUIR");

    listar();

    char cpf[MAX_CPF]; // entrada do usuário
    ler_string("  CPF do discente: ", cpf, MAX_CPF);    

    int idx = buscar_discente_cpf(cpf);
    if(idx < 0){ // Caso de discente não registrado
        printf("\n  [!] Discente não está registrado.\n"); pausar(); return;
    }

    char sn[3];
    printf("  Excluir %s ", discentes[idx].nome);
    ler_string(" ? (s/n) : ", sn, 3);
    if(sn[0] != 's' && sn[0] != 'S'){
        printf("\n\tOperação cancelada."); pausar(); return;
    }

    for(int i = idx; i < total_discentes; i++){
        discentes[i] = discentes[i + 1];
    }

    total_discentes--;       
    
    salvar_discentes();
    printf("  [!] Discente Excluído."); pausar(); return;    
}

void inserir_discente() {
    cabecalho("DISCENTES > INSERIR");   

    // formulário
     Discente novo;
     ler_string("  CPF (apenas dígitos): ", novo.cpf, MAX_CPF);     

     ler_string("  Nome: ", novo.nome, MAX_NOME);
     if (strlen(novo.nome) < 3) {
          printf("  [!] Nome muito curto.\n");
          pausar();
          return;
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
        printf(" [1] Inserir\n");
        printf(" [2] Editar\n");
        printf(" [3] Excluir\n");
        printf(" [4] Pesquisar\n");
        printf(" [0] Voltar\n");
        op = ler_inteiro("     Opção: ", 0, 4);
        switch (op) {
                case 1: inserir_discente(); break;
                case 2: editar_discente(); break;
                case 3: excluir_discente(); break;
             // case 4: pesquisar_discente(); break;
        }
     } while (op != 0);
}

/* ================ MENU CURSO - CRUD ============*/
void excluir_curso(){
    cabecalho("   CURSO > EXCLUIR");

    char cod_alto[MAX_CODIGO];
    ler_string("  Código: ", cod_alto, MAX_CODIGO);

    int cdx = buscar_curso_codigo(cod_alto);
    if(cdx < 0) {printf(" [!] Curso não existe"); pausar(); return;}

    int i;
    for(i = cdx; i < total_cursos; i++) {
        cursos[i] = cursos[i + 1];
    }
    
    total_cursos--;
    salvar_cursos();
    pausar();
    
}

void editar_curso(){

}

void inserir_curso(){
    cabecalho("CURSO > INSERIR");

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

void menu_cursos(){
    int op;
    do
    {
        cabecalho("CURSO");
        printf(" [1] Inserir\n");
        printf(" [2] Editar\n");
        printf(" [3] Excluir\n");
        printf(" [4] Pesquisar\n");
        printf(" [0] Sair\n");
        op = ler_inteiro("     Opção: ", 0 , 4);
        switch (op) {
        case 1: inserir_curso(); break;
        case 2: editar_curso();  break;
        case 3: excluir_curso(); break;
        
        default:
            break;
        }
        
    } while (op != 0);
    
}


/* ================ MENU PRINCIPAL ===============*/
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
        op = ler_inteiro("     Opção: ", 0, 4);
        switch (op) {
            case 1:
                   menu_discentes(); break;
            case 2:
                   menu_cursos(); break;
            case 3:
                   printf("Vc esta na op3"); break;
            case 4:
                   printf("Vc esta na op4"); break;
            case 0:
                   cabecalho("ATÉ LOGO!");
                   printf("  Sistema encerrado.\n\n"); break;
        }
    } while (op != 0);
}

/* ================ MAIN ===============*/
int main() {
     system("chcp 65001 > nul");
     carregar_discentes();
     carregar_cursos();

     menu_principal();

     return 0;
}

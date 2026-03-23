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
#define MAX_ANO 2026

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

float ler_float(const char legenda[], const float minf, const float maxf){
    char buf[10]; float valor;
    while(1){
        printf("%s", legenda);
        fgets(buf, sizeof(buf), stdin);
        if(sscanf(buf, "%f", &valor) == 1 && valor > minf && valor < maxf) return valor;
        printf(" [] Valor invalido, Digite entre %.2f e %.2f.\n", minf, maxf);
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
/* ======================== TURMA - ARQUIVO ========================*/
void carregar_turma(){
    FILE *f = fopen(ARQ_TURMAS, "r");
    if(!f){printf(" [!] Carregar turma falhou"); pausar(); return;}

    char linha[128];
    total_turmas = 0;
    while(total_turmas < MAX_TURMAS && fgets(linha, sizeof(linha), f) != NULL){
        char *token = strtok(linha, "|");
        if(token) turmas[total_turmas].numero = atoi(token);
        
        token = strtok(NULL, "|");
        if(token) strcpy(turmas[total_turmas].cpf, token);
        
        token = strtok(NULL, "|");
        if(token) strcpy(turmas[total_turmas].codigo_curso, token);
        
        token = strtok(NULL, "|");
        if(token) turmas[total_turmas].ano = atoi(token);
        
        token = strtok(NULL, "|");
        if(token) turmas[total_turmas].nota = atof(token);

        token = strtok(NULL, "|\n");
        if(token) turmas[total_turmas].hora_participacao = atoi(token);

        total_turmas++;
    }
    fclose(f);
}

void salvar_turma(){
    FILE *f = fopen(ARQ_TURMAS, "w");
    if(!f){printf("  [!] Salvar turma falhou"); pausar(); return;}

    for(int i = 0; i < total_turmas; i++)
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
     for (i = 0; i < total_discentes; i++) {
          if (strcmp(discentes[i].cpf, cpf) == 0) {
               return i;
          }
     }
     return -1;
}

// Busca o índice do curso pelo código
int buscar_curso_codigo(const char busca[]){
    int i;
    for(i = 0; i < total_cursos; i++)    
        if(strcmp(cursos[i].codigo, busca) == 0) return i;
    return -1;
}

// Busca o primeiro índice da turma pelo número
int buscar_turma_numero(const int busca){
    int i;
    for(i = 0; i < total_turmas; i++){
        if(busca == turmas[i].numero) return i;}    
    return -1;
}

// Retorna o índice da tuma onde o discente é matriculado.
int buscar_discente_turma(const int numero, const char cpf[]){
    int i; // o discente está na turma? -> retorna o indice do arrays de turma
    for(i =0; i < total_turmas; i++){
        if(turmas[i].numero == numero && strcmp(turmas[i].cpf,cpf) == 0){
            return i;
        }
    }
    return -1;
}

/* ======================== *LISTAGEM* =================================  */
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
    if(idx < 0){printf(" Discente não existe"); pausar(); return;}    
    
    printf("\t%-15s %-40s %s\n", "[1] CPF", "[2] NOME", "[3] IDADE" );
    printf("\t%-15s %-40s %d", discentes[idx].cpf, 
                             discentes[idx].nome, 
                             discentes[idx].idade);
    
    int opt = ler_inteiro(" \n\n Escolha o que editar: ", 0, 3);

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
    if(buscar_discente_cpf(novo.cpf) >= 0){
        printf(" [!] Discente já registrado!\n"); pausar(); return;
    }

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

    char cod_alvo[MAX_CODIGO];
    ler_string("  Código: ", cod_alvo, MAX_CODIGO);

    int cdx = buscar_curso_codigo(cod_alvo);
    if(cdx < 0) {printf(" [!] Curso não existe"); pausar(); return;}

    int i;
    for(i = cdx; i < total_cursos; i++) {cursos[i] = cursos[i + 1];}
    
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
        printf(" [0] Voltar\n");
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
/* ================ TURMA - CRUD ===================*/
void inserir_turma(){
    cabecalho("INSERIR > TURMA");
    
    Turma novo;
    novo.numero = ler_inteiro(" Número da Turma: ", 0, MAX_TURMAS);

    int i;
    // verifica se a turma já tem um curso vinculado
    for(i = 0; i < total_turmas; i++){
        if(turmas[i].numero == novo.numero){ // se já existe ...        
            strcpy(novo.codigo_curso, turmas[i].codigo_curso);
            printf("\tTurma de %s - %s\n",  novo.codigo_curso, 
                                            cursos[buscar_curso_codigo(novo.codigo_curso)].nome);
            i = -1; break;}
    }
    
    // se não há, então insere um código de curso na nova turma/sala
    if(i != -1) ler_string(" Código do curso: ", novo.codigo_curso, MAX_CODIGO);
    if(buscar_curso_codigo(novo.codigo_curso) == -1 ){
        printf(" [!] Curso não registrado."); pausar(); return;
    }

    ler_string( " CPF: ", novo.cpf, MAX_CPF);
    if(buscar_discente_cpf(novo.cpf) == -1){
        printf(" [!] Discente não está registrado na escola."); pausar(); return;
    }

    for(i = 0; i < total_turmas; i++){
        if(turmas[i].numero == novo.numero && strcmp(turmas[i].cpf, novo.cpf) == 0){
            printf(" [!] Discente já está matriculado na turma."); 
            pausar(); return;}
    }

    novo.ano  = ler_inteiro(" Ano: ", 2000, MAX_ANO);
    novo.nota = ler_inteiro(" Nota: ", 0, 10);
    novo.hora_participacao = ler_inteiro (" Hora de participação: ", 0, 9999);

    turmas[total_turmas] = novo; total_turmas++;
    ++cursos[buscar_curso_codigo(novo.codigo_curso)].participantes;

    salvar_turma(); salvar_cursos();
    printf(" [OK] Turma criada e/ou discente adicionado na turma!");
    pausar();
}

void editar_turma(){
    cabecalho("TURMA > EDITAR");
    int n_turma = ler_inteiro("  Número da turma: ", 1, MAX_TURMAS);
    if(buscar_turma_numero(n_turma) == -1){ // verifica se o numero da turma existe.
        printf("\n  [!] Turma não encontrada!"); pausar(); return;
    }

    int i; // visual
    printf("\n  %-10s %-15s %-10s %-10s %-10s %-10s",
            "TURMA", "CPF", "CURSO", "ANO", "NOTA", "HORAS");
    puts("\n  ----------------------------------------------------------------------");
    // lista os matriculados
    for (i = 0; i < total_cursos; i++){
        if(turmas[i].numero == n_turma){ //<--- só lista o numero escolhido
            printf("  %-10d %-15s %-10s %-10d %-10.2f %-10d\n", 
                turmas[i].numero, turmas[i].cpf, turmas[i].codigo_curso,
                turmas[i].ano, turmas[i].nota, turmas[i].hora_participacao);
        }
    }

    // pede o cpf do discente e verifica
    char cpf_discente[MAX_CPF];
    ler_string("\n  CPF do Discente: ", cpf_discente, MAX_CPF);
    int it = buscar_discente_turma(n_turma, cpf_discente);
    if(it == -1){ printf("  [!] Aluno não registrado na turma."); pausar(); return;}

    // mostra as legendas das opcao para escolher
    printf("\n\t%s\n", discentes[buscar_discente_cpf(cpf_discente)].nome);
    printf("\n  %-10s  %-10s%-10s\n", "[1] NOTA", "[2] HORA", "[3] ANO");
    printf("    %-10.2f%-10d%-10d\n", turmas[it].nota, turmas[it].hora_participacao, turmas[it].ano);

    char buf[5]; float fval; int ival;
    printf("\n   Nova nota: ");
    fgets(buf, sizeof(buf), stdin);
    if(sscanf(buf, "%f", &fval) == 1 && fval >= 0 && fval <= 10)
        turmas[it].nota = fval;    
    
    printf("   Nova hora: ");
    fgets(buf, sizeof(buf), stdin);
    if(sscanf(buf, "%d", &ival) == 1 && ival > 0)
        turmas[it].hora_participacao = ival;

    printf("   Novo ano [2000 - 2026]: ");
    fgets(buf, sizeof(buf), stdin);
    if(sscanf(buf, "%d", &ival) && ival > 2000 && ival < 2026)
        turmas[it].ano = ival; 
    
    salvar_turma();
    printf("  [OK]  Turma atualizada!\n");      
    pausar();
}

void excluir_turma(){
    cabecalho( "TURMA > EXCLUIR");

    ler_inteiro(" Número da Turma: ", 0, total_turmas);

    
    pausar(); return;
}

void pesquisar_turma(){
    cabecalho("TURMA > PESQUISAR");

    int n_turma = ler_inteiro("  Número da turma: ", 0, total_turmas);    
    int i;
    printf("\n  %-10s %-15s %-10s %-10s %-10s %-10s",
            "TURMA", "CPF", "CURSO", "ANO", "NOTA", "HORAS");
    puts("\n  ----------------------------------------------------------------------");

    for (i = 0; i < total_cursos; i++){
        if(turmas[i].numero == n_turma){
            printf("  %-10d %-15s %-10s %-10d %-10.2f %-10d\n", 
                turmas[i].numero, turmas[i].cpf, turmas[i].codigo_curso,
                turmas[i].ano, turmas[i].nota, turmas[i].hora_participacao);
        }
    }    
    pausar();
}

void menu_turmas(){
    int op;
    do {
        cabecalho("TURMAS");
        printf(" [1] Inserir\n");
        printf(" [2] Editar\n");
        printf(" [3] Excluir\n");
        printf(" [4] Pesquisar\n");
        printf(" [0] Voltar\n");
        
        switch( op = ler_inteiro( " Opção: ", 0, 4) ){
            case 1:  inserir_turma();  break;
            case 2:  editar_turma();   break;
            case 3:  excluir_turma();  break;
            case 4: pesquisar_turma(); break;
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
            case 1: menu_discentes(); break;
            case 2: menu_cursos(); break;
            case 3: menu_turmas(); break;
            case 4: printf("Vc esta na op4"); break;
            case 0: cabecalho("ATÉ LOGO!");
                    printf("  Sistema encerrado.\n\n"); break;
        }
    } while (op != 0);
}

/* ================ MAIN ===============*/
int main() {
    system("chcp 65001 > nul");
    carregar_discentes();
    carregar_cursos();
    carregar_turma();

    menu_principal();

    return 0;
}

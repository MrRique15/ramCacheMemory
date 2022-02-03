#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_RAM_LINES 1000
#define MEMORY_BLOCK_SIZE 5
#define MEMORY_BLOCKS (MAX_RAM_LINES / MEMORY_BLOCK_SIZE)
#define MAX_CACHE_LINES 5
#define MIN_INT_GENERATE 100
#define MAX_INT_GENERATE 999
#define CLEAR_SCREEN "clear"

//-----------------------------------
// ESTRUTURAS DE MEMORIA
//-----------------------------------
//Estrutura contendo os dados de uma linha da memoria principal
typedef struct {
    int content;          // conteudo da linha
    int identificator;    //Identificador da linha dentro da RAM
    int blockID;          //Identificador do bloco da linha dentro da RAM
} RamLine;

//Estrutura contendo os dados de UMA linha da memória cache, utilizando um vetor que contém um numero MEMORY_BLOCK_SIZE de linhas provenientes da RAM
typedef struct {
    int filled;                        //0 - vazio, 1 - preenchido
    int acess_count;                   //contador de acessos
    int update_count;                  //contador de atualizações
    RamLine r_lines[MEMORY_BLOCK_SIZE];//vetor de linhas provenientes da RAM //m_cache[i].r_lines[j].content//
} CacheLine;
//-----------------------------------
// FUNCOES GERAIS
//-----------------------------------

//Gera um numero inteiro indo de MIN_INT_GENERATE ate MAX_INT_GENERATE
int sort_int(int min, int max){
    int num;
    num = rand() % (max - min + 1) + min;
    return num;
}

//Imprime uma certa linha da memória RAM
void print_ram_line(RamLine r_line){
    printf("---------------------------------\n");
    printf("[%d] |", r_line.identificator);
    printf(" Content: %d |", r_line.content);
    printf(" BlockID: %d\n", r_line.blockID);
}

//Imprime a memória RAM completa
void print_full_ram(RamLine r_lines[MAX_RAM_LINES]){
    int i;
    int aux = 1;
    for(i = 0; i < MAX_RAM_LINES; i++){
        if(aux == 1){
            printf("----------------------------------------------------\n");
        }
        print_ram_line(r_lines[i]);
        aux++;
        if(aux == MEMORY_BLOCK_SIZE){
            aux = 0;
        }
    }
    printf("----------------------------------------------------\n");
}

//Imprime a memória CACHE completa
void print_full_cache(CacheLine m_cache[MAX_CACHE_LINES]){
    printf("----------------------------------------------------\n");
    for(int i = 0; i < MAX_CACHE_LINES; i++){
        //printf("[%d] | Preenchido: %d | Acessos: %d | Atualizações: %d\n", i, m_cache[i].filled, m_cache[i].acess_count, m_cache[i].update_count);
        printf("[%d]", i);
        if(m_cache[i].filled == 1){
            for(int j = 0; j < MEMORY_BLOCK_SIZE; j++){
                //printf("[%d] | Conteúdo: %d | Bloco: %d\n", m_cache[i].r_lines[j].identificator, m_cache[i].r_lines[j].content, m_cache[i].r_lines[j].blockID);
                printf(" | %d", m_cache[i].r_lines[j].content);
            }
        }
        printf("\n");
        printf("----------------------------------------------------\n");
    }
    
}

//Altera o conteúdo de uma linha da memória RAM
void change_line_content(RamLine *r_line, int content){
    r_line->content = content;
}

//Inicializa a RAM com números aleatórios em cada linha
void initialize_ram(RamLine m_ram[MAX_RAM_LINES]){
    int filled_blocks = 0;
    int filled_lines = 0;

    for(int i = 0; i < MAX_RAM_LINES; i++){
        m_ram[i].content = sort_int(MIN_INT_GENERATE, MAX_INT_GENERATE);
        m_ram[i].identificator = i;
        if(filled_lines < MEMORY_BLOCK_SIZE) {
            filled_lines++;
            m_ram[i].blockID = filled_blocks;
        }else{
            filled_lines = 1;
            filled_blocks++;
            m_ram[i].blockID = filled_blocks;
        }
    }
}

//Coloca um bloco da memória RAM em alguma posição da memória CACHE
void fill_cache_line(CacheLine m_cache[MAX_CACHE_LINES], RamLine m_ram[MAX_RAM_LINES], int ram_line_number){
    int found = 0, block = m_ram[ram_line_number].blockID, count = 0;

    for(int i = 0; i < MAX_CACHE_LINES; i++){
        if(m_cache[i].filled == 0){
            for(int j = 0; j < MAX_RAM_LINES; j++){
                if(count < MEMORY_BLOCK_SIZE && m_ram[j].blockID == m_ram[ram_line_number].blockID){
                    m_cache[i].r_lines[count].content = m_ram[j].content;
                    m_cache[i].r_lines[count].identificator = m_ram[j].identificator;
                    m_cache[i].r_lines[count].blockID = m_ram[j].blockID;
                    count++;
                }
            }
            m_cache[i].filled = 1;
            found = 1;
            break;
        }
    }
    if(found == 0){
        printf("Erro ao preencher a cache\n");
    }
}

//Acessar registro da RAM e armazená-lo na memória CACHE
void acess_ram_register(CacheLine m_cache[MAX_CACHE_LINES], RamLine m_ram[MAX_RAM_LINES]){
    int resp = 0, found = 0;

    printf("Insira o número da linha que deseja acessar: \n");
    scanf("%d", &resp);
    for(int i = 0; i < MAX_CACHE_LINES; i++){
        if(m_cache[i].filled == 1){
            for(int j = 0; j < MEMORY_BLOCK_SIZE; j++){
                if(m_cache[i].r_lines[j].identificator == resp){
                    m_cache[i].acess_count++;
                    found = 1;
                    printf("O conteúdo da linha %d já está registrado na LINHA: %d da memória CACHE\n", resp, i);
                    print_full_cache(m_cache);
                }
            }
        }
    }
    if(found == 0){
        printf("O conteúdo da linha %d não está registrado na memória CACHE\n", resp);
        fill_cache_line(m_cache,m_ram,resp);
    }
    
}

//Inicializa a memória cache, deixando-a vazia
void initialize_cache(CacheLine m_cache[MAX_CACHE_LINES]){
    for(int i = 0; i < MAX_CACHE_LINES; i++){
        m_cache[i].filled = 0;
        m_cache[i].acess_count = 0;
        m_cache[i].update_count = 0;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void infos_start(){
    printf("----------------------------------------------------\n");
    printf("\t\t\tSimulador de Memória Cache\n");
    printf("----------------------------------------------------\n");
}

int main_menu(){
    int option;
    printf("Opções Disponíveis:\n");
    printf("0 - Encerrar\n");
    printf("1 - Mostrar toda a memória RAM\n");
    printf("2 - Mostrar toda a memória Cache\n");
    printf("3 - Selecionar método de alteração da CACHE (padrao - aleatorio)\n");
    printf("4 - Acessar registros da RAM\n");
    scanf("%d", &option);
    system(CLEAR_SCREEN);
    return option;
}

void invalid_option(){
    printf("Opção Inválida!\n");
}
void shutdown_message(){
    printf("Somulação sendo encerrada...\n");
}

void select_method(int *option){
    printf("----------------------------------------------------\n");
    printf("Escolha uma opção:\n");
    printf("1 - Aleatório\n");
    printf("2 - Sequencial\n");
    printf("3 - FIFO\n");
    scanf("%d", option);
    if (*option < 1 || *option > 3){
        invalid_option();
        select_method(option);
    }else{
        printf("----------------------------------------------------\n");
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void main(){
    RamLine m_ram[MAX_RAM_LINES];
    CacheLine m_cache[MAX_CACHE_LINES];
    int resp = 1, metodo = 1;

    infos_start();
    initialize_ram(m_ram);
    initialize_cache(m_cache);

    do{
        switch(main_menu()){
            case 0:
                shutdown_message();
                resp = 0;
            break;

            case 1:
                print_full_ram(m_ram);
            break;

            case 2:
                print_full_cache(m_cache);
            break;

            case 3:
                select_method(&metodo);
            break;

            case 4:
                acess_ram_register(m_cache, m_ram);
            break;

            default:
                invalid_option();
            break;
        }
    } while(resp != 0);
}
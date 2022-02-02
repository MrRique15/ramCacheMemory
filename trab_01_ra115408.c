#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_RAM_LINES 100
#define MEMORY_BLOCK_SIZE 5
#define MEMORY_BLOCKS (MAX_RAM_LINES / MEMORY_BLOCK_SIZE)

#define MAX_CACHE_LINES 10

#define MIN_INT_GENERATE 100
#define MAX_INT_GENERATE 999

//-----------------------------------
// ESTRUTURAS DE MEMORIA
//-----------------------------------
//Estrutura contendo os dados de uma linha da memoria principal
typedef struct {
    int content;        // conteudo da linha
    int identificator;  //Identificador da linha dentro da RAM
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

void print_ram_line(RamLine r_line){
    printf("-----------------------------------\n");
    printf("[%d] |", r_line.identificator);
    printf(" Content: %d |", r_line.content);
    printf(" BlockID: %d\n", r_line.blockID);
}
/*
void print_ram(RamLine r_lines[MAX_RAM_LINES]){
    int i;
    for(i = 0; i < MAX_RAM_LINES; i++){
        print_ram_line(r_lines[i]);
    }
}
*/
void print_full_ram(RamLine m_ram[MAX_RAM_LINES]){
    int aux = 1;
    for(int i = 0; i < MAX_RAM_LINES; i++){
        if(aux == 1){
            printf("----------------------------------------------------\n");
        }
        printf("[%d] | Conteúdo: %d | Bloco: %d\n", m_ram[i].identificator, m_ram[i].content, m_ram[i].blockID);
        aux++;
        if(aux == MEMORY_BLOCK_SIZE){
            aux = 0;
        }
    }
    printf("----------------------------------------------------\n");
}

void main(){
    RamLine m_ram[MAX_RAM_LINES];
    CacheLine m_cache[MAX_CACHE_LINES];
    
    int filled_blocks = 0;
    int filled_lines = 0;

    for(int i = 0; i < MAX_RAM_LINES; i++){
        m_ram[i].content = sort_int(100, MAX_INT_GENERATE);
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

    for(int i = 0; i < MAX_CACHE_LINES; i++){
        m_cache[i].filled = 0;
        m_cache[i].acess_count = 0;
        m_cache[i].update_count = 0;
    }

    print_full_ram(m_ram);
    //print_ram_line(m_ram[85]);
    //print_ram(m_ram);
}

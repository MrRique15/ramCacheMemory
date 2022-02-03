#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_RAM_LINES 50
#define MEMORY_BLOCK_SIZE 5
#define MEMORY_BLOCKS (MAX_RAM_LINES / MEMORY_BLOCK_SIZE)
#define MAX_CACHE_LINES 5
#define MIN_INT_GENERATE 100
#define MAX_INT_GENERATE 999
#define CLEAR_SCREEN "clear"
#define TRUE 1
#define FALSE 0

//lfu - usado com menor frequencia
//lru - atualizado com menor frequencia
//fifo - primeiro que entrou, sai primeiro, o mais velho sai primeiro

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

//função para analisar se há linhas vazias na cache, caso haja, retorna uma linha vazia aleatória
int has_empty_lines(CacheLine m_cache[MAX_CACHE_LINES]){
    int empty_lines = 0;
    for(int i = 0; i < MAX_CACHE_LINES; i++){
        if(m_cache[i].filled == 0){
            empty_lines++;
        }
    }
    if(empty_lines == 0){
        return -1;
    }else{
        int lines_to_fill[empty_lines];
        for(int i = 0, j = 0; i < MAX_CACHE_LINES; i++){
            if(m_cache[i].filled == 0){
                lines_to_fill[j] = i;
                j++;
            }
        }
        int random_line = rand() % empty_lines;
        return lines_to_fill[random_line];
    }
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

//Altera o conteúdo de uma linha da memória RAM, dentro da estrutura da LINHA
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

void debugFifo(int mapFifo[MAX_CACHE_LINES]){
    printf("\nOrdens de linhas utilizadas da CACHE: ");
    for(int a = 0; a < MAX_CACHE_LINES; a++){
        printf("%d ", mapFifo[a]);
    }
    printf("\n\n");
}

//Função para registrar um acesso na cache, quando não existem linhas vazias na cache
void fill_cache_line_two(CacheLine *m_cache, RamLine m_ram[MAX_RAM_LINES], int ram_line_number, int method, int mapFifo[MAX_CACHE_LINES]){
    int random_line = rand() % MAX_CACHE_LINES;
    int lineAux = 0, fifoMapped = 0, fifoAux = 0;
    switch (method)
    {
        case 1:
            printf("Inserindo registro em posição aleatória: %d na memória CACHE\n", random_line);

            debugFifo(mapFifo);

            for(int i = 0; i < MEMORY_BLOCK_SIZE; i++){
                lineAux = m_cache[random_line].r_lines[i].identificator;
                m_ram[lineAux] = m_cache[random_line].r_lines[i];
                m_cache[random_line].r_lines[i] = m_ram[ram_line_number];
                m_cache[random_line].acess_count = 0;
                m_cache[random_line].update_count = 0;
                printf("Realizando escrita writeBack na memória RAM para a LINHA: %d...\n", lineAux);
            }
            for(int w = 0; w < MAX_CACHE_LINES-1; w++){
                if(mapFifo[w] == random_line){
                    fifoAux = mapFifo[w];
                    mapFifo[w] = mapFifo[w+1];
                    mapFifo[w+1] = fifoAux;
                   
                }
            }
            printf("O regitro foi inserido com sucesso na LINHA: %d da memória CACHE\n", random_line);

            debugFifo(mapFifo);
        break;

        case 2:  //FIFO
            printf("Inserindo registro com método FIFO na memória CACHE\n");

            debugFifo(mapFifo);

            for(int i = 0; i < MEMORY_BLOCK_SIZE; i++){
                lineAux = m_cache[mapFifo[0]].r_lines[i].identificator;
                m_ram[lineAux] = m_cache[mapFifo[0]].r_lines[i];
                m_cache[mapFifo[0]].r_lines[i] = m_ram[ram_line_number];
                m_cache[mapFifo[0]].acess_count = 0;
                m_cache[mapFifo[0]].update_count = 0;
                printf("Realizando escrita writeBack na memória RAM para a LINHA: %d...\n", lineAux);
            }
            printf("O regitro foi inserido com sucesso na LINHA: %d da memória CACHE\n", mapFifo[0]);
            for(int w = 0; w < MAX_CACHE_LINES-1; w++){
                fifoAux = mapFifo[w];
                mapFifo[w] = mapFifo[w+1];
                mapFifo[w+1] = fifoAux;
            }

            debugFifo(mapFifo);
        break;

        case 3:  //LRU
            printf("Lru");
        break;

        default:
            printf("Erro interno ao preencher cache com o método selecionado\n");
        break;
    }
}

//Coloca um bloco da memória RAM em alguma posição da memória CACHE
void fill_cache_line(CacheLine m_cache[MAX_CACHE_LINES], RamLine m_ram[MAX_RAM_LINES], int ram_line_number, int method, int mapFifo[MAX_CACHE_LINES]){
    int found = 0, block = m_ram[ram_line_number].blockID, count = 0;

    int empty_line = has_empty_lines(m_cache);
    if(empty_line > -1){
        if(m_cache[empty_line].filled == 0){
            printf("Inserindo registro na LINHA vazia: %d da memória CACHE\n", empty_line);
            for(int j = 0; j < MAX_RAM_LINES; j++){
                if(count < MEMORY_BLOCK_SIZE && m_ram[j].blockID == m_ram[ram_line_number].blockID){
                    m_cache[empty_line].r_lines[count].content = m_ram[j].content;
                    m_cache[empty_line].r_lines[count].identificator = m_ram[j].identificator;
                    m_cache[empty_line].r_lines[count].blockID = m_ram[j].blockID;
                    count++;
                }
            }
            for(int w = 0, stop = 0; w < MAX_CACHE_LINES; w++){
                if (mapFifo[w] == -1 && stop == 0){
                    mapFifo[w] = empty_line;
                    stop = 1;
                }
            }
            m_cache[empty_line].filled = 1;
            printf("O registro foi inserido com sucesso na LINHA: %d da memória CACHE\n", empty_line);
            found = 1;
        }else{
            printf("Erro interno ao encontrar linha vazia na memória cache\n");
        }
    }else{
        printf("Não existem linhas vazias na memória CACHE\n");
        fill_cache_line_two(m_cache, m_ram, ram_line_number, method, mapFifo);
        //Implementar código para acessar posições já preenchidar e devolver os valores à RAM
    }
}

//Acessar registro da RAM e armazená-lo na memória CACHE
void acess_ram_register(CacheLine m_cache[MAX_CACHE_LINES], RamLine m_ram[MAX_RAM_LINES], int method, int mapFifo[MAX_RAM_LINES]){
    int resp = 0, found = 0, resptwo = 0, respSelect = 0;

    printf("Insira o número da linha que deseja acessar: \n");
    scanf("%d", &resp);
    for(int i = 0; i < MAX_CACHE_LINES; i++){
        if(m_cache[i].filled == 1){
            for(int j = 0; j < MEMORY_BLOCK_SIZE; j++){
                if(m_cache[i].r_lines[j].identificator == resp){
                    m_cache[i].acess_count++;
                    found = 1;
                    printf("O conteúdo da linha %d já está registrado na LINHA: %d, posição %d da memória CACHE\n", resp, i, j);
                    print_full_cache(m_cache);
                    printf("Deseja alterar esse registro? (1 - Sim / 0 - Não)\n");
                    scanf("%d", &respSelect);
                    if(respSelect == 1){
                        printf("Insira o novo conteúdo para a linha %d: \n", resp);
                        scanf("%d", &resptwo);
                        change_line_content(&m_cache[i].r_lines[j], resptwo);
                        m_cache[i].update_count++;
                        printf("Registro alterado com sucesso!\n");
                        print_full_cache(m_cache);
                    }else{
                        printf("O conteúdo da linha %d foi mantido\n", resp);
                    }
                }
            }
        }
    }
    if(found == 0){
        printf("O conteúdo da linha %d não está registrado na memória CACHE\n", resp);
        fill_cache_line(m_cache, m_ram, resp, method, mapFifo);
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
    printf("\n\nOpções Disponíveis:\n");
    printf("0 - Encerrar\n");
    printf("1 - Mostrar toda a memória RAM\n");
    printf("2 - Mostrar toda a memória Cache\n");
    printf("3 - Selecionar método de alteração da CACHE (Padrao: ALEATORIO)\n");
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
    printf("2 - FIFO\n");
    printf("3 - LRU\n");
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
    int mapFifo[MAX_CACHE_LINES];
    for(int i = 0; i < MAX_CACHE_LINES; i++){
        mapFifo[i] = -1;
    }

    srand(time(0));
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
                acess_ram_register(m_cache, m_ram, metodo, mapFifo);
            break;

            default:
                invalid_option();
            break;
        }
    } while(resp != 0);
}
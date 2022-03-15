#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_RAM_LINES 4096
#define MEMORY_BLOCK_SIZE 4
#define MEMORY_BLOCKS (MAX_RAM_LINES / MEMORY_BLOCK_SIZE)
#define MAX_CACHE_LINES 4
#define MIN_INT_GENERATE 100
#define MAX_INT_GENERATE 999
#define CLEAR_SCREEN "clear"

//Some useful information can be shown on screen while true, but final user don´t need to see it.
// 1 = debug mode on, 0 = debug mode off
#define DEBUG 0
/*
--------------------------------------------------------------
-- DATA STRUCTURES TO CREATE VIRTUAL RAM AND CACHE MEMORIES --
--------------------------------------------------------------
*/
//Struct to create a virtual ram line, with content, line number and the block that line is in.
typedef struct {
    int content;          //Line Content
    int identificator;    //Line Number
    int blockID;          //RAM Block ID
} RamLine;

/*
Struct to create a virtual cache line, with the filled or not controller, acces count, update cound and
the content of the cache line being an array of ram lines.
*/
typedef struct {
    int filled;                        //0 - empty, 1 - filled
    int acess_count;                   
    int update_count;                  
    RamLine r_lines[MEMORY_BLOCK_SIZE];//RAM Lines array.
} CacheLine;

/*
------------------------------------------------------------------------------------------------------
-- CORE FUNCTIONS, TO CONTROL THE RAM AND CACHE MEMORIES, ORGANIZE THE DATA AND MAKE THE OPERATIONS --
------------------------------------------------------------------------------------------------------
*/
//Function to print a full RAM line, with line number, content and block ID.
void print_ram_line(RamLine r_line){
    printf("---------------------------------\n");
    printf("[%d] |", r_line.identificator);
    printf(" Content: %d |", r_line.content);
    printf(" BlockID: %d\n", r_line.blockID);
}
//Function to print the full virtual RAM memory.
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
//Function to print the full virtual CACHE memory.
void print_full_cache(CacheLine m_cache[MAX_CACHE_LINES]){
    printf("----------------------------------------------------\n");
    for(int i = 0; i < MAX_CACHE_LINES; i++){
        printf("[%d]", i);
        if(m_cache[i].filled == 1){
            for(int j = 0; j < MEMORY_BLOCK_SIZE; j++){
                printf(" | %d", m_cache[i].r_lines[j].content);
            }
        }
        printf("\n");
        printf("----------------------------------------------------\n");
    }  
}
//Function to show the FIFO map of used CACHE lines, in order.
void debugFifo(int mapFifo[MAX_CACHE_LINES]){
    printf("Mapeamento FIFO de linhas utilizadas da CACHE: ");
    for(int a = 0; a < MAX_CACHE_LINES; a++){
        printf("%d ", mapFifo[a]);
    }
    printf("\n\n");
}
//Function to generate a random integer number, from MIN_INT_GENERATE to MAX_INT_GENERATE
int sort_int(int min, int max){
    int num;
    num = rand() % (max - min + 1) + min;
    return num;
}
//Function to change the content into a line that has RamLine type.
void change_line_content(RamLine *r_line, int content){
    r_line->content = content;
}
/*
Function to analize the content of the cache line, to see if it is filled or not, if there is any empty line,
the function will return an aleatory line to be filled.
*/
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
//Function to insert some registers in the cache line, if there is any empty line.
void fill_cache_line_two(CacheLine *m_cache, RamLine m_ram[MAX_RAM_LINES], int ram_line_number, int method, int mapFifo[MAX_CACHE_LINES]){
    int random_line = rand() % MAX_CACHE_LINES;
    int lineAux = 0, fifoMapped = 0, fifoAux = 0;
    switch (method)
    {
        case 1:
            printf("Inserindo registro em posição ALEATORIA: %d na memória CACHE\n", random_line);

            if(DEBUG){
                debugFifo(mapFifo);
            }

            for(int i = 0; i < MEMORY_BLOCK_SIZE; i++){
                lineAux = m_cache[random_line].r_lines[i].identificator;
                m_ram[lineAux] = m_cache[random_line].r_lines[i];
                m_cache[random_line].r_lines[i] = m_ram[ram_line_number];
                m_cache[random_line].acess_count = 0;
                m_cache[random_line].update_count = 0;
                if(DEBUG){
                    printf("Realizando escrita writeBack na memória RAM para a LINHA: %d...\n", lineAux);
                }
            }

            for(int w = 0; w < MAX_CACHE_LINES-1; w++){
                if(mapFifo[w] == random_line){
                    fifoAux = mapFifo[w];
                    mapFifo[w] = mapFifo[w+1];
                    mapFifo[w+1] = fifoAux;
                   
                }
            }

            printf("O regitro foi inserido com sucesso na LINHA: %d da memória CACHE\n", random_line);

            if(DEBUG){
                debugFifo(mapFifo);
            }
        break;

        case 2:
            printf("Inserindo registro com método FIFO na memória CACHE\n");

            if(DEBUG){
                debugFifo(mapFifo);
            }

            for(int i = 0; i < MEMORY_BLOCK_SIZE; i++){
                lineAux = m_cache[mapFifo[0]].r_lines[i].identificator;
                m_ram[lineAux] = m_cache[mapFifo[0]].r_lines[i];
                m_cache[mapFifo[0]].r_lines[i] = m_ram[ram_line_number];
                m_cache[mapFifo[0]].acess_count = 0;
                m_cache[mapFifo[0]].update_count = 0;
                if(DEBUG){
                    printf("Realizando escrita writeBack na memória RAM para a LINHA: %d...\n", lineAux);
                }
            }

            printf("O regitro foi inserido com sucesso na LINHA: %d da memória CACHE\n", mapFifo[0]);
            
            for(int w = 0; w < MAX_CACHE_LINES-1; w++){
                fifoAux = mapFifo[w];
                mapFifo[w] = mapFifo[w+1];
                mapFifo[w+1] = fifoAux;
            }

            if(DEBUG){
                debugFifo(mapFifo);
            }
        break;

        default:
            printf("Erro interno ao preencher cache com o método selecionado\n");
        break;
    }
}
/*
Insert some RAM memory block into one cache line, analizing the content of the cache line, 
to see if it is filled or not.
*/
void fill_cache_line(CacheLine m_cache[MAX_CACHE_LINES], RamLine m_ram[MAX_RAM_LINES], int ram_line_number, int method, int mapFifo[MAX_CACHE_LINES]){
    int found = 0, block = m_ram[ram_line_number].blockID, count = 0;

    int empty_line = has_empty_lines(m_cache);
    if(empty_line > -1){
        if(m_cache[empty_line].filled == 0){
            printf("Inserindo registro na linha VAZIA: %d da memória CACHE\n", empty_line);
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
        if(DEBUG){
            printf("Não existem linhas vazias na memória CACHE\n");
        }
        fill_cache_line_two(m_cache, m_ram, ram_line_number, method, mapFifo);
    }
}
/*
Function to try acessing some RAM line content, if the line is already registered into the cache, the function will show
the full cache memory, and ask for modifications on that line content, but this alteration will not be made in the RAM
memory, it will be done in the cache memory and then will be written back to the RAM memory when it is needed.
*/
void acess_ram_register(CacheLine m_cache[MAX_CACHE_LINES], RamLine m_ram[MAX_RAM_LINES], int method, int mapFifo[MAX_RAM_LINES]){
    int resp = 0, found = 0, resptwo = 0, respSelect = 0;

    printf("Insira o número da linha que deseja acessar: \n");
    scanf("%d", &resp);
    if(resp > MAX_RAM_LINES-1 || resp < 0){
        printf("Linha indisponível na memória RAM\n");
    }else{
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
    }
    if(found == 0){
        if(DEBUG){
            printf("Não existe registro na memória CACHE para a linha %d\n", resp);
        }
        fill_cache_line(m_cache, m_ram, resp, method, mapFifo);
    }
    
}
//Function to initialize the RAM memory, full of random numbers in it lines.
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

//Function to initialize the virtual CACHE, full of empty lines.
void initialize_cache(CacheLine m_cache[MAX_CACHE_LINES]){
    for(int i = 0; i < MAX_CACHE_LINES; i++){
        m_cache[i].filled = 0;
        m_cache[i].acess_count = 0;
        m_cache[i].update_count = 0;
    }
}
/*
------------------------------------------------------
-- FUNCTIONS TO SHOW MESSAGES AND MENUS TO THE USER --
------------------------------------------------------
*/
void infos_start(){
    printf("----------------------------------------------------\n");
    printf("\tSimulador de Memória Cache\n");
    printf("----------------------------------------------------\n");
}
int main_menu(){
    int option;
    printf("\nOpções Disponíveis:\n");
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
    scanf("%d", option);
    if (*option < 1 || *option > 3){
        invalid_option();
        select_method(option);
    }else{
        printf("----------------------------------------------------\n");
    }
}
/*
--------------------------------------
-- MAIN FUNCTION TO RUN THE PROGRAM --
--------------------------------------
*/
void main(){
    RamLine m_ram[MAX_RAM_LINES];
    CacheLine m_cache[MAX_CACHE_LINES];
    int resp = 1, metodo = 1;
    int mapFifo[MAX_CACHE_LINES];
    for(int i = 0; i < MAX_CACHE_LINES; i++){
        mapFifo[i] = -1;
    }

    int timeTest = time(0);

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
    
    if(DEBUG){
        printf("Tempo total de simulação: %.2f segundos\n", (float)(time(0) - timeTest));
    }
}
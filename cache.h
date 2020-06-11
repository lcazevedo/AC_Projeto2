#include <iostream>
#include <stdlib.h>
#include <string>
#include <bits/stdc++.h> 
#include <stdio.h>

using namespace std;

// Cache do processador Intel(R) Core(TM) i5-3330 CPU @ 3.00GHz
// Cache Dados: 4 x 32 KB (8-way, 64 bytes line size)
// Cache Instruções: 4 x 32 KB (8-way, 64 bytes line size)
#define TAMANHO_BYTES 32768
#define NUMERO_LINHAS 512 
#define N_VIAS 8 

#define POLITICA_DE_WRITE "writethrough" // pode ser "writeback" ou "writethrough"
#define ALGORITMO_SUBSTITUCAO_LINHAS 1 // opções válidas: 1 - LRU, 2 - MRU, 3 - FIFO e 4 - ALEATORIO   

// definição da classe
class cache {

    private:
        
        // propriedades privadas   
        struct myData {
            unsigned char data[4];
        };
        struct myLinhaCache {
            bitset<1> valida;
            bitset<1> dirty;
            unsigned int contador; // para LRU
            bitset<62> tag;  // rever este tamnho!!!!
            myData data[];
            
        };    

        myLinhaCache* linhaCache; // o tamanho do array será declarado no construtor da classe

        cache* cache_proximo_nivel; // ponteiro para o pŕoxio nível de cache, caso exista

        int tamanho_bytes; // tamanho do cache em bytes
        int quantidade_linhas; // quantidade de linhas que o cache deve ter
        int tamanho_linhas; // tamanho de cada linha em bytes
        int quantidade_vias;
        int quantidade_sets;  // quantidade de sets de linhas de acomrdo com a quantidade de vias
        string politica_de_write;
        int algoritmo_substituicao_linhas;
        
        int fator_alinhamento_endereco;

        int bits_byteOffset;
        int bits_index;
        int maxval_byteOffset;

        int misses;
        int hits;
        int leituras;
        int gravacoes;

        // métodos privados
        int linha_endereco(int endereco);
        void grava_linha_no_cache(int set, int64_t tag);
        void grava_linha_memoria(int linha);
        int verifica_set(int set, int64_t tag);
        void ajusta_contador_LRU_MRU(int linha, int set);
        void inicializa_com_parametros(int tamanho_bytes, int quantidade_linhas, int quantidade_vias, string politica_de_write, 
                int algoritmo_substituicao_linhas);

    public: 
        // construtores
        cache();
        cache(int tamanho_bytes, int quantidade_linhas, int quantidade_vias, string politica_de_write, 
                int algoritmo_substituicao_linhas);
        
        
        // métodos públicos
        void ler(int byte_offset, int index, int64_t tag, int tamanho);
        void gravar(int byte_offset, int index, int64_t tag, int tamanho);
       

        // expondo propriedades 
        void setCacheProximoNivel(cache*);
        cache* getCacheProximoNivel();

        int getMisses();
        int getHits();
        int getLeituras();
        int getGravacoes();
                
        
};


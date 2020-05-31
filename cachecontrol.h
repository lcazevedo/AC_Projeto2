#include <iostream>
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include "cache.h"

using namespace std;


#define SPLIT_CACHE 1  // Se Ture, tem cache de dados e cache de instruções 
#define NIVEIS_DE_CACHE 2 //define a quantidade de níveis de cache que serão criados (L1, L2, L3, etc.)

#define CHKBIT(n, p) ((n >> p) & 1U)

// definição da classe
class cachecontrol {

    private:
        
        // propriedades privadas
        cache* cacheDados;
        cache* cacheInstrucoes; 

        bool split_cache;
        int niveis_de_cache;

        int bits_byteOffset;
        int bits_index;
        int maxval_byteOffset;

        // métodos privados
        string* split_comando(string comando);
        string* split_config(string config);
        int64_t* split_endereco(int64_t endereco);
        int64_t endereco_decimal(string endereco);
        
    public: 
        // construtores
        cachecontrol();
        cachecontrol(string config_file);
        
        
        // métodos públicos
        string processaComando(string comando);

        // expondo propriedades 
        int getMissesInstrucoes(int nivel);
        int getHitsInstrucoes(int nivel);
        int getLeiturasInstrucoes(int nivel);
        int getGravacoesInstrucoes(int nivel);

        int getMissesDados(int nivel);
        int getHitsDados(int nivel);
        int getLeiturasDados(int nivel);
        int getGravacoesDados(int nivel);


};


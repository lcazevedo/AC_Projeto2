#include "cache.h"


/************
Grava na memória o valor no local indicado
Entrada:
    int: linha do cache
    int: tamanho em bytes a ser lido
    int: byte offset do início da leitura
    int: tag para identificação do endereço correto
Saída:
    nenhum: somente simulação
************/
void cache::gravar(int byte_offset, int index, int64_t tag, int tamanho){
    int bytes_gravados = 0;
    int set, linha;

    // laço para ler todos os bytes
    while (bytes_gravados < tamanho) {
        // identifica linha do cache onde deve gravar o valor
        set = index % quantidade_sets;

        // verifica se alguma das linhas do set é válida e tem o endereço desejado
        linha = verifica_set(set, tag);

        // se a linha que está no cache é inváida ou não é a que precisa gravar,  
        // marca miss e busca
        if (linha < 0){
            busca_linha_memoria(set, tag); // essa função já incrementa o miss e grava, se for dirty com whrite back
        } else {
            hits ++;

            // ajusta o contador se for LRU ou MRU
            // contador em 1 na linha utilizada (mais recentemente usado) e incremente em 1 o contador das outras
            if (algoritmo_substituicao_linhas == 1 || algoritmo_substituicao_linhas == 2) {
                ajusta_contador_LRU_MRU(linha, set);
            }
        }

        // age conforme política
        if (politica_de_write == "writethrough") {
            grava_linha_memoria(linha);
        } else {  // é whriteback
            linhaCache[linha].dirty = 1;
        }

        bytes_gravados += (tamanho_linhas - byte_offset);
        
        // ajusta o index e byteOffset para poder gravar os bytes restantes, se necessário
        index++;
        byte_offset = 0;

        gravacoes++;
    }  
}

/************
Lê no cache, ou busca na memória, o valor no local indicado
Entrada:
    int: linha do cache
    int: tamanho em bytes a ser lido
    int: byte offset do início da leitura
    int: tag para identificação do endereço correto
Saída:
    nenhum: somente simulação
************/
void cache::ler(int byte_offset, int index, int64_t tag, int tamanho){
    
    int bytes_lidos = 0;
    int set, linha;

    // laço para ler todos os bytes
    while (bytes_lidos < tamanho) {
        // identifica linha do cache onde deve estar o valor
        //linha = linha_endereco(endereco);
        set = index % quantidade_sets;

        // verifica se alguma das linhas do set é válida e tem o endereço desejado
        linha = verifica_set(set, tag);

        // se a linha não é válida ou o endereço buscado não está na linha, é um Miss e tem que buscar na memória
        if (linha < 0) {
            busca_linha_memoria(set, tag);
        } else {  // senão, é hit
            hits ++;

            // ajusta o contador se for LRU ou MRU
            // contador em 1 na linha utilizada (mais recentemente usado) e incremente em 1 o contador das outras
            if (algoritmo_substituicao_linhas == 1 || algoritmo_substituicao_linhas == 2) {
                ajusta_contador_LRU_MRU(linha, set);
            }
        }
        
        // incrementa os bytes que estão na linha lida
        bytes_lidos += (maxval_byteOffset - byte_offset + 1);

        // ajusta o index e byteOffset para poder buscar os bytes restantes, se necessário
        index++;
        byte_offset = 0;

        leituras++;

    }
}


/************
verifica se alguma das linhas do set é válida e tem o valor desejado
Entrada:
    set: set a ser verificado
    tag: tag para comparação se é a linha desejada
Saída:
    int: número da linah válida ou -1, se não encontrou
************/
int cache::verifica_set(int set, int64_t tag) {
    int linha = -1;

    for (int i = (set * quantidade_vias); i < ((set * quantidade_vias) + quantidade_vias); i++){
        if (linhaCache[i].valida == 1 && linhaCache[i].tag == tag){
            linha = i;
            break;
        }
    }
    
    return linha;
};



/************
Simula a busca de um endereço de memória e gravar no cache, ajustando as propriedades necessárias
Entrada:
    int: linha a ser lida no cache
    int: nova tag para a linha
Saída:
    nenhuma, é só simulação
************/
void cache::busca_linha_memoria(int set, int64_t tag) {
    int linha, linha_ini_set, linha_fim_set;
    unsigned int maior_valor, menor_valor;

    misses ++;

    // calcula as linhas inicial e final do set
    linha_ini_set = set * quantidade_vias;
    linha_fim_set = linha_ini_set + quantidade_vias - 1;

    // indentifica uma linha do set para gravar
    switch (algoritmo_substituicao_linhas)
    {
    case 1:  // LRU
        // identifica uma linha inválida ou a linha com maior contador (menos recentemente usado)
        maior_valor = 0;
        linha = set * quantidade_vias;
        for (int i = linha_ini_set; i <= linha_fim_set; i++){
            if (linhaCache[i].valida == 0) {
                linha = i;
                break;
            }
            else {
                if (linhaCache[i].contador > maior_valor) {
                    maior_valor = linhaCache[i].contador;
                    linha = i;
                }
            }            
        }
        // marca contador em 1 (mais recentemente usado) e incrementa os outros
        ajusta_contador_LRU_MRU(linha, set);
        break;
    
    case 2:  // MRU
        // identifica uma linha inválida ou a linha com menor contador (mais recentemente usado)
        menor_valor = UINT_MAX;
        linha = set * quantidade_vias;
        for (int i = linha_ini_set; i <= linha_fim_set; i++){
            if (linhaCache[i].valida == 0) {
                linha = i;
                break;
            }
            else {
                if (linhaCache[i].contador < menor_valor) {
                    menor_valor = linhaCache[i].contador;
                    linha = i;
                }
            }            
        }
        // marca contador em 1 (mais recentemente usado) e incrementa os outros
        ajusta_contador_LRU_MRU(linha, set);
        break;
    
    case 3:  // FIFO
        // identifica uma linha inválida ou a linha com maior contador (menos recentemente usado)
        maior_valor = 0;
        linha = set * quantidade_vias;
        for (int i = linha_ini_set; i <= linha_fim_set; i++){
            if (linhaCache[i].valida == 0) {
                linha = i;
                break;
            }
            else {
                if (linhaCache[i].contador > maior_valor) {
                    maior_valor = linhaCache[i].contador;
                    linha = i;
                }
            }            
        }
        // marca contador em 1 (mais recentemente usado) e incrementa os outros
        // Esta função serve para FIFO e deve ser cahamada apenas aqui, onde busca a linha.
        ajusta_contador_LRU_MRU(linha, set);
        break;
    
    case 4:  // Randômico
        // identifica uma linha inválida ou a linha com maior contador (menos recentemente usado)
        linha = rand() % quantidade_vias + 1;
        break;
    
    default:
        // procura por uma inválida ou pega a primeira
        linha = set * quantidade_vias;
        for (int i = linha_ini_set; i <= linha_fim_set; i++){
            if (linhaCache[i].valida == 0) {
                linha = i;
                break;
            }
        }

        break;
    }
    
    // grava linha se for suja, proveniente de write back
    if (linhaCache[linha].dirty == 1){
        grava_linha_memoria(linha);
    }

    // ajusta a tag e a marca de válida, para simular a leitura
    linhaCache[linha].valida = 1;
    linhaCache[linha].tag = tag;
};

/************
Ajusta contador em 1 na linha selecionada e incremente as outras
Entrada:
    int linha: linha para ajustar contador
    int set: set para incrementar outras linhas (pode ser calculado a partir da linha, mas recber evita cácluo)
Saída:
    não há
************/
void cache::ajusta_contador_LRU_MRU(int linha, int set){
    linhaCache[linha].contador = 1;
    for (int i = (set * quantidade_vias); i < ((set * quantidade_vias) + quantidade_vias); i++){
        if (i != linha && linhaCache[i].valida == 1 && linhaCache[i].contador < UINT_MAX) {
            linhaCache[i].contador++;
        }
    }
}


/************
Simula a gravação de uma linha de cache na memória
Entrada:
    int: número da linha que será gravada, mas se fosse gravar realmente, precisaria do byteoffset e do index para encontrar o endereço.
Saída:
    nenhuma, é só simulação
************/
void cache::grava_linha_memoria(int linha) {
  
    // grava linha se for suja, proveniente de write back
    linhaCache[linha].dirty == 0;
    
};


/************
Construtur da classe a partir de parâmetros pré definidos na classe:
Entrada:
    nenhuma
Saída:
    nenuma
************/
cache::cache(){

    inicializa_com_parametros(TAMANHO_BYTES, NUMERO_LINHAS, N_VIAS, POLITICA_DE_WRITE, ALGORITMO_SUBSTITUCAO_LINHAS);

};


/************
Construtur da classe a partir de uma lista de parâmetros:
Entrada:
    int tamanho_bytes: quantidade de bytes total do cache, exceto bytes de controle
    int quantidade_linhas: quantidade de linhas do cache
    int quantidade_vias: quantidade de vias, podendo variar de 1 (não associativa, até quantidade de linhas (totalmente associativa)
    string politica_de_write: pode ser "writeback" ou "writethrough" 
    int algoritmo_substituicao_linhas: pode ser:  1 - LRU, 2 - MRU, 3 - FIFO e 4 - ALEATORIO
Saída:
    nenuma
************/
cache::cache(int tamanho_bytes, int quantidade_linhas, int quantidade_vias, string politica_de_write, 
                int algoritmo_substituicao_linhas){

        inicializa_com_parametros(tamanho_bytes, quantidade_linhas, quantidade_vias, 
                                politica_de_write, algoritmo_substituicao_linhas);
}

/************
inicializa os parametros da classe
Entrada:
    int tamanho_bytes: quantidade de bytes total do cache, exceto bytes de controle
    int quantidade_linhas: quantidade de linhas do cache
    int quantidade_vias: quantidade de vias, podendo variar de 1 (não associativa, até quantidade de linhas (totalmente associativa)
    string politica_de_write: pode ser "writeback" ou "writethrough" 
    int algoritmo_substituicao_linhas: pode ser:  1 - LRU, 2 - MRU, 3 - FIFO e 4 - ALEATORIO
Saída:
    nenuma
************/
void cache::inicializa_com_parametros(int tamanho_bytes, int quantidade_linhas, int quantidade_vias, string politica_de_write, 
                int algoritmo_substituicao_linhas){

    // incialização padrão dos parâmetros
    this->tamanho_bytes = tamanho_bytes;  // tamanho do cache em bytes
    this->quantidade_linhas = quantidade_linhas;  // quantidade de linhas que o cache deve ter
    this->tamanho_linhas = tamanho_bytes / quantidade_linhas;  // tamanho de cada linha em bytes
    this->quantidade_vias = quantidade_vias;
    if (quantidade_vias < 1 || quantidade_vias > quantidade_linhas)
        throw runtime_error("Quantidade de vias inválida: " + to_string(quantidade_vias) + 
            " A quantidade de vias deve ser maior que 0 e menor ou igual a quantidade de linhas." + 
            "(quantidade de linhas: " + to_string(quantidade_linhas) + ")");
    quantidade_sets = quantidade_linhas / quantidade_vias;
    if ((quantidade_linhas % quantidade_vias) != 0)
        throw runtime_error("Quantidade de vias inválida: " + to_string(quantidade_vias) + 
            " A quantidade de linhas deve ser divisível pela quantidade de vias para formação da quantidade de sets."+ 
            "(quantidade de linhas: " + to_string(quantidade_linhas) + ")");
    this->politica_de_write = politica_de_write;
    this->algoritmo_substituicao_linhas = algoritmo_substituicao_linhas;

    // se o algoritmo de substituição é randomico, inicia o 
    // gerador e numeros randomicos
    if (algoritmo_substituicao_linhas == 4)
        srand( (unsigned)time(NULL) );

    bits_byteOffset = log2(tamanho_linhas);
    bits_index = log2(quantidade_sets);
    maxval_byteOffset = 0;
    for (int i = 0; i < bits_byteOffset; i++) {
         maxval_byteOffset = (maxval_byteOffset << 1) | 1;
    }
    
    // cria o cache
    linhaCache = new myLinhaCache[quantidade_linhas];

    // invalida todas as linas do cache e deixa os blocos como limpos e contador de LRU em zero
    for (int i = 0; i < quantidade_linhas; i++) {
        linhaCache[i].valida = linhaCache[i].dirty = linhaCache[i].contador = 0;
    }

    // inicializa os contadores
    hits = misses = leituras = gravacoes = 0;

};

// getters e setters

void cache::setCacheProximoNivel(cache* cache){
    cache_proximo_nivel = cache;
}

cache* cache::getCacheProximoNivel(){
    return cache_proximo_nivel;
}


int cache::getMisses(void) {
    return misses;
};

int cache::getHits(void) {
    return hits;
};

int cache::getLeituras(void) {
    return leituras;
};

int cache::getGravacoes(void) {
    return gravacoes;
};


        

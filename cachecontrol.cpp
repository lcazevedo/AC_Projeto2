#include "cachecontrol.h"
#include "tinyxml2.h"

/************
Recebe uma linha de comando do processador e processa conforme operação da linha, retornando o valor solicitado
Entrada:
    string: linha de comando no formato "operação,endereço,tamanho"
         operação: I=instrução, S=Store, L=Load, M=Memory(acesso direto)
         endereço: endereço de 64 bits
         tamanho: tamanho em bytes do acesso
Saída:
    string: valor contido no endereço de memória solcitado
************/
string cachecontrol::processaComando(string comando) {

    string* s_comando;
    string operacao, retorno;
    int tamanho, linha;
    int64_t endereco;  // inteiro com 64 bits, tamanho do endereço previsto
    int byte_offset, index;
    int64_t tag; // na pior hipótese, a tag pode ter até 62bits para os endereços de 64bits 
    int64_t* quebra;
    
    // split do comando
    s_comando = split_comando(comando);
    operacao = s_comando[0];
    endereco = endereco_decimal(s_comando[1]);
    tamanho = atoi(s_comando[2].c_str());

    // quebra o endereço em byte_offset, index e tag
    quebra = split_endereco(endereco);
    byte_offset = quebra[0];
    index = quebra[1];
    tag = quebra[2];
    
    // processa de acordo com a operação
    switch (operacao[0]) {
        case 'I':
            cacheInstrucoes->ler(byte_offset, index, tag, tamanho);  
            break;

        case 'S':
            cacheDados->gravar(byte_offset, index, tag, tamanho); 
            break;

        case 'L':
            cacheDados->ler(byte_offset, index, tag, tamanho); 
            break;

        case 'M':   // recomendado pelo professro para fazer igual ao Store.
            cacheDados->gravar(byte_offset, index, tag, tamanho); 
            break;
        
        default:
            throw runtime_error("Operação '" + operacao + "' inválida em '" + comando + "'");
    }

    return retorno;

};


/************
Recebe um endereço e identifica suas partes  byte_offset, index e tag
Entrada:
    string: endereço 
Saída:
    int[]: array com 3 inteiros: byte_offset, index e tag
************/
int64_t* cachecontrol::split_endereco(int64_t endereco){
    int64_t* split = new int64_t[3];
    
    split[0] = 0; // byte_Ofsset
    int pos = 0;
    for (int i = 0; i < bits_byteOffset; i++) {
        if (CHKBIT(endereco, i)) {
            split[0] += pow(2, pos);
        }
        pos++;
    }

    split[1] = 0; // index
    pos = 0;
    for (int i = bits_byteOffset; i < bits_index + bits_byteOffset; i++) {
        if (CHKBIT(endereco, i)) {
            split[1] += pow(2, pos);
        }
        pos++;
    }

    split[2] = 0; // tag
    pos = 0;
    for (int i = bits_index + bits_byteOffset; i < 64; i++) {
        if (CHKBIT(endereco, i)) {
            split[2] += pow(2, pos);
        }
        pos++;
    }

    return split;
}


/************
Recebe uma linha de comando e faz o split em suas 3 partes
Entrada:
    string: linha de comando no formato "operação,endereço,tamanho"
Saída:
    string*: array de 3 posições com as 3 partes do comando
         operação: I=instrução, S=Store, L=Load, M=Memory(acesso direto)
         endereço: endereço de 64 bits
         tamanho: tamanho em bytes do acesso
************/
string* cachecontrol::split_comando(string comando) {
    
    string* split = new string[3];
    string erro = "";

    // split do comando
    stringstream s_comando(comando);
    if (!getline( s_comando, split[0], ',' )) {
        erro = "Falha na leitura da operacao em " + comando;
    };    
    if (!getline( s_comando, split[1], ',' )) {
        erro = "Falha na leitura do endereço em " + comando;
    };    
    if (!getline( s_comando, split[2], ',' )) {
        erro = "Falha na leitura do tamanho da operacao em " + comando;
    };    
    if (erro != "") {
        throw runtime_error(erro);
    }

    //cout << split[0] << "/" << split[1] << "/" << split[2] << endl;

    return split;
};

/************
Recebe uma sequência de valores de configuração separados por vírgua e retorna um array com as partes
IMPORTANTE: deve ter a mesma quantidade de dados declarado em níveis_de_cache
Entrada:
    string: linha de dados de configuração separados por vírgula
Saída:
    string*: array com os valores separados
************/
string* cachecontrol::split_config(string config) {
    
    string* split = new string[niveis_de_cache];
    int contador = 0;

    // split do comando
    stringstream s_config(config);
    while (getline( s_config, split[contador], ',' )) {
        contador++;
    };    
    
    return split;
};

/************
Converte uma string contendo um endereço em hexadecimal para um inteiro decimal
Entrada:
    string: endereço hexadecimal
Saída:
    int: valor em decimal do endereço
************/
int64_t cachecontrol::endereco_decimal(string endereco) {
    int64_t retorno;
    stringstream stream;

    stream << endereco;
    stream >> hex >> retorno;
    return retorno;
};

/************
Em caso de cache único (split_cahce = True), as contagens dos caches de instruções e dados serão iguais.
************/
int cachecontrol::getMissesInstrucoes(int nivel) {
    return cacheInstrucoes[nivel].getMisses();
};

/************
Em caso de cache único (split_cahce = True), as contagens dos caches de instruções e dados serão iguais.
************/
int cachecontrol::getHitsInstrucoes(int nivel) {
    return cacheInstrucoes[nivel].getHits();
};

/************
Em caso de cache único (split_cahce = True), as contagens dos caches de instruções e dados serão iguais.
************/
int cachecontrol::getLeiturasInstrucoes(int nivel) {
    return cacheInstrucoes[nivel].getLeituras();
};

/************
Em caso de cache único (split_cahce = True), as contagens dos caches de instruções e dados serão iguais.
************/
int cachecontrol::getGravacoesInstrucoes(int nivel) {
    return cacheInstrucoes[nivel].getGravacoes();
};

/************
Em caso de cache único (split_cahce = True), as contagens dos caches de instruções e dados serão iguais.
************/
int cachecontrol::getMissesDados(int nivel) {
    return cacheDados[nivel].getMisses();
};

/************
Em caso de cache único (split_cahce = True), as contagens dos caches de instruções e dados serão iguais.
************/
int cachecontrol::getHitsDados(int nivel) {
    return cacheDados[nivel].getHits();
};

/************
Em caso de cache único (split_cahce = True), as contagens dos caches de instruções e dados serão iguais.
************/
int cachecontrol::getLeiturasDados(int nivel) {
    return cacheDados[nivel].getLeituras();
};

/************
Em caso de cache único (split_cahce = True), as contagens dos caches de instruções e dados serão iguais.
************/
int cachecontrol::getGravacoesDados(int nivel) {
    return cacheDados[nivel].getGravacoes();
};



/************
Construtur da classe a partir de parâmetros pré definidos na classe:
Entrada:
    nenhuma
Saída:
    nenuma
************/
cachecontrol::cachecontrol(){

    this->split_cache = SPLIT_CACHE;

    if (split_cache) {
        // inicialização dos caches de dados e instruções
        cacheDados = new cache();
        cacheInstrucoes = new cache();
    } else {
        cacheDados = new cache();
        cacheInstrucoes = cacheDados;
    }

};


/************
Construtur da classe a partir de arquivo de configuração:
Entrada:
    string: caminho e nome do arquivo de configuração
Saída:
    nenuma
************/
cachecontrol::cachecontrol(string json_config_file){
    tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
	clock_t startTime = clock();
	if (doc->LoadFile( json_config_file.c_str() ) != 0){
        throw runtime_error("erro elndo arquivo de configuração");
    }

    // configurações do cachecontrol
    int niveis_de_cache = atoi(doc->FirstChildElement( "conf" )->FirstChildElement( "niveis_de_cache" )->GetText());
    this->niveis_de_cache = niveis_de_cache;    
    
    int split_cache = atoi(doc->FirstChildElement( "conf" )->FirstChildElement( "split_cache" )->GetText());
    this->split_cache = split_cache;

    // configurações dos caches de dados e intruções
    string* tbcd = split_config(doc->FirstChildElement( "conf" )->FirstChildElement( "tamanho_bytes_cahce_dados" )->GetText());
    string* qlcd = split_config(doc->FirstChildElement( "conf" )->FirstChildElement( "quantidade_linhas_cahce_dados" )->GetText());
    string* qvcd = split_config(doc->FirstChildElement( "conf" )->FirstChildElement( "quantidade_vias_cahce_dados" )->GetText());
    string* pwcd = split_config(doc->FirstChildElement( "conf" )->FirstChildElement( "politica_de_write_cahce_dados" )->GetText());
    string* aslcd = split_config(doc->FirstChildElement( "conf" )->FirstChildElement( "algoritmo_substituicao_linhas_cache_dados" )->GetText());

    string* tbci = split_config(doc->FirstChildElement( "conf" )->FirstChildElement( "tamanho_bytes_cahce_instrucoes" )->GetText());
    string* qlci = split_config(doc->FirstChildElement( "conf" )->FirstChildElement( "quantidade_linhas_cahce_instrucoes" )->GetText());
    string* qvci = split_config(doc->FirstChildElement( "conf" )->FirstChildElement( "quantidade_vias_cahce_instrucoes" )->GetText());
    string* pwci = split_config(doc->FirstChildElement( "conf" )->FirstChildElement( "politica_de_write_cahce_instrucoes" )->GetText());
    string* aslci = split_config(doc->FirstChildElement( "conf" )->FirstChildElement( "algoritmo_substituicao_linhas_cache_instrucoes" )->GetText());
    

    // cria arrays para guardar os diverso sníveis de cache
    cacheDados = new cache[niveis_de_cache];
    if (split_cache) {
        // inicialização dos caches de dados e instruções
        cacheInstrucoes = new cache[niveis_de_cache];
    } else {
        cacheInstrucoes = cacheDados;
    }
    // instancia os níveis de cache, utilizando os parametros lidos no arquivo de cnfiguração 
    for (int i = 0; i < niveis_de_cache; i++){
        cacheDados[i] = cache(atoi(tbcd[i].c_str()), atoi(qlcd[i].c_str()), atoi(qvcd[i].c_str()), pwcd[i], atoi(aslcd[i].c_str()));
        if (i > 0) cacheDados[i-1].setCacheProximoNivel(&cacheDados[i]); // marca que é o cache de próximo nível do cache anterior

        if (split_cache) {
            // inicialização dos caches de dados e instruções
            cacheInstrucoes[i] = cache(atoi(tbci[i].c_str()), atoi(qlci[i].c_str()), atoi(qvci[i].c_str()), pwci[i], atoi(aslci[i].c_str()));
            if (i > 0) cacheInstrucoes[i-1].setCacheProximoNivel(&cacheInstrucoes[i]);// marca que é o cache de próximo nível do cache anterior
        }
    }
    cacheDados[niveis_de_cache-1].setCacheProximoNivel(NULL); // deixa o último cache do array sem cache de próximo nível
    if (split_cache) 
        cacheInstrucoes[niveis_de_cache-1].setCacheProximoNivel(NULL);  // deixa o último cache do array sem cache de próximo nível
       
};




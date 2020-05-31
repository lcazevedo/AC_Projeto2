
#include "cachecontrol.h"
#include <time.h>


void poolcache() {
    // cria a quantidade de níveis de cache solcitados

    //enquanto não tiver retorno do dado

        // busca no próximo nível de cache

}

int main() {
    clock_t inicio, fim;
    double tempo;
    unsigned long int contador = 0; 

    cachecontrol m = cachecontrol("cachecontrol.conf.xml");
    //cachecontrol m = cachecontrol();


    //m.processaComando("I,8,1");
    //m.processaComando("I,0,1");  
    //m.processaComando("I,8,1");  
    //m.processaComando("I,10,1"); 
    

    //string arquivo = "trace_nano_grande.txt";
    //string arquivo = "trace_ls_medio.txt";
    string arquivo = "trace_hello_pequeno.txt";
    string linha;
    ifstream fs (arquivo); 
    if (fs.is_open())
    {
        inicio = clock();
        while (! fs.eof() ) {
            getline (fs, linha); 
            if (!linha.empty()) {
                //cout << linha << endl;
                m.processaComando(linha);
                contador++;
            }
        }
        fim = clock();
        tempo = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
        //fs.close();
    }

    // quantidade de níveis de cahce configurados
    int L = 1;

    int mi, h, l, g;
    for (int i = 0; i < L; i++) {
        mi = m.getMissesDados(i);
        h  = m.getHitsDados(i);
        l = m.getLeiturasDados(i);
        g = m.getGravacoesDados(i);
        cout << "Cache de Dados L" << (i+1) << ": " << endl;
        cout << "misses   = " << mi << "    Hits      = " << h << "    total = " << mi+h << endl;
        cout << "leituras = " << l  << "    Gravacoes = " << g << "    total = " << l +g << endl;
        mi = m.getMissesInstrucoes(i);
        h  = m.getHitsInstrucoes(i);
        l = m.getLeiturasInstrucoes(i);
        g = m.getGravacoesInstrucoes(i);
        cout << endl;
        cout << "Cache de Instruções L" << (i+1) << ": " << endl;
        cout << "misses   = " << mi << "    Hits      = " << h << "    total = " << mi+h << endl;
        cout << "leituras = " << l  << "    Gravacoes = " << g << "    total = " << l +g << endl;
        cout << endl;
    }
    cout << "Total de comandos processados: " << contador << endl;
    cout.precision(5);
    cout << endl;
    cout << "Tempo de execução: " << fixed  << tempo << " segundos " << endl;
    
}


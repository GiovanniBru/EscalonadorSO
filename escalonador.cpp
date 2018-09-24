/*  Giovanni Bruno Travassos de Carvalho - 11506849
    Trabalho de SO - Escalonador de Processos*/

#include <iostream>
#include<fstream>
#include<deque>
#include<vector>
#include<bits/stdc++.h>

using namespace std;

typedef struct {
    int ProcessID=0;
    int timeDura=0;
    int timeExec=0;
    int timeChegada=0;
    int timeResto=0;
}Process;

vector<int>timedura;
vector<int>timeentr;
//vetores de entrada dos arquivos

void leArquivo(string nome){
        ifstream file;

        file.open("testes.txt");
        int novo_T_entrada, novo_T_duracao;
        if (file.is_open())
        {
            while (file >> novo_T_entrada >> novo_T_duracao){
                timeentr.push_back(novo_T_entrada);
                timedura.push_back(novo_T_duracao);
            }
        }
        file.close();
}

//Ordena caso tempo de chegada esteja fora de ordem
void OrdenaChegada(Process *ordena){
    Process aux;
    int j, i;
    for(j= timeentr.size()-1; j>0; j--){
        for (i=0; i<j; i++){
            if (ordena[i].timeChegada > ordena[i+1].timeChegada){
                aux = ordena[i];
                ordena[i] = ordena[i+1];
                ordena[i+1] = aux;
            }
        }
    }
}

//Compara tempo de duração dos Processos
bool Compara(const Process &p1, const Process &p2){
	return (p1.timeDura <= p2.timeDura);
}

//Calcula Medias e Printa
void CalculaMedias(string TipoEscalonador, float tResposta,float tRetorno,float tEspera){
    int numProcess = timedura.size(); //numero de processos
    float TMResposta = 0;
    float TMRetorno = 0;
    float TMEspera = 0;
    //TM = tempo medio

    TMResposta = tResposta / numProcess;
    TMRetorno = tRetorno / numProcess;
    TMEspera = tEspera / numProcess;

    cout << TipoEscalonador << " "  << TMRetorno << " " << TMResposta << " " << TMEspera <<endl;
}

//FCFS - First-Come, First-Served
void FCFS(const Process *process)
{
        int numProcess = timeentr.size();
        float tInicio[numProcess];
        float tRetorno[numProcess];
        float tEspera[numProcess];
        float tResposta[numProcess];
        //Tempo Medio
        float TMEspera = 0;
        float TMResposta = 0;
        float TMRetorno = 0;
        //Tempo Total
        float TTRetorno = 0;
        float TTResposta = 0;
        float TTEspera = 0;

        tInicio[0] = process[0].timeChegada; //Inicio do primeiro processo = tempo de chegada

        int i;
        for(i=0;i<numProcess;i++){
            tInicio[i+1] = tInicio[i] + process[i].timeDura;

            tResposta[i] = tInicio[i] - process[i].timeChegada;
            TTResposta = TTResposta + tResposta[i];

            tEspera[i] = tInicio[i] - process[i].timeChegada;
            TTEspera = TTEspera + tEspera[i];

            tRetorno[i] = tInicio[i+1] - process[i].timeChegada;
            TTRetorno = TTRetorno + tRetorno[i];
        }

        CalculaMedias("FCFS ",TTResposta,TTRetorno,TTEspera);
}

//SJF - Shorted Job First
void SJF(const Process *process)
{
    int numProcess = timeentr.size();
	float tResposta = 0;
	float tRetorno = 0;
	float tEspera = 0;
	float TMResposta = 0;
	float TMRetorno = 0;
	float TMEspera = 0;
	int retorno = numProcess;
	int ciclo = 0;

	vector<Process>novos;
	vector<Process>prontos;

    int i;
    for (i= 0; i < numProcess; i++){
         novos.push_back(process[i]);
    }

    while(retorno != 0){
        for (i = 0; i < novos.size(); i++){
            if( novos[i].timeChegada <= ciclo)
            {
                prontos.push_back(novos[i]);
                novos.erase (novos.begin()+i);
                i--;
            }
        }
        sort(prontos.begin(),prontos.end(),Compara);

        Process p = prontos[0];
        retorno--;
        prontos.erase(prontos.begin());

        tEspera = tEspera + ciclo - p.timeChegada;
        tResposta = tResposta + ciclo - p.timeChegada;
        ciclo = ciclo + p.timeDura;
        tRetorno  = tRetorno + ciclo - p.timeChegada;
      	}
    CalculaMedias("SJF ",tResposta,tRetorno,tEspera);
}

//RR - Round Robin
void RR(const Process *process){
    int numProcess = timeentr.size();
  	float tResposta = 0;
  	float tRetorno = 0;
  	float tEspera = 0;
  	float TMResposta = 0;
  	float TMRetorno = 0;
  	float TMEspera = 0;
  	int retorno = numProcess;
  	int ciclo = 0;
  	int q = 2; //quantum
  	bool  volta_para_pronto = false;

	vector<Process> prontos;
	vector<Process> novos;

	Process inicio;

    int i;
    // Coloca todos os processo na fila de novos
    for (i= 0; i < numProcess; i++){
    		novos.push_back(process[i]);
    }
    while(retorno != 0){
        for (i = 0; i < novos.size(); i++){
    			if( novos[i].timeChegada == ciclo){
    				prontos.push_back(novos[i]);
    				novos.erase (novos.begin()+i);
    				i--;
    			}else if( novos[i].timeChegada < ciclo){
    				tEspera += ciclo - novos[i].timeChegada;
    				prontos.push_back(novos[i]);
    				novos.erase (novos.begin()+i);
    				i--;
    			}
        }

        if(volta_para_pronto){
            prontos.push_back(inicio);
            volta_para_pronto = false;
        }

        inicio = prontos[0];
        prontos.erase(prontos.begin());

        if(inicio.timeResto == inicio.timeDura){
            tResposta += ciclo - inicio.timeChegada;
        }

   		ciclo = ciclo + q;
        inicio.timeResto = inicio.timeResto - q;
        tEspera = tEspera + q * prontos.size();

        if(inicio.timeResto > 0){
            volta_para_pronto = true;
        }

        if(inicio.timeResto <= 0){
            retorno--;
            int numProntos = prontos.size();

            ciclo = ciclo + inicio.timeResto;
            tEspera = tEspera + (inicio.timeResto * numProntos);
            tRetorno = tRetorno + ciclo - inicio.timeChegada;
        }

    }
    CalculaMedias("RR ",tResposta,tRetorno,tEspera);

}

void print (const Process *process, int numProcess){
    int i;
    for(i=0;i<numProcess;i++){
      cout<<"ID do Processo: "<<process[i].ProcessID<<" Tempo de Entrada: "<<process[i].timeChegada<<" Tempo de Duracao: "<<process[i].timeDura<<endl;
    }//Printa os processos e seus tempos
}

void ArmazenaNoStruct(Process *process,int numProcess){
    int i; //Armazena dados de entrada do arquivo
    for(i = 0; i < numProcess; i++){
      process[i].ProcessID = i;
      process[i].timeChegada = timeentr[i];
      process[i].timeDura = timedura[i];
      process[i].timeResto = process[i].timeDura;

      if(process[i].timeDura== 0){
          numProcess = numProcess - 1;
      }
    }
}

int main(){
    leArquivo("testesSO.txt");

    int numProcess = timeentr.size();

    Process * process = new Process[numProcess];

    ArmazenaNoStruct(process,numProcess);

    OrdenaChegada(process);
    print(process, numProcess);

    cout << endl;

    FCFS(process);
    SJF(process);
    RR(process);

    return 0;
}

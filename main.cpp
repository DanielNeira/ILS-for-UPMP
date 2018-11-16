//
//  main.cpp
//  UPMP - iterated local search
//
//  Created by Daniel on 21-10-18.
//  Copyright © 2018 Daniel Neira. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <map>
#include <time.h>

#define d 1
#define INF 9999999
using namespace std;

vector<vector<int>> tiempo_proceso;
vector<vector<int>> orden_trabajos;
vector<int> tiempo_por_maquina;
vector<int> Lista_reposicion;
map<int,vector<int>> santa_cachucha; //map ordenado que guarda la máquina y un vector con las tareas asignadas a la maquina
int Makespan;
int Maquinas;
int Trabajos;

void leer_archivo(string nombre){
    ifstream myfile;
    myfile.open(nombre);
    if (myfile.is_open()) {
        cout<<"El archivo se abrió"<<endl;
        string line;
        getline(myfile, line);
        Trabajos = atoi(line.substr(0,line.find(" ")).c_str());
        Maquinas = atoi(line.substr(line.find(" ")).c_str());
        cout<<"Maquinas: "<<Maquinas<<", "<<"Trabajos :"<<Trabajos<<endl;
        getline(myfile, line);
        vector<int> temp_vector;
        int temp_valor;
        while (getline(myfile, line)) {
             for (int j=0; j < Maquinas; j++) {
                 line = line.substr(line.find('\t')+1);
                 line = line.substr(line.find('\t')+1);
                 temp_valor = atoi(line.substr(0,line.find('\t')).c_str());
                 temp_vector.push_back(temp_valor);
             }
            tiempo_proceso.push_back(temp_vector);
            temp_vector.clear();
         }
        //Para verla porsiacaso
        cout<<"J-M";
        for (int i=0; i<Maquinas; i++) {
            cout<<'\t'<<"T"<<i+1;
        }
        cout<<endl;
        for (int i=0; i<Trabajos; i++) {
            cout << i <<'\t';
            for (int j=0; j<Maquinas; j++) {
                cout<<tiempo_proceso[i][j]<<'\t';
            }
            cout<<endl;
        }
        myfile.close();
    }
    else{
        cout <<"El archivo no se abrió o no existe. \n cerdo estupido"<<endl;;
    }
}

void generar_sol_inicial(){
    int temp_sum_tiempo_maquina[Maquinas];
    for (int i=0; i<Maquinas; i++) { //inicializando el map y los tiempos
        santa_cachucha.insert(pair<int,vector<int>> (i, vector<int>()));
        temp_sum_tiempo_maquina[i]= 0;
    }
    /*for (int i = 0; i< Trabajos; i++) {
        auto it = min_element(tiempo_proceso[i].begin(), tiempo_proceso[i].end());
        int posi = (int) (it - tiempo_proceso[i].begin());
        cout << *tiempo_proceso[i].begin()<<"  "<< *tiempo_proceso[i].end()<<" "<<*it<<" posicion "<< posi<<endl;
    }*/
    for (int i=0; i<Trabajos; i++) {
        auto it = min_element(tiempo_proceso[i].begin(), tiempo_proceso[i].end());
        int posi = (int) (it - tiempo_proceso[i].begin());
        santa_cachucha[posi].push_back(i);
        temp_sum_tiempo_maquina[posi] += *it;
    }
    for (int i=0; i<Maquinas; i++) {
        cout <<"Tiempo total Máquina "<< i <<": "<<temp_sum_tiempo_maquina[i]<<", Tareas incluidas: ";
        for (auto it=santa_cachucha[i].begin(); it!=santa_cachucha[i].end(); ++it) {
            cout << *it<<" ";
        }
        cout << endl;
    }
    Makespan = *max_element(temp_sum_tiempo_maquina, temp_sum_tiempo_maquina+Maquinas);
    cout<<"Makespan inicial es: "<< Makespan<<endl;
    for (int i=0; i<Maquinas; i++) {
        tiempo_por_maquina.push_back(temp_sum_tiempo_maquina[i]);
    }
}

void fase_de_destruccion(bool verbose){
    int maq_de_task, pos_task_a_sacar;
    for (int i=0; i<d; i++) {
        maq_de_task = rand()% Maquinas;
        //cout<<"Maquina "<<maq_de_task<<" tiene "<<santa_cachucha[maq_de_task].size()<<" tareas asignadas "<<endl;
        while (santa_cachucha[maq_de_task].size() == 0) {
            maq_de_task = rand()% Maquinas;
        }
        pos_task_a_sacar = rand()%santa_cachucha[maq_de_task].size();
        if (verbose){cout <<"Que maquina, posicion, actividad y tiempo:  "<< maq_de_task<<" , "<< pos_task_a_sacar <<" , "<<santa_cachucha[maq_de_task][pos_task_a_sacar]<< " , "<<tiempo_proceso[santa_cachucha[maq_de_task][pos_task_a_sacar]][maq_de_task]<<endl;}
        Lista_reposicion.push_back(santa_cachucha[maq_de_task][pos_task_a_sacar]);
        //Actualizamos ahora los tiempos de las maquinas
        if (verbose) cout<<"Tiempo para maquina " << maq_de_task<<" baja de "<< tiempo_por_maquina[maq_de_task];
        tiempo_por_maquina[maq_de_task] -= tiempo_proceso[santa_cachucha[maq_de_task][pos_task_a_sacar]][maq_de_task];
        if (verbose) cout<<" a "<< tiempo_por_maquina[maq_de_task] <<endl;
        //eliminamos la tarea del vector
        santa_cachucha[maq_de_task].erase(remove(santa_cachucha[maq_de_task].begin(), santa_cachucha[maq_de_task].end(), santa_cachucha[maq_de_task][pos_task_a_sacar]), santa_cachucha[maq_de_task].end());
    }
}

void fase_de_construccion(bool verbose){
    int maquina_donde_poner_task, mini;
    for (int i=0; i<Lista_reposicion.size(); i++) {// i posicion Lista_reposicion[i] elemento
        mini=INF;
        maquina_donde_poner_task=-1;
        for (int j=0; j<Maquinas; j++) {
            if ( tiempo_por_maquina[j]+tiempo_proceso[Lista_reposicion[i]][j] < mini) {
                mini = tiempo_por_maquina[j]+tiempo_proceso[Lista_reposicion[i]][j];
                maquina_donde_poner_task = j;
            }
        }
        if (verbose) {cout<<"La actividad "<<Lista_reposicion[i]<<" se pone en Maquina "<<maquina_donde_poner_task<<", Tiempo antes y despues "<<tiempo_por_maquina[maquina_donde_poner_task]<<"->"<<mini<<" diferencia "<< tiempo_proceso[Lista_reposicion[i]][maquina_donde_poner_task]<<endl;}
        for (int k=0; k<Maquinas; k++) {
            cout<<tiempo_proceso[Lista_reposicion[i]][k]<<" ";
        }cout<<endl;
        tiempo_por_maquina[maquina_donde_poner_task] = mini;
        santa_cachucha[maquina_donde_poner_task].push_back(Lista_reposicion[i]);
    }
    //se vacia la lista de reposicion
    Lista_reposicion.clear();
    //recalcula makespan
    Makespan = *max_element(tiempo_por_maquina.begin(),tiempo_por_maquina.end());
}

void imprimir_maquinas(){
    for (int i=0; i<Maquinas; i++) {
        cout <<"Tiempo total Máquina "<< i <<": "<<tiempo_por_maquina[i]<<", Tareas incluidas: ";
        for (auto it=santa_cachucha[i].begin(); it!=santa_cachucha[i].end(); ++it) {
            cout << *it<<" ";
        }
        cout << endl;
    }
    cout <<"El Makespan es: "<<Makespan<<endl;
}

void imprimir_lista_de_reposicion(){
    if (Lista_reposicion.size()>0) {
        for (auto it=Lista_reposicion.begin(); it!=Lista_reposicion.end(); it++) {
            cout<<*it<<" ";
        }
        cout<<endl;
    }
    else{
        cout<<"No hay tareas a eliminar en lista de reposición"<<endl;
    }
}

int main(int argc, const char * argv[]) {
    srand(time(0));
    leer_archivo("Instance 2.txt");
    generar_sol_inicial();
    clock_t t=clock();
    vector<int> avance_makespan;
    bool verbose = false;
    bool result = true;
    avance_makespan.push_back(Makespan);
    while (((float)(clock()-t))/CLOCKS_PER_SEC < 2) { //1800
        fase_de_destruccion(verbose);
        imprimir_maquinas();
        //imprimir_lista_de_reposicion();
        fase_de_construccion(verbose);
        imprimir_maquinas();
        //imprimir_lista_de_reposicion();
        avance_makespan.push_back(Makespan);
    }
    if (result){
    for (auto it=avance_makespan.begin(); it!=avance_makespan.end(); it++) {
        cout<<*it<<" ";
    }
    cout<<endl;
    cout<<"Menor Makespan encontrado: "<<*min_element(avance_makespan.begin(), avance_makespan.end())<<endl;
    }
}

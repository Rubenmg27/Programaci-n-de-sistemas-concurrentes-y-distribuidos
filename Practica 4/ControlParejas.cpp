//------------------------------------------------------------------------------
// File:   ControlaParejas.cpp
// Authors:Sergio Salesa Quintanilla NIP:811019
//         Rubén Martín González NIP:841886
// Date:   20 de noviembre de 2022
// Coms:   Fichero .cpp del monitor usado en la práctica 4 de PSCD
//------------------------------------------------------------------------------
#include <iostream>
#include <iomanip>
#include "ControlParejas.hpp"

controlParejas::controlParejas(){
    for (unsigned int i = 0; i < N_EST; i++){parejas[i]=-1;fin[i]=false;}
    terminados=0;
    libres=2;
    filas=0;
}

void controlParejas::esperarlibre(int i){
    unique_lock<mutex> lck(mtxMonitor);
    while (libres==0)
    {
        puedeEmparejar.wait(lck);
    }
    libres--;
    examinados[libres]=i;
    if (libres==0)
    {
        puedeCrear.notify_one();
    }
}

void controlParejas::emparejar(){
    unique_lock<mutex> lck(mtxMonitor);
    while (libres>0)
    {
        puedeCrear.wait(lck);
    }
    parejas[examinados[0]]=examinados[1];
    parejas[examinados[1]]=examinados[0];
    asigFila[examinados[0]]=filas;
    asigFila[examinados[1]]=filas;
    libres=2;
    filas++;
    puedeEmparejar.notify_all();
    parejaok.notify_one();
    parejaok.notify_one();
}


void controlParejas::parejacreada(int i,int &aux,int &fila){
    unique_lock<mutex> lck(mtxMonitor);
    if(parejas[i]==-1)
    {
        parejaok.wait(lck);
    }
    fila=asigFila[i];
    aux=parejas[i];
    
    
}

void controlParejas::recibirmax(int i){
    unique_lock<mutex> lck(mtxMonitor);
    if (!fin[i])
    {
        pasadomax[i].wait(lck);
    }   
}

void controlParejas::pasarmax(int i){
    unique_lock<mutex> lck(mtxMonitor);
    fin[parejas[i]]=true;
    pasadomax[parejas[i]].notify_one();
}

void controlParejas::acabado(){
    unique_lock<mutex> lck(mtxMonitor);
    while (terminados!=N_FIL)
    {
        finalizar.wait(lck);
    }
}


void controlParejas::comunicarfinest(int fila,int nip,int miPareja,int res,int suma){
    unique_lock<mutex> lck(mtxMonitor);
    cout<<left<<setw(5)<<to_string(fila)<<"|"<<right<<setw(5)<<to_string(miPareja)<<"-"<<left<<setw(5)<<to_string(nip)<<"|"<<right<<setw(8)<<to_string(res)<<right<<setw(3)<<"|"<<right<<setw(9)<<to_string(suma)<<endl;
    terminados++;
    if (terminados==N_FIL)
    {
        finalizar.notify_all();
    }
}
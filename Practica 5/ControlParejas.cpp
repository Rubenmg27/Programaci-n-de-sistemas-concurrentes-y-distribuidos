//******************************************************************
// File:   ControlParejas.cpp
// Author: Sergio Salesa Quintanilla  NIP: 811019
// Author: Rubén Martín González      NIP: 841886
// Date:   20 Diciembre de 2022
// Coms:   Fichero .cpp del monitor utilizado en la práctica 5 para solucionarla
//******************************************************************

#include "ControlParejas.hpp"


//iniciamos las variables
controlParejas::controlParejas(){
    for (unsigned int i = 0; i < N_EST; i++){parejas[i]=-1;fin[i]=false;}
    terminados=0;
    libres=2;
    filas=0;
}


//si no hay sillas libres se bloquea esperando a que haya una libre
//si hay libre se queda eperando a que se le asigne pareja
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


//se bloquea hasta que dos alumnos esten sentados y les asigna una fila y el nip de la pareja
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


//alumno se bloquea hasta que se le asigne compañero y fila
void controlParejas::parejacreada(int i,int &aux,int &fila){
    unique_lock<mutex> lck(mtxMonitor);
    if(parejas[i]==-1)
    {
        parejaok.wait(lck);
    }
    fila=asigFila[i];
    aux=parejas[i];
    
    
}

//el alumno con nip mayor espera y recibe el max de su pareja una vez este calculado
void controlParejas::recibirmax(int i,int fila,int suma,int& max){
    unique_lock<mutex> lck(mtxMonitor);
    if (!fin[i])
    {
        pasadomax[i].wait(lck);
    }
    max=resultados[fila][0];
}

//comunica a su pareja que a calculado el max de la fila
void controlParejas::pasarmax(int i,int fila,int maximo){
    unique_lock<mutex> lck(mtxMonitor);
    resultados[fila][0]=maximo;
    fin[parejas[i]]=true;
    pasadomax[parejas[i]].notify_one();
}

//se bloquea hasta que todos los alumnos hayan acabado de realizar sus tareas
void controlParejas::acabado(){
    unique_lock<mutex> lck(mtxMonitor);
    while (terminados!=N_FIL)
    {
        finalizar.wait(lck);
    }
}

//cada pareja comunica que ha terminado y si todos han finalizado se le comunica al profesor
void controlParejas::comunicarfinest(){
    unique_lock<mutex> lck(mtxMonitor);
    terminados++;
    if (terminados==N_FIL)
    {
        finalizar.notify_all();
    }
}
//******************************************************************
// File:   ControlParejas.hpp
// Author: Sergio Salesa Quintanilla  NIP: 811019
// Author: Rubén Martín González      NIP: 841886
// Date:   20 Diciembre de 2022
// Coms:   Fichero .h del monitor utilizado en la práctica 5 para solucionarla
//******************************************************************

#include <mutex>
#include <condition_variable>
#include <iostream>

using namespace std;

const int N_EST = 60; // # de estudiantes
const int N_FIL = N_EST /2; // # de filas en la matriz
const int N_COL = 1000; // # de columnas

class controlParejas{

    public:
        controlParejas();
        void esperarlibre(int i);
        void emparejar();
        void parejacreada(int i,int &aux,int &fila);
        void pasarmax(int i,int fila,int maximo);
        void recibirmax(int i,int fila,int suma,int& max);
        void acabado();
        void comunicarfinest();
    private:
        int parejas[N_EST];
        int asigFila[N_EST];
        int examinados[2];
        bool fin[N_EST];
        int resultados[N_FIL][2];
        int terminados;
        int libres;
        int filas;
        
        mutex mtxMonitor;
        condition_variable puedeEmparejar,puedeCrear,parejaok,pasadomax[N_EST],finalizar;

};
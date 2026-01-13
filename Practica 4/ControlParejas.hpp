//------------------------------------------------------------------------------
// File:   ControlaParejas.hpp
// Authors:Sergio Salesa Quintanilla NIP:811019
//         Rubén Martín González NIP:841886
// Date:   20 de noviembre de 2022
// Coms:   Fichero .hpp del monitor usado en la práctica 4 de PSCD
//------------------------------------------------------------------------------
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
        void pasarmax(int i);
        void recibirmax(int i);
        void acabado();
        void comunicarfinest(int fila,int nip,int miPareja,int res,int suma);
    private:
        int parejas[N_EST];
        int asigFila[N_EST];
        int examinados[2];
        bool fin[N_EST];
        int terminados;
        int libres;
        int filas;
        
        mutex mtxMonitor;
        condition_variable puedeEmparejar,puedeCrear,parejaok,pasadomax[N_EST],finalizar;

};
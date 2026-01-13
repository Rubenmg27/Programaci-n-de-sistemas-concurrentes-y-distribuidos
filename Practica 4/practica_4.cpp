//------------------------------------------------------------------------------
// File:   practica_4.cpp
// Authors:Sergio Salesa Quintanilla NIP:811019
//         Rubén Martín González NIP:841886
// Date:   20 de noviembre de 2022
// Coms:   Práctica 4 de PSCD
//         Compilar mediante
//           make -f Makefile_p4
//------------------------------------------------------------------------------

#include <iostream>
#include <thread>
#include <string>
#include <fstream>
#include <atomic>
using namespace std;

#include "ControlParejas.hpp"
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void meterDatos(int D[][N_COL],string nombre){
    ifstream f(nombre);
    if (f.is_open())
    {
        for (unsigned int i = 0; i < N_FIL; i++)
        {
            for (unsigned int j = 0; j < N_COL; j++)
            {
                f >> D[i][j];
            }
        }
    }
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Pre : < fila > es un  ́ı ndice de fila de <D >
// Post : devuelve el m  ́a ximo de la fila < fila >
int maxFila ( int D [ N_FIL ][ N_COL ] , int fila ) {
    int max = 0;
    for (unsigned int i = 0; i < N_COL; i++)
    {
        if (D[fila][i]>max)
        {
            max=D[fila][i];
        }
    }
    return max ;
}
// Pre : < fila > es un  ́ı ndice de fila de <D >
// Post : devuelve la suma de los els . de la fila < fila >
int sumaFila ( int D [ N_FIL ][ N_COL ] , int fila ) {
    int sum = 0;
    for (unsigned int i = 0; i < N_COL; i++)
    {
        sum = sum + D[fila][i];
    }
    return sum ;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Estudiante (int nip,int D [ N_FIL ][ N_COL ],int res[N_EST],controlParejas &c) {
	
    /*
    <await libre>0
		libre--;
		examinado[libre]=nip;
	>
    */
    c.esperarlibre(nip);
    //<await parejas[nip]!=-1>          esperar me sea asignada pareja y fila    
    int miPareja=0,fila=0;            
    c.parejacreada(nip,miPareja,fila);
    if (nip < miPareja) {
        res[miPareja]=maxFila(D,fila);        // calcular max de mi fila
        //<fin[pareja[nip]]=true;>              hacerselo llegar a mi pareja
        c.pasarmax(nip);
    }
    else {
        int suma=sumaFila(D,fila);      // calcular la suma de mi fila
        /*
        <await fin[nip]             coger info de max ( de mi pareja )                          
            
        >                  
        */
        c.recibirmax(nip);
        // mostrar resultados y comunicar finalizacion
        //cout de la informacion y <terminados:=terminados+1>
        c.comunicarfinest(fila,nip,miPareja,res[nip],suma);
    }
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Profesor (controlParejas &c) {
    for ( int i =0; i < N_FIL ; i ++) {
        
        /*
        <await libre===0
        pareja[examinado[1]]=examinado[0];
        pareja[examinado[0]]=examinado[1];
        asigFila[examinado[1]]=i;
        asigFila[examinado[0]]=i;
        libre=2;
        >
        */
        // esperar a que haya dos
        // comunicar a cada uno su pareja , y la fila que les toca
        c.emparejar();
    }
    //<await filaExam==N_FIL>
    // esperar que todos hayan terminado
    c.acabado();
    cout << "\n";
    cout << "\n";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int main () {
    controlParejas c;
    int D [ N_FIL ][ N_COL ]; // para almacenar los datos
    int result[N_EST];
    string nom = "datos.txt";
    // cargar " datos . txt " en " D "
    meterDatos(D,nom);    
    thread P[N_EST];
    thread th_1(&Profesor,ref(c));   //se inician los procesos
    for(int i=0;i<N_EST;i++){
        P[i] = thread(&Estudiante,i,D,result,ref(c));
    }

    th_1.join();        //se bloquean los procesos
    for(int i=0;i<N_EST;i++){
        P[i].join();
    }

    cout << " Prueba finalizada \n " ;
    return 0;
}
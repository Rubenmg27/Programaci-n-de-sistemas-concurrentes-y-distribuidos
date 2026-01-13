//------------------------------------------------------------------------------
// File:   practica_3.cpp
// Authors:Sergio Salesa Quintanilla NIP:811019
//         Rubén Martín González NIP:841886
// Date:   5 de noviembre de 2022
// Coms:   Práctica 3 de PSCD
//         Compilar mediante
//           make -f Makefile_p3
//------------------------------------------------------------------------------

#include <iostream>
#include <thread>
#include <string>
#include <fstream>
#include <atomic>
#include "librerias/Logger_V3/Logger.hpp"
#include "librerias/Semaphore_V4/Semaphore_V4.hpp"
using namespace std;


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const int N_EST = 60; // # de estudiantes
const int N_FIL = N_EST /2; // # de filas en la matriz
const int N_COL = 1000; // # de columnas

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


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
void pasarTestigo(Semaphore& testigo, Semaphore& b1, Semaphore& b2,Semaphore* b3[N_EST],Semaphore& b4,Semaphore* b5[N_EST], int& d1, int& d2,int d3[],int& d4,int d5[], int& libres, int parejas[], int& terminados, bool fin[]){
    int var = 0;
    int i = 0,j=0;
    bool enco = false,encon=false,a4=false,a1=false;
    while (i<N_EST && !enco)
    {
        if(parejas[i]!=-1 && d3[i]>0){
            var=3;
            enco=true;
        }
        else{
            i++;
        }
    }
    
    
    while (j<N_EST && !encon && !enco)
    {
        if(fin[j] && d5[j]>0){
            var=5;
            encon=true;
        }
        else{
            j++;
        }
    }
    
    if (terminados==N_FIL && d4>0 && !enco && !encon)
    {
        var=4;
        a4=true;
    }
    else if (libres>0 && d1>0 && !enco && !encon && !a4)
    {
        var=1;
        a1=true;
    }
    else if (libres==0 && d2>0 && !enco && !encon && !a4 && !a1)
    {
        var=2;
    }
    switch(var){
        case 1: d1--; b1.signal();
            break;    
        case 2: d2--; b2.signal();
            break;
        case 3: d3[i]--; b3[i]->signal();
            break;    
        case 4: d4--; b4.signal();
            break;
        case 5: d5[j]--; b5[j]->signal();
            break;
        default: testigo.signal();
    }
    
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Estudiante ( int nip,Semaphore& testigo, Semaphore& b1, Semaphore& b2,Semaphore* b3[N_EST],Semaphore& b4,Semaphore* b5[N_EST], int& d1, int& d2,int d3[],int& d4, int d5[],int& libres,int parejas[], int asigFila[],int examinado[], int& terminados,bool fin[],int res[],int D[][N_COL]) {
    // esperar por una silla libre
    /*	
    <await libre>0
		libre--;
		examinado[libre]=nip;
	>
    */
    testigo.wait();
    if (libres==0)
    {
        d1++;
        testigo.signal();
        b1.wait();
    }
    libres--;
    examinado[libres]=nip;
    pasarTestigo(ref(testigo),ref(b1),ref(b2),b3,ref(b4),b5,d1,d2,d3,d4,d5,libres,parejas,terminados,fin);  

    //<await parejas[nip]!=-1>         esperar me sea asignada pareja y fila 
    testigo.wait();
    if (parejas[nip]==-1)
    {
        d3[nip]++;
        testigo.signal();
        b3[nip]->wait();
    }
    pasarTestigo(ref(testigo),ref(b1),ref(b2),b3,ref(b4),b5,d1,d2,d3,d4,d5,libres,parejas,terminados,fin);  

    if (nip < parejas[nip]) {
        // calcular max de mi fila
        res[parejas[nip]]=maxFila(D,asigFila[nip]);
        //hacerselo llegar a mi pareja
        //<fin[pareja[nip]]=true;>              
        testigo.wait();
        fin[parejas[nip]]=true;
        pasarTestigo(ref(testigo),ref(b1),ref(b2),b3,ref(b4),b5,d1,d2,d3,d4,d5,libres,parejas,terminados,fin); 
    }
    else {
        // calcular la suma de mi fila
        int sum=sumaFila(D,asigFila[nip]);
        // esperar nip menor acabe
        /*
        <await fin[nip]                                 
            filaExam++;                 
        >       
        */
        testigo.wait();
        if (!fin[nip])
        {
            d5[nip]++;
            testigo.signal();
            b5[nip]->wait();
        }
        // comunicar finalizacion
        terminados++;
        // mostrar resultados
        cout << " El maximo de mi pareja con nip: " + to_string(parejas[nip]) + " es " + to_string(res[nip]) + " y la suma obtenida por mi nip: " + to_string(nip) + " es " + to_string(sum) + "\n";
        pasarTestigo(ref(testigo),ref(b1),ref(b2),b3,ref(b4),b5,d1,d2,d3,d4,d5,libres,parejas,terminados,fin); 
    }
    
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Profesor (Semaphore& testigo, Semaphore& b1, Semaphore& b2,Semaphore* b3[N_EST],Semaphore& b4,Semaphore* b5[N_EST],int& d1, int& d2,int d3[],int& d4, int d5[],int& libres,int parejas[], int asigFila[],int examinado[], int& terminados,bool fin[]) {
    for ( int i =0; i < N_FIL ; i ++) {
        // esperar a que haya dos
        // comunicar a cada uno su pareja , y la fila que les toca
        /*        
        <await libre===0
        pareja[examinado[1]]=examinado[0];
        pareja[examinado[0]]=examinado[1];
        asigFila[examinado[1]]=i;
        asigFila[examinado[0]]=i;
        libre=2;
        >
        */
        testigo.wait();
        if (libres>0)
        {
            d2++;
            testigo.signal();
            b2.wait();
        }
        parejas[examinado[0]]=examinado[1];
        parejas[examinado[1]]=examinado[0];
        asigFila[examinado[0]]=i;
        asigFila[examinado[1]]=i;
        libres=2;
        pasarTestigo(ref(testigo),ref(b1),ref(b2),b3,ref(b4),b5,d1,d2,d3,d4,d5,libres,parejas,terminados,fin);   
    }
    // esperar que todos hayan terminado
    //<await filaExam==N_FIL>
    testigo.wait();
    if (terminados<N_FIL)
    {
        d4++;
        testigo.signal();
        b4.wait();
    }
    pasarTestigo(ref(testigo),ref(b1),ref(b2),b3,ref(b4),b5,d1,d2,d3,d4,d5,libres,parejas,terminados,fin);  
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int main () {
    int D [ N_FIL ][ N_COL ]; // para almacenar los datos
    int fila = 0; // cada pareja coger  ́a una
    int pareja [ N_EST ]; // pareja [ i ] ser  ́a la pareja asignada
    
    Semaphore testigo(1),b1(0),b2(0),b4(0);
    Semaphore* b3[N_EST];
    Semaphore* b5[N_EST];
    bool fin[N_EST];
    int d1=0,d2=0,d3[N_EST],d4=0,d5[N_EST],res[N_EST];
    for (unsigned int i = 0; i < N_EST; i++)
    {
        b3[i]= new Semaphore(0);
        b5[i]= new Semaphore(0);
        pareja[i]=-1;
        d3[i]=0;
        d5[i]=0;
        fin[i]=false;
    }
    // cargar " datos . txt " en " D "
    string nFichero = "datos.txt";
    meterDatos(D,nFichero);
    int examinado[2], asigFilas[N_EST],libres=2;
    thread E[N_EST];
    thread P(&Profesor,ref(testigo),ref(b1),ref(b2),b3,ref(b4),b5,ref(d1),ref(d2),d3,ref(d4),d5,ref(libres),pareja,asigFilas,examinado, ref(fila),fin);
    
    for (unsigned int i = 0; i < N_EST; i++)
    {
        E[i]=thread(&Estudiante,i,ref(testigo),ref(b1),ref(b2),b3,ref(b4),b5,ref(d1),ref(d2),d3,ref(d4),d5,ref(libres),pareja,asigFilas,examinado, ref(fila),fin,res,D);
    }
    
    for (unsigned int i = 0; i < N_EST; i++)
    {
        E[i].join();
    }
    P.join();    
    cout << " Prueba finalizada \n " ;
    
   
    return 0;
}
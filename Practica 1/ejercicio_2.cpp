//------------------------------------------------------------------------------
// File:   ejercicio_2.cpp
// Authors:Sergio Salesa Quintanilla NIP:811019
//         Rubén Martín González NIP:841886
// Date:   29 de septiembre de 2022
// Coms:   Parte del práctica 1 de PSCD
//         Compilar mediante
//           g++ ejercicio_2.cpp -o ejercicio_2 -std=c++11 -pthread
//------------------------------------------------------------------------------

#include <iostream>
#include <thread>
#include <string>
#include <chrono>
#include <cstdlib>

using namespace std;

//Pre: int nombre es el nombre  del proceso lanzado, int retardo es el retardo del thread y int veces el numero de veces que se repetirá.
//Post: devuelve la cadena de caracteres "Soy << nombre << enld".
void saludo(int nombre, int retardo, int veces) {
    for(int i=1; i<=veces; i++) {
        //cout << "Soy " << nombre << endl;
        cout << "Soy " + to_string(nombre) + "\n";
        //el thread que me ejecuta se bloquea durante "retardo" milisegundos
        this_thread::sleep_for(chrono::milliseconds(retardo));
    }
}


int main(int argc, char* argv[]) {
    const int N = 10;
    thread P[N]; //de momento, ningún thread se pone en marcha
    srand(time(NULL));
    P[0] = thread(&saludo, 1,  rand()%201 + 100, rand()%11 + 5); //P[0] se pone en marcha
    for (unsigned int i = 1; i < N; i++)
    {
        P[i]= thread(&saludo, i+1, rand()%201 + 100, rand()%11 + 5);
    }

    for (unsigned int i = 0; i < N; i++)
    {
        P[i].join();
    }
    



    cout << "Fin\n";
    return 0;
}

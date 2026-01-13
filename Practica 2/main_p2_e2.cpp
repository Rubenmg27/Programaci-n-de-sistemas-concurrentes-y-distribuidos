//------------------------------------------------------------------------------
// File:   main_p2_e2.cpp
// Authors:Sergio Salesa Quintanilla NIP:811019
//         Rubén Martín González NIP:841886
// Date:   16 de septiembre de 2022
// Coms:   Parte del práctica 2 de PSCD
//         Compilar mediante
//           g++ main_p2_e2.cpp -o main_p2_e2 -std=c++11 -pthread
//           o mediante
//           make -f Makefile_p2_e2
//------------------------------------------------------------------------------
#include <atomic>
#include <iostream>
#include <thread>
#include <string>
#include <fstream>

const int N = 512;     //# de datos
const int N_BUSC = 8; //# de procesos buscadores
int maxVeces=0;
int indMin,indMax; //definimos las variables globales


using namespace std;
using VectInt = int[N]; //"VectInt" es un "alias" para vector de int de dim. N

//Pre: v es un vector de tipo VectInt con N elementos y se encuentra vacio
//Post:rellena cada espacio del vector con un entero del fichero datos.txt
void meterDatos(VectInt v,string nom){
    ifstream f(nom);
    if (f.is_open())
    {
        string aux;
        char delimitador = 32;
        int i = 0;
        while (!f.eof())
        {
           getline(f,aux,delimitador);
           v[i]=atoi(aux.c_str());
           i++;
        }
    }
}


//-----------------------------------------------------
//Pre: 0 ≤ i ≤ d ≤ N − 1
//Post: devuelve en maxVeces el numero de repeticiones si este es mayor que en el tramo anterior y en indMin e indMax devuelve i y d del corresponiente tramo.
void search(const VectInt v,const int i, const int d,const int value, int& maxVeces, int& indMin, int& indMax, atomic_flag& tas){
    int result=0;
    for(int j=i;j<d;j++){
        if(v[j]==value){   //si coincide se aumenta en 1 el resultado
            result++; 
        }
    }
    while(tas.test_and_set()){}  //sirve para hacer atómica la operación
    if(result>maxVeces){        //comparamos con la variable global
        maxVeces=result;
        indMin=i;
        indMax=d;
    } 
    tas.clear();
}


//Pre: VectInt contiene los enteros, noCargado sera true si VecInt esta cargado, i y d contienen los indices del trozo a buscar, value el valor a buscar. Los vectores
//     veces y noCalculado no tendran valor y estarán a true en todos sus componentes respectivamente. 
//Post: Almacena en veces el número de veces que se repite el valor en ese tramo. A su vez noCalculado se pondrá a false. 
void buscador(const VectInt v,bool& leido,int ind,const int i, const int d,const int value,bool calculado[],atomic_flag& tas){
    while(!leido){
        std::this_thread::yield(); //se deja paso a otro proceso
    }
    search(v,i,d,value,maxVeces,indMin,indMax,tas);
    calculado[ind]=true;   //una vez obtenido el resultado del proceso i se pone a true el vector de booleanos
}

//Pre: VectInt es un vector vacio, noCargado es true. 
//Post: Carga los datos en VecInt, noCargado devuelve false y devuelve en resultado el número de veces que se ha repetido el número elegido.
void coordinador(VectInt v,bool& leido,bool calculado[],string nom){
    meterDatos(v,nom);
    leido=true;  //ya estan cargados los datos         
    for(int i=0;i<N_BUSC;i++){
        while(!calculado[i]){
            std::this_thread::yield();  //se deja paso a otro proceso
        }
    }
    cout<<"Máximo de repeticiones -> "+to_string(maxVeces)+" en el intervalo del "+to_string(indMin)+" y "+to_string(indMax)<<endl;
}


int main(int argc, char* argv[]){
    VectInt v;  //vector de N enteros
    thread P[N_BUSC];   //procesos buscadores
    string nom;
    cout<<"Dime el nombre del fichero:";
    cin>>nom;
    cout<<endl;
    bool calculado[N_BUSC]={false};  //se guardaran las repeticiones de cada trozo en cada componente
    atomic_flag tas=ATOMIC_FLAG_INIT;  //se inicia la variable atomic
    bool leido=false;   //aun no se han leido los datos del fichero por eso se inicia a false
    int i=0;
    int tramo=N/N_BUSC; //dividimos el numero de datos para repartirlos en cada proceso
    int valor;
    int resultado,resGen;
    cout<<"Dime el valor a buscar 1 y 25:"<<endl;
    cin>>valor;
    while(0>valor || valor>25){     //hasta que se introduzcan los valores correctos
        cout<<"Valor incorrecto"<<endl;
        cin>>valor;
    }
    thread th_1(&coordinador,v,ref(leido),calculado,nom);   //se inician los procesos
    for(int i=0;i<N_BUSC;i++){
        P[i] = thread(&buscador,v,ref(leido),i,i*tramo,(i+1)*tramo,valor,calculado,ref(tas));
    }

    th_1.join();        //se bloquean los procesos
   for(int i=0;i<N_BUSC;i++){
        P[i].join();
    }
}
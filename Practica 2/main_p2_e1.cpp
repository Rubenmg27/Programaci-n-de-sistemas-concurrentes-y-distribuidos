//------------------------------------------------------------------------------
// File:   main_p2_e1.cpp
// Authors:Sergio Salesa Quintanilla NIP:811019
//         Rubén Martín González NIP:841886
// Date:   16 de septiembre de 2022
// Coms:   Parte del práctica 2 de PSCD
//         Compilar mediante
//           g++ main_p2_e1.cpp -o main_p2_e1 -std=c++11 -pthread
//------------------------------------------------------------------------------
#include <iostream>
#include <thread>
#include <string>
#include <fstream>
#include <cstdlib>

const int N = 512;    //# de datos
const int N_BUSC = 8; //# de procesos buscadores


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
    else{
        cerr<<"No se ha podido abrir el fichero"<<endl;   //si no se a podido escribir devuelve este mensaje
    }
}

//-----------------------------------------------------
//Pre: 0 ≤ i ≤ d ≤ N − 1
//Post: result = N um α ∈ [i, d].value = v[α]
void search(const VectInt v,const int i, const int d,const int value, int& result){
    result=0;
    for(int j=i;j<d;j++){
        if(v[j]==value){     //si coincide con el valor aumentamos en 1 el resultado
            result++;
        }
    }
}


//Pre: VectInt contiene los enteros, noCargado sera true si VecInt esta cargado, i y d contienen los indices del trozo a buscar, value el valor a buscar. Los vectores
//     veces y noCalculado no tendran valor y estarán a true en todos sus componentes respectivamente. 
//Post: Almacena en veces el número de veces que se repite el valor en ese tramo. A su vez noCalculado se pondrá a false. 
void buscador(const VectInt v,bool& leido,int ind,const int i, const int d,const int value,int& result,bool calculado[]){
    while(!leido){
        std::this_thread::yield();     //pasa el turno a otro proceso
    }
    search(v,i,d,value,result);         
    calculado[ind]=true;              //una vez obtenido el resultado del proceso i se pone a true el vector de booleanos
}

//Pre: VectInt es un vector vacio, noCargado es true. 
//Post: Carga los datos en VecInt, noCargado devuelve false y devuelve en resultado el número de veces que se ha repetido el número elegido.
void coordinador(VectInt v,bool& leido,bool calculado[],int veces[],string nom){
    meterDatos(v,nom);        
    leido=true;  //ya estan cargados los datos
    int repeticiones=0;           
    for(int i=0;i<N_BUSC;i++){
        while(!calculado[i]){ 
            std::this_thread::yield();    //pasa el turno hasta que se calculen todos los procesos buscadores
        }
    }
    for(int j=0;j<N_BUSC;j++){
        repeticiones=repeticiones+veces[j];  //una vez calculados se suman en una variable
    }
    cout<<"Aparece "+to_string(repeticiones)+" veces"<<endl;
}


int main(int argc, char* argv[]){
    VectInt v;              //vector de N enteros
    thread P[N_BUSC];       //procesos buscadores
    string nom;
    cout<<"Dime el nombre del fichero:";
    cin>>nom;
    cout<<endl;
    int veces[N_BUSC]={0};  //se guardaran las repeticiones de cada trozo en cada componente
    bool calculado[N_BUSC]={false}; //se inicializan en falso 
    bool leido=false;   //aun no se han leido los datos del fichero por eso se inicia a false
    int i=0;      
    int tramo=N/N_BUSC; //dividimos el numero de datos para repartirlos en cada proceso
    int valor;
    cout<<"Dime el valor a buscar 1 y 25:"<<endl;
    cin>>valor;
    while(0>valor || valor>25){        //hasta que se introduzcan los valores correctos
        cout<<"Valor incorrecto"<<endl;
        cin>>valor;
    }
   thread th_1(&coordinador,v,ref(leido),calculado,veces,nom);  //se inician los procesos
   for(int i=0;i<N_BUSC;i++){
        P[i] = thread(&buscador,v,ref(leido),i,i*tramo,(i+1)*tramo,valor,ref(veces[i]),calculado);
    }

    th_1.join();
    for(int i=0;i<N_BUSC;i++){  //se bloquean los procesos
        P[i].join();
    }
    return 0;
}
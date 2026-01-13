//------------------------------------------------------------------------------
// File:   ejercicio_3.cpp
// Authors:Sergio Salesa Quintanilla NIP:811019
//         Rubén Martín González NIP:841886
// Date:   29 de septiembre de 2022
// Coms:   Parte del práctica 1 de PSCD
//         Compilar mediante
//           g++ ejercicio_3.cpp -o ejercicio_3 -std=c++11 -pthread
//------------------------------------------------------------------------------


#include <iostream>
#include <thread>
#include <string>
#include <chrono>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <iomanip>

using namespace std;

const int N=100;

//Pre: V es un vector de reales con N elementos.
//Post: calcula la media de V y cambia terminado a true.
void calcularMedia(double V[],int N,double& media,bool& terminado){
    for(int i=0;i<N;i++){
        media+=V[i];
    }
    media=media/N;
    terminado=true;
}


//Pre:V es un vector de reales con N elementos.
//Post:guarda en max el real de mas valor y en min el de menos valor de V.
void maxmin(double V[],const int N,double& max,double& min){
    max=V[0];
    min=V[0];
    for(int i=1;i<N;i++){
        if(V[i]>max){
            max=V[i];
        }
        if(V[i]<min){
            min=V[i];
        }
    }
}

//Pre:V es un vector de reales con N elementos, media es la media del vector y terminado vale true si la media esta calculada.
//Post:guarda en sigma la desviación típica de V.
void sigma(double V[],int N,double& sigma,double& media,bool& terminado){
    while(!terminado){   //Hasta que no se haya calculado la media en su totalidad, no podra acceder a su SC.
    }
    for(int i=0;i<N;i++){
        sigma+=pow(V[i]-media,2);
    }
    sigma=sqrt(sigma/(N-1));
}

int main(int argc, char* argv[]){
    thread P[3]; 
    bool terminado=false;
    double media=0.0;
    double maxim=0.0;
    double minim=0.0;
    double sigmav=0.0;
    double V[N]={};
    srand(time(nullptr));

    for(int i=0;i<N;i++){
        V[i]=double(rand());
    }


    P[0]=thread(&calcularMedia,V,N,ref(media),ref(terminado));
    P[1]=thread(&maxmin,V,N,ref(maxim),ref(minim));
    P[2]=thread(&sigma,V,N,ref(sigmav),ref(media),ref(terminado));

    for(int i=0;i<3;i++){
        P[i].join();
    }


    cout<<"#datos: "<<setprecision(10)<<N<<endl;

    cout<<"media: "<<setprecision(10)<<media<<endl;

    cout<<"máximo: "<<setprecision(10)<<maxim<<endl;

    cout<<"mínimo: "<<setprecision(10)<<minim<<endl;

    cout<<"sigma: "<<setprecision(10)<<sigmav<<endl;



    return 0;
}


//******************************************************************
// File:   Coodinador.hpp
// Author: Sergio Salesa Quintanilla  NIP: 811019
// Author: Rubén Martín González      NIP: 841886
// Date:   20 Diciembre de 2022
// Coms:   Fichero .cpp del monitor utilizado en la práctica 5 para solucionarla
//*****************************************************************

#include "Socket.hpp"
#include "ControlParejas.hpp"
#include <iostream>
#include <thread>
#include <vector>
#include <sstream>
#include <vector>
#include <iomanip>

using namespace std;

//-------------------------------------------------------------
// Pre : <s > contiene un string con 1 o m  ́a s campos , separados por el
// car  ́a cter en <sep >
// Post : devuelve un vector cuyas componentes son los valores que
// estaban separados por <sep > en <s > , por orden
// Com : Mirar la documentaci  ́o n sobre la clase ‘‘ vector ’’
vector<string> split( const string & s , char sep){
    vector<string> elems;
    string el;
    istringstream seqEls(s);
    while ( getline(seqEls,el,sep)){
        elems.push_back(el);
    }
    return elems ;
}

//-------------------------------------------------------------
/*
    Cada estudiante que se conecta al servidor se le junta con un representante único con el que se comunica
    y comparten datos. También es el encargado de gestionar cada estudiante representado.
*/
void RepresentanteEstudiantes(Socket& chan, int client_fd,controlParejas &c){

    vector<string> recibidos;
    //recibimos el mensaje del cliente
    /*
    <await libre>0
		libre--;
		examinado[libre]=nip;
	>
    */
    string buffer;
    string orden;
    bool fin=false;
    int valor,nip,nipPareja,filaAProcesar;;
    int lenght=100;;
    while(!fin){
        int rcv_bytes = chan.Receive(client_fd,buffer,lenght);
        //Si falla al recibir
        if (rcv_bytes == -1) {
            cerr << chan.error("Error al recibir datos",errno);
            // Cerramos los sockets
            chan.Close(client_fd);
        }
        //troceamos string hasta la coma
        recibidos=split(buffer,',');
        orden = recibidos[0];
        valor= stoi(recibidos[1]);

        if(orden=="sentar"){
            nip=valor;
            //hay que mirar que se pueda sentar
            c.esperarlibre(nip);
            c.parejacreada(nip,nipPareja,filaAProcesar);
            string s = to_string(nipPareja)+","+to_string(filaAProcesar);
            //Enviamos el mensaje al Estudiante
            /*
            <await parejas[nip]!=-1>          esperar me sea asignada pareja y fila    
            */
            int send_bytes = chan.Send(client_fd,s);
            //Error al enviar
            if(send_bytes == -1) {
                cerr << chan.error("Error al enviar datos",errno);
                // Cerramos los sockets
                chan.Close(client_fd);
                exit(1);
            }
        }
        else if(orden=="max"){
            //<fin[pareja[nip]]=true;>              hacerselo llegar a mi pareja
            int max = valor;
            c.pasarmax(nip,filaAProcesar,max);
            fin=true;
        }else if(orden=="sum"){
           int sum =valor;
            int max;
            /*
            <await fin[nip]                                 
                filaExam++;                 // comunicar finalizacion
            >            
            */      
            c.recibirmax(nip,filaAProcesar,sum,max);
            //mismo que con max;
            cout<<to_string(filaAProcesar)+"|"+to_string(nipPareja)+"-"+to_string(nip)+"|"+to_string(max)+"|"+to_string(sum)+"\n";
            fin=true;
            c.comunicarfinest();
        }
    }
    chan.Close(client_fd);   
}

//-------------------------------------------------------------
/*
Proceso Profesor encargado de emparejar a los estudiantes
*/
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


//-------------------------------------------------------------
int main(int argc,char* argv[]) {
    const int N = 4;
    controlParejas c;
    // Puerto donde escucha el proceso servidor
    int SERVER_PORT = atoi(argv[1]); //normalmente será un parámetro de invocación. P.e.: argv[1]
    
    vector<thread> estudiante;
    vector<int> client_fds;

    // Creación del socket con el que se llevará a cabo
    // la comunicación con el servidor.
    Socket chan(SERVER_PORT);

    // bind
    int socket_fd = chan.Bind();
    if (socket_fd == -1) {
        cerr << chan.error("Error en el bind",errno);
        exit(1);
    }

    //lisen
    int error_code = chan.Listen();
    if (error_code == -1) {
        cerr << chan.error("Error en el listen",errno);
        // Cerramos el socket
        chan.Close();
        exit(1);
    }
    thread th_1(&Profesor,ref(c));   //se inician los procesos
    int i = 0;
    while (i<N_EST) {
    	i++;
        // Accept
        int new_client_fds = chan.Accept();

        if(new_client_fds == -1) {
            cerr << chan.error("Error en el accept",errno);
            // Cerramos el socket
            chan.Close();
            exit(1);
        } 
        else {
            //introducir en el vector el cliente y arrancar el thread
            client_fds.push_back(new_client_fds);
            estudiante.push_back(thread(&RepresentanteEstudiantes, ref(chan), new_client_fds,ref(c)));
        }
    }
   


    //¿Qué pasa si algún thread acaba inesperadamente?
    for (int i=0; i<estudiante.size(); i++) {
        estudiante[i].join();
    }

    th_1.join(); 

    // Cerramos el socket del servidor
    error_code = chan.Close();
    if (error_code == -1) {
        cerr << chan.error("Error cerrando el socket del servidor",errno);
    }

    return error_code;
}
//-------------------------------------------------------------

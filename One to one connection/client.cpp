#include <iostream>
#include <winsock.h>
using namespace std;
#define PORT 9909

int nclientsock;
struct sockaddr_in srv;

int main(){
    int nret = 0;
    // WSA initialize
    WSADATA ws;
    if (WSAStartup(MAKEWORD(2, 2), &ws) < 0)
    {
        cout << "WSA failed in intitalize..." << endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Initialize socket
    nclientsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(nclientsock < 0){
        cout << endl << "Socket call failed...";
        WSACleanup();
        return (EXIT_FAILURE);
    }

    // Initialize the sockaddr environment
    // in case of server, in srv.sin_addr.s_addr, we initialize it INADDR_ANY, as there we have to connect to local port
    // But in case of client, we cannot initialize it with INADDR_ANY, we have to initialize it with inet_addr(), and have to provide the address.
    // Here in inet_addr(), we have to provide the IP address of the machine.
    // inet_addr() => it is used to convert it from host byte order to network byte order.
    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT);
    srv.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(&srv.sin_zero, 0, 8);

    // Connect the network
    // Connect() => 
    nret = connect(nclientsock, (struct sockaddr*)&srv, sizeof(srv));

    if(nret < 0){
        cout << endl << "connect failed...";
        WSACleanup();
        return (EXIT_FAILURE);        
    }    
    else{
        cout << endl << "Connected to the Server...";
        char buffer[255] = { 0, };
        // Recieve from the server
        recv(nclientsock, buffer, 255, 0);
        cout << endl << "Press Enter to see the message.";
        getchar();
        cout<<"\n Server Message => " << buffer; 
        recv(nclientsock, buffer, 255, 0);
        cout << endl << "No of client => " << buffer;
        cout << endl << " Send Message to server : ";
        while(1){
            fgets(buffer, 256, stdin);
            send(nclientsock, buffer, 256, 0);
            cout <<"\n------------------------------------------------------------------------------------\n";
            recv(nclientsock, buffer, 256, 0);
            cout << endl << " Server Message => " << buffer <<endl << " Enter to Send Message : ";
        }
    }
}

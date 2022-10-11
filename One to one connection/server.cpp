#include <iostream>
#include <string.h>
// #include <bits/stdc++.h>
#include <winsock.h>
// #include <sys/socket.h>
#define PORT 9909
using namespace std;

// g++ server.cpp -o server -lws2_32

struct sockaddr_in srv;
fd_set fr, fw, fe;
int nsocket;
int nArrayclient[5];      // array to hold the client client_socket_id;

void newMeassage(int nClientSocket){
    cout << endl << "Processing the new message for client socket "<<nClientSocket;
    char buff[256+1] = {0,};
    int nret = recv(nClientSocket, buff, 256, 0);
    if(nret < 0){
        cout << "\nSomething wrong happen, closing the connection for client.";
        closesocket(nsocket);
        for(int index=0; index < 5; index++){
            if(nArrayclient[index] == nClientSocket){
                nArrayclient[index] = 0;
                break;
            }
        }
    }
    else{
        cout << endl << "Message received from client => " << buff;
        cout << endl << "Send message to client : ";
        fgets(buff, 256, stdin);
        send(nClientSocket, buff, 256, 0);
        cout<<"\n----------------------------------------------------------------\n";
    }
}

void process_new_req(){
    // New Connection request
    // cout<<"called\n";
    if( FD_ISSET(nsocket, &fr) ){
        // if this, my listner socket is ready to read.
        // SOCKET accept(SOCKET s, sockaddr *addr, int *addrlen)
        int nlen = sizeof(struct sockaddr);
        int nclientsocket = accept(nsocket, NULL, &nlen);
        
        // nsocket => this socket is listner socket, which is a channel where you receive the new request from the client but here you can't communicate with the client.
        // nclientsocket => this socket is to communicate with the client.
        // this accept() will return a new socket ID, which is the client socket id.
        if( nclientsocket > 0 ){
            // Put it into the client fd_set.
            int index = 0;
            int count = 34;
            for(index = 0; index < 5; index++){
                if(nArrayclient[index] == 0){
                    nArrayclient[index] = nclientsocket;
                    send(nclientsocket, "Got the connection", 19, 0);
                    break;
                }
            }
            if(index == 5){
                cout << endl << "No space for new connection...";
            }
        }
    }
    else{
         //   cout<<"Yes\n";
            for(int index = 0; index < 5; index++){
                if(FD_ISSET(nArrayclient[index], &fr)){
                    // Got the new message from the client
                    // Just recv new message
                    // Just queue that for new worker of your server to fulfill the request
                    newMeassage(nArrayclient[index]);
                }
            }
        }
}

int main()
{
    int nret = 0;

    // STEPS :
    // WSA Initialize => needed in window to rum the socket programming files
    WSADATA ws;
    if (WSAStartup(MAKEWORD(2, 2), &ws) < 0)
    {
        cout << "WSA failed in intitalize..." << endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else
    {
        cout << "WSA successfully initialize..." << endl;
    }

    // 1. Initialize the Socket
    nsocket = socket(AF_INET, SOCK_STREAM, 0);
    if (nsocket < 0)
    {
        cout << endl
             << "Socket is not Opened...";
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else
    {
        cout << endl
             << "Socket Opened Successfully...";
    }

    // Initialize the environment for sockaddr structure
    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT);       // host to network sort
    srv.sin_addr.s_addr = INADDR_ANY; // this will assign the address of local machine. IP address of local machine
    // srv.sin_addr.s_addr = inet_addr("127.0.0.1");     // also the way to do this
    memset(&(srv.sin_zero), 0, 8);

    // To ALLOW OTHER SOCKET TO USE SAME FULLY OCCUPIED ADDRESS.
// setsocketopt()
// it means one socket can use one network address, one port, one protocol, other socket cannot use it.
// if some socket is bind to fully occupied address (port,address,protocol), then other socket cannot bind to it.
// int setsockopt(SOCKET s, int level, int optname, const char *optval, int optlen)
    int noptval = 0;
    int noptlen = sizeof(noptval);
    nret = setsockopt(nsocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&noptval, noptlen);

// SO_REUSEADDR => it will allow the other socket to use the same fully occupied address.

    if( nret == 0 ){
        cout << endl << "The Socket opt call successfully.";
    }
    else{
        cout << endl << "The Socketopt call is not success.";
        WSACleanup();
        exit(EXIT_FAILURE);
    }



    // Bind the socket to the local port
    // int bind(SOCKET s, const sockaddr *addr, int namelen)
    nret = bind(nsocket, (sockaddr *)&srv, sizeof(sockaddr));
    if (nret < 0)
    {
        cout << endl
             << "Fail to bind to local port...";
             WSACleanup();
        exit(EXIT_FAILURE);
    }
    else
    {
        cout << endl
             << "Bind successfully to local port...";
    }

    // Listen the request from the client (queue the requests)
    // int listen(SOCKET s, int backlog)
    nret = listen(nsocket, 5);
    if (nret < 0)
    {
        cout << endl
             << "Failed to start listen at local port...";
             WSACleanup();
        exit(EXIT_FAILURE);
    }
    else
    {
        cout << endl
             << "Started listening to local port...";
    }

    // Keep waiting for new requests and proceed as per the request

    int nMax_fd = nsocket;
    struct timeval tv;
    // Here is have to provide the timeval, how many time we need to wait to see if anyone is ready.
    tv.tv_sec = 1;  // wait for 1 second.
    tv.tv_usec = 0; // don't need to provide any milliseconds.

    while (1)
    {
        FD_ZERO(&fr);
        FD_ZERO(&fw);
        FD_ZERO(&fe);

        // Before this select call, we need to set socket descriptor.
        FD_SET(nsocket, &fr);
        FD_SET(nsocket, &fe);

        for(int index=0; index < 5; index++){
            if(nArrayclient[index] != 0){
                FD_SET(nArrayclient[index], &fr);
                FD_SET(nArrayclient[index], &fe);
            }
        }


        // cout << "\nBefore => " << fr.fd_count << endl;
        // int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, const timeval *timeout)
        nret = select(nMax_fd + 1, &fr, &fw, &fe, &tv);
        // cout << "\nAfter => " << fr.fd_count << endl;

        if (nret > 0)
        {
            // containing the number of sockets are ready to connect / communicate.
            cout << endl << "Data on port... Processing now...";
            process_new_req();
            // break;
            // Process the request
            // Now i have to check on which fd_set request is available, is it on fr, or on fw, or etc..
            // So here we use a MACRO  FD_ISSET(), which helps to find if there is some request available at particular fd_set
            // if(FD_ISSET(nsocket, &fe)){
            //     cout << endl << "there is an exception.";
            // }
            // if(FD_ISSET(nsocket, &fw)){
            //     cout << endl << "Ready to write something.";
            // }
            // if(FD_ISSET(nsocket, &fr)){
            //     cout << endl << "Ready to read.";
            // }
            // break;
        }
        else if (nret == 0)
        {
            // no connection / non of the socket descriptor are ready.
            // cout << endl
                //  << "Nothing on Port.";
        }
        else
        {
            // It failed, your application should show some useful message.
            cout << "\n Failed";
            WSACleanup();
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}

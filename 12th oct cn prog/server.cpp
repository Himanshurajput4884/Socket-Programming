#include <iostream>
#include <vector>
#include <string.h>
#include <winsock.h>
#include <map>
#define PORT 9909
using namespace std;

// g++ server.cpp -o server -lws2_32

struct sockaddr_in srv;
fd_set fr, fw, fe;
int nsocket;
int nArrayclient[5]; // array to hold the client client_socket_id;

int ciph = 3;
vector<char> smal(26), large(26);

void encrypt(char buff[])
{
    int n = strlen(buff);
    for (int i = 0; i < n; i++)
    {
        if (buff[i] == ' ')
        {
            continue;
        }
        if (buff[i] >= 'a' && buff[i] <= 'z')
        {
            buff[i] = smal[(buff[i] - 'a' + ciph) % 26];
        }
        else if (buff[i] >= 'A' && buff[i] <= 'Z')
        {
            buff[i] = large[(buff[i] - 'A' + ciph) % 26];
        }
    }
}

void decrypt(char buff[])
{
    int n = strlen(buff);
    for (int i = 0; i < n; i++)
    {
        if (buff[i] == ' ')
        {
            continue;
        }
        if (buff[i] >= 'a' && buff[i] <= 'z')
        {
            int index = (buff[i] - 'a' - ciph);
            if (index < 0)
            {
                index += 26;
            }
            buff[i] = smal[index];
        }
        else if (buff[i] >= 'A' && buff[i] <= 'Z')
        {
            int index = (buff[i] - 'A' - ciph);
            if (index < 0)
            {
                index += 26;
            }
            buff[i] = large[index];
        }
    }
}

void newMeassage(int nClientSocket)
{
    cout << endl
         << "Processing the new message for client socket " << nClientSocket;
    char buff[256 + 1] = {
        0,
    };
    int nret = recv(nClientSocket, buff, 256, 0);
    if (nret < 0)
    {
        cout << "\nSomething wrong happen, closing the connection for client.";
        closesocket(nsocket);
        for (int index = 0; index < 5; index++)
        {
            if (nArrayclient[index] == nClientSocket)
            {
                nArrayclient[index] = 0;
                break;
            }
        }
    }
    else
    {
        if(strncmp(buff,"bye",3)==0)
		{
			cout << "Disconnected\n";
            closesocket(nsocket);
            return;
		}
        // cout << endl
            //  << "Encrypted message received from client => " << buff;
        // cout << "Encrypted : " << buff;
        // decrypt(buff);
        // cout << endl
        //      << "Decrypted message =>  " << buff;
        cout << "\nMessage from client =>  " << buff ;
        int n = strlen(buff);
        bool intt = false, decimal = false, alpha = false, special = false;
        for(int i=0; i<n; i++){
            if(buff[i] >= '0' && buff[i] <= '9'){
                int j = i;
                while(j < n && (buff[j] >= '0' && buff[j] <= '9') && buff[j] != '.'){
                    j++;
                }
                if(buff[j] == '.'){
                    decimal = true;
                }
                else{
                    intt = true;
                }
            }
            if((buff[i]>='a' && buff[i]<='z') || (buff[i]>='A' && buff[i]<='Z')){
                alpha = true;
            }
            if(((buff[i]>=33 && buff[i]<=47) && buff[i]!=46) || (buff[i]>=58 && buff[i]<=64)){
                special = true;
            }
        }
        string respond;
        if(special){
            respond = "Message contain Special Character.";
        }
        else if(alpha){
            if(decimal || intt){
                respond = "Message contain Alphanumeric Character.\n";
            }
            else {
                respond= "Message contain Alphabet only.\n";
            }
        }
        else if(decimal){
            respond = "Message contain Decimal Number.\n";
        }
        else if(intt){
            respond = "Message contain Numbers.\n";
        }
        // cout << endl
            //  << "Send message to client : ";
        // fgets(buff, 256, stdin);
        // encrypt(buff);
        int i=0;
        for(i=0; i<respond.size(); i++){
            buff[i] = respond[i];
        }
        buff[i] = '\0';
        send(nClientSocket, buff, 256, 0);
        cout << "\n----------------------------------------------------------------\n";
    }
}

void process_new_req()
{
    if (FD_ISSET(nsocket, &fr))
    {
        int nlen = sizeof(struct sockaddr);
        int nclientsocket = accept(nsocket, NULL, &nlen);
        if (nclientsocket > 0)
        {
            // Put it into the client fd_set.
            int index = 0;
            for (index = 0; index < 5; index++)
            {
                if (nArrayclient[index] == 0)
                {
                    nArrayclient[index] = nclientsocket;
                    send(nclientsocket, "Got the connection", 18, 0);
                    break;
                }
            }
            if (index == 5)
            {
                cout << endl
                     << "No space for new connection...";
            }
        }
    }
    else
    {
        for (int index = 0; index < 5; index++)
        {
            if (FD_ISSET(nArrayclient[index], &fr))
            {
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

    for (int i = 0; i < 26; i++)
    {
        smal[i] = 'a' + i;
        large[i] = 'A' + i;
    }
    // Initialize the environment for sockaddr structure
    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT);       // host to network sort
    srv.sin_addr.s_addr = INADDR_ANY; // this will assign the address of local machine. IP address of local machine
    // srv.sin_addr.s_addr = inet_addr("127.0.0.1");     // also the way to do this
    memset(&(srv.sin_zero), 0, 8);

    int noptval = 0;
    int noptlen = sizeof(noptval);
    nret = setsockopt(nsocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&noptval, noptlen);

    if (nret == 0)
    {
        cout << endl
             << "The Socket opt call successfully.";
    }
    else
    {
        cout << endl
             << "The Socketopt call is not success.";
        WSACleanup();
        exit(EXIT_FAILURE);
    }

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

    int nMax_fd = nsocket;
    struct timeval tv;

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

        for (int index = 0; index < 5; index++)
        {
            if (nArrayclient[index] != 0)
            {
                FD_SET(nArrayclient[index], &fr);
                FD_SET(nArrayclient[index], &fe);
            }
        }

        nret = select(nMax_fd + 1, &fr, &fw, &fe, &tv);
        if (nret > 0)
        {
            cout << endl
                 << "Data on port... Processing now...";
            process_new_req();
        }
        else if (nret == 0)
        {
            // no connection / non of the socket descriptor are ready.
            // cout << endl
            //  << "Nothing on Port.";
        }
        else
        {
            cout << "\n Failed";
            WSACleanup();
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}
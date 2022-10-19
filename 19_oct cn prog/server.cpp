#include <iostream>
#include <vector>
#include <string.h>
#include <winsock.h>
#define PORT 9909
using namespace std;

// g++ server.cpp -o server -lws2_32
int key = 3;
string decrypt(char s[]){
    int n = strlen(s);
	vector<vector<char>> data(key, vector<char>(n, ' '));
	int i=0, j=0, k=0;
	bool f = true;
	while(j!=n){
        data[i][j] = '*';
        k++;
        if(f){
        	i++;
        	if(i==key){
        		f=false;
        		i=key-2;
			}
		}
		else{
			i--;
			if(i<0){
				f=true;
				i=1;
			}
		}
        j++;
    }
    k=0;
    for(int i=0; i<key; i++){
    	for(int j=0; j<n; j++){
    		if(data[i][j] == '*' && k<n){
    			data[i][j] = s[k];
    			k++;
			}	
		}
	}
	string res = "";
	i=0, k=0, j=0;
	f=true;
	while(j!=n){
        res += data[i][j];
        k++;
        if(f){
        	i++;
        	if(i==key){
        		f=false;
        		i=key-2;
			}
		}
		else{
			i--;
			if(i<0){
				f=true;
				i=1;
			}
		}
        j++;
    }
    // cout << "\n decrypted => ";
    // cout << res << endl;
	return res;
}


struct sockaddr_in srv;
fd_set fr, fw, fe;
int nsocket;
int nArrayclient[5]; // array to hold the client client_socket_id;

int convert(char buff[]){
    int t = 0;
    int n = strlen(buff);
    for(int i=0; i<n; i++){
        t = t*10 + (buff[i]-'0');
    }
    return t;
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
        // cout << endl << buff << endl;
        // key = convert(buff);
        // cout<<endl<<key;
        cout << endl << " Enter the key : ";
        cin>>key;
        cin.ignore();
        // memset(buff, ' ', sizeof(buff));
        // recv(nClientSocket, buff, 256, 0);
        cout << "\n Encrypted message receive from client => "<< buff;
        string res = decrypt(buff);
        // memset(buff, ' ', sizeof(buff));
        int k=0;
        for(int i=0; i<res.size(); i++){
            buff[k] = res[i];
            k++;
        }
        cout << "\n Decrypted Message => " << buff << endl;
        send(nClientSocket, buff, 256, 0);
        // fgets(buff, 256, stdin);
        // send(nClientSocket, buff, 256, 0);
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
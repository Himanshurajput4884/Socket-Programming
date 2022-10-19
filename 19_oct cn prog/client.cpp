#include <iostream>
#include <vector>
#include <algorithm>
#include <winsock.h>
using namespace std;
#define PORT 9909

int nclientsock;
struct sockaddr_in srv;
int key;

string encrypt(char s[])
{
    int n = strlen(s);
    vector<vector<char>> data(key, vector<char>(n, '@'));
    string buff = "";
    int i = 0, j = 0, k = 0;
    bool f = true;
    while (j != n)
    {
        data[i][j] = s[k];
        k++;
        if (f)
        {
            i++;
            if (i == key)
            {
                f = false;
                i = key - 2;
            }
        }
        else
        {
            i--;
            if (i < 0)
            {
                f = true;
                i = 1;
            }
        }
        j++;
    }
    // k = 0;
    for (int i = 0; i < key; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (data[i][j] != '@')
            {
                buff+=(data[i][j]);
                // k++;
            }
        }
    }
    return buff;
}

string convert(int k)
{
    string res = "";
    while (k)
    {
        res += (k % 10) + '0';
        k = k / 10;
    }
    reverse(res.begin(), res.end());
    return res;
}

int main()
{
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

    if (nclientsock < 0)
    {
        cout << endl
             << "Socket call failed...";
        WSACleanup();
        return (EXIT_FAILURE);
    }
    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT);
    srv.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(&srv.sin_zero, 0, 8);

    // Connect the network
    // Connect() =>
    nret = connect(nclientsock, (struct sockaddr *)&srv, sizeof(srv));
    if (nret < 0)
    {
        cout << endl
             << "connect failed...";
        WSACleanup();
        return (EXIT_FAILURE);
    }
    else
    {
        cout << endl
             << "Connected to the Server...";
        char buffer[255] = {
            0,
        };
        // Recieve from the server
        recv(nclientsock, buffer, 255, 0);
        cout << endl
             << "Press Enter to see the message.";
        getchar();
        cout << "\n Server Message => " << buffer;
        // cout << "\n Send a connection msg => ";
        // fgets(buffer, 256, stdin);
        // send(nclientsock, buffer, 256, 0);
        while (1)
        {
            cout << endl
                 << "Enter key : ";
            cin>>key;
            cin.ignore();
            // fgets(buffer, 256, stdin);
            
            // string kyy = convert(key);
            int k = 0;
            // memset(buffer, ' ', sizeof(buffer));
            // for (int i = 0; i < kyy.size(); i++)
            // {
            //     buffer[k] = kyy[i];
            //     k++;
            // }
            // send(nclientsock, buffer, 256, 0);
            // memset(buffer, ' ', sizeof(buffer));
            cout << endl << " Enter the message : ";
            fgets(buffer, 256, stdin);
            if (strncmp(buffer, "bye", 3) == 0)
            {
                cout << "Disconnected\n";
                break;
            }
            string s = encrypt(buffer);
            k = 0;
            // cout<<s<<endl;
            for (int i = 0; i < s.size(); i++)
            {
                buffer[k] = s[i];
                k++;
            }
            cout << "\n Encrypted : " << buffer;
            send(nclientsock, buffer, 256, 0);
            cin.ignore();
            recv(nclientsock, buffer, 256, 0);
            // cout << endl
            //  << "Encrypted message from server => " << buffer << endl;
            // decrypt(buffer);
            cout << "\n Decrypted message from server => " << buffer ;
            // cout << "\n Send Message to server.";
            cout << "\n------------------------------------------------------------------------------------\n";
        }
        closesocket(nclientsock);
    }
}
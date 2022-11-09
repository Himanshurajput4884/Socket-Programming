#include <iostream>
#include <vector>
#include <winsock.h>
using namespace std;
#define PORT 9909

int nclientsock;
struct sockaddr_in srv;
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
    // return buff;
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
    // return buff;
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

    for (int i = 0; i < 26; i++)
    {
        smal[i] = i + 'a';
        large[i] = i + 'A';
    }

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
             << " Connected to the Server...";
        char buffer[255] = {
            0,
        };
        // Recieve from the server
        recv(nclientsock, buffer, 255, 0);
        cout << endl
             << " Press Enter to see the message.";
        getchar();
        cout << "\n Server Message => " << buffer;
        cout << endl
             << "\n Send Message to server => ";
        while (1)
        {
            fgets(buffer, 256, stdin);
            if (strncmp(buffer, "bye", 3) == 0)
            {
                cout << "Disconnected\n";
                break;
            }
            encrypt(buffer);
            cout << "\n Encrypted message => " << buffer << endl;
            send(nclientsock, buffer, 256, 0);
            cout << "\n------------------------------------------------------------------------------------\n";
            recv(nclientsock, buffer, 256, 0);
            cout << endl
                 << " Encrypted message from server => " << buffer << endl;
            decrypt(buffer);
            cout << " Decrypted message from server => " << buffer << endl;
            cout << " Send Message to server => ";
        }
        closesocket(nclientsock);       
    }
}
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <winsock.h>
using namespace std;
#define PORT 9909

int nclientsock;
struct sockaddr_in srv;
string key;

string encrypt(char s[], vector<vector<char>> &data)
{
    int n = strlen(s)-1;
    int no = key.size();
    if(no < n){
        int i = no;
        int j=0;
        while(i!=n){
            key.push_back(key[j]);
            i++;
            j=(j+1)%no;
        }
    }
    // cout << key << endl;
    // cout << n <<endl;
    string res  ="";
    for(int i=0; i<n; i++){
        int i1 = s[i]-'A';
        int i2 = key[i]-'A';
        // cout << i1 << " " << i2 << endl;
        res+= data[i1][i2];
    }
    // cout << res << endl;
    return res;
}


void fill(vector<vector<char>> &data){
    vector<char> temp(26, 'A');
    for(int i=0; i<26; i++){
        temp[i] = 'A'+i;
    }
    for(int i=0; i<26; i++){
        int index = i;
        for(int j=0; j<26; j++){
            data[i][j] = temp[(index+j)%26];
        }
    }
    return;
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
    vector<vector<char>> data(26, vector<char>(26, 'A'));
    fill(data);
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
            getline(cin, key);
            // cin.ignore();
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
            // cout << buffer << endl;
            if (strncmp(buffer, "bye", 3) == 0)
            {
                cout << "Disconnected\n";
                break;
            }
            string s = encrypt(buffer, data);
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
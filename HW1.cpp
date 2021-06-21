/*
Student No.: 07xx3xx
Student Name: xxxxxxx
Email: b-----------7@nctu.edu.tw
SE tag: xnxcxtxuxoxsx
Statement: I am fully aware that this program is not
supposed to be posted to a public server, such as a
public GitHub repository or a public web page.
*/

#include<bits/stdc++.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
using namespace std;
pid_t pid;

int main()
{
    while(1){
        cout<<">";
        string s;
        getline(cin,s);
        stringstream ss;
        ss << s;
        char* argv[255];
        for(int i = 0; i < 255; i++)
            argv[i] = new char(255);
        int n = 0;
        while(ss >> argv[n]){
            n++;
            if(n >= 254)
                n--;
        }
        delete argv[n];
        argv[n] = NULL;



        int notwait = 0;
        if(argv[n-1][0] == '&'){
            notwait = 1;
            delete argv[n-1];
            argv[n-1] = NULL;
        }



        pid = fork();

        if(pid < 0){///fork error
            fprintf(stderr, "Fork Failed");
            exit(-1);
        }
        else if(pid == 0){///child
            if(notwait){
                pid = fork();
                if(pid < 0){
                    fprintf(stderr, "Fork Failed");
                    exit(-1);
                }
                else if(pid > 0)
                    exit(0);
            }
            if(execvp(argv[0], argv) < 0){
                fprintf(stderr, "Exec Failed");
                exit(0);
            }
        }
        else{///parent
            wait(NULL);
        }
    }
}




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
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
using namespace std;
pid_t pid;

int main()
{
    int dim, size;
    cout<<"Input the matrix dimension:";
    cin>>dim;
    size = dim * dim * sizeof(unsigned int);

    int shmid_a, shmid_b, shmid_c;
    unsigned int *shmaddr_a, *shmaddr_b, *shmaddr_c;

    shmid_a = shmget(IPC_PRIVATE, size, IPC_CREAT|0600);
    shmid_b = shmget(IPC_PRIVATE, size, IPC_CREAT|0600);
    shmid_c = shmget(IPC_PRIVATE, size, IPC_CREAT|0600);

    shmaddr_a = (unsigned int *) shmat(shmid_a, NULL, 0);
    shmaddr_b = (unsigned int *) shmat(shmid_b, NULL, 0);

    int idx = 0;
    for(int i = 0; i < dim; i++){
        for(int j = 0; j < dim; j++){
            shmaddr_a[i*dim+j] = idx;
            shmaddr_b[i*dim+j] = idx;
            idx++;
        }
    }

    for(int num = 1; num <= 16; num++){
        struct timeval start, end;
        gettimeofday(&start, 0);
        int id = 0;
        for(id = 0; id < num; id++){
            pid = fork();
            if(pid == 0)
                break;
        }

        if(pid < 0){///fork error
            fprintf(stderr, "Fork Failed");
            exit(-1);
        }
        else if(pid == 0){///child
            shmaddr_a = (unsigned int *) shmat(shmid_a, NULL, 0);
            shmaddr_b = (unsigned int *) shmat(shmid_b, NULL, 0);
            shmaddr_c = (unsigned int *) shmat(shmid_c, NULL, 0);

            int num_to_calculate = dim / num + (((dim%num) == 0) ? 0 : 1);
            int bottom = num_to_calculate * id, top = min (num_to_calculate * (id + 1), dim);
            for(int i = bottom; i < top;i++){
                for(int j = 0; j < dim;j++){
                    shmaddr_c[i*dim+j] = 0;
                    for(int k = 0; k < dim; k++){
                        shmaddr_c[i*dim+j] += shmaddr_a[i*dim+k] * shmaddr_b[k*dim+j];
                    }
                }
            }
            shmdt(shmaddr_a);
            shmdt(shmaddr_b);
            shmdt(shmaddr_c);
            return 0;
        }
        else{///parent
            shmaddr_c = (unsigned int *) shmat(shmid_c, NULL, 0);
            for(id = 0; id < num; id++)
                wait(NULL);
            gettimeofday(&end, 0);
            int sec = end.tv_sec-start.tv_sec;
            int usec = end.tv_usec-start.tv_usec;
            unsigned int checksum = 0;
            for(int i = 0; i < dim; i++){
                for(int j = 0; j < dim; j++){
                    checksum += shmaddr_c[i*dim+j];
                }
            }

            printf("Multiplying matrices using %d process", num);
            (num > 1) ? printf("es\n") : printf("\n");
            printf("Elapsed time: %f sec", sec+(usec/1000000.0));
            printf(", Checksum: %u\n",checksum);

            shmdt(shmaddr_a);
            shmdt(shmaddr_b);
            shmdt(shmaddr_c);
        }
    }
    shmctl(shmid_a, IPC_RMID, NULL);
    shmctl(shmid_b, IPC_RMID, NULL);
    shmctl(shmid_c, IPC_RMID, NULL);
}




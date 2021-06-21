/*
Student No.: 07xx3xx
Student Name: xxxxxxx
Email: b-----------7@nctu.edu.tw
SE tag: xnxcxtxuxoxsx
Statement: I am fully aware that this program is not
supposed to be posted to a public server, such as a
public GitHub repository or a public web page.
*/
#include <bits/stdc++.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>
#define MAX_THREAD_NUM 8
using namespace std;

typedef struct job{
    int type;  //0 for bottom bubble sort, 1 for higher merge
    int number;
} job;
sem_t job_mutex;
sem_t event;
sem_t semaphore[16]; // event for bottom notifing upper

int inteval[2][16] = {0};///record for the inteval of  job #k [l,r)
list <job> job_list;
vector <int>  input_data, working;
void* sub_thread(void *in);

int main()
{
    cout<<"Reading data from input.txt...\n\n";
    ifstream infile("input.txt");
    pthread_t thread[MAX_THREAD_NUM];
    
    int n = 0;
    infile>>n;
    input_data.resize(n);
    working.resize(n);
    for(int i = 0; i < n; i++){
        infile>>input_data[i];
    }
    sem_init(&job_mutex, 0, 1);
    sem_init(&event, 0, 0);
    inteval[0][1] = 0;
    inteval[1][1] = n;
    for(int i = 1; i < 8; i++){
        int l = inteval[0][i], m = (inteval[0][i] + inteval[1][i]) / 2, r = inteval[1][i];
        inteval[0][2*i]    = l;
        inteval[1][2*i]    = m;
        inteval[0][2*i+1]  = m;
        inteval[1][2*i+1]  = r;
    }

    for(int t_num = 1; t_num <= MAX_THREAD_NUM; t_num++){
        printf("\nParallel merge-sort using %d threads.\n", t_num);
        for(int i = 0; i < n; i++)
            working[i] = input_data[i];
        for(int i = 0; i < 16; i++)
            sem_init(&semaphore[i], 0, 0);
        struct timeval start, end;
        gettimeofday(&start, 0);
        pthread_create(&thread[t_num-1], NULL, sub_thread, NULL);


        for(int i = 8; i <= 15; i++){//dispatch bottom layer jobs
            job temp;
            temp.type = 0;
            temp.number = i;

            sem_wait(&job_mutex);
            job_list.push_back(temp);
            sem_post(&job_mutex);
            sem_post(&event);
        }
        for(int i = 2; i >= 0; i--){//dispatch higher layer jobs
            int l = 1 << i, r = 1 << (i+1) ;
            for(int j = l; j < r; j++){
                sem_wait(&semaphore[j*2]);
                sem_wait(&semaphore[j*2+1]);
                job temp;
                temp.type = 1;
                temp.number = j;

                sem_wait(&job_mutex);
                job_list.push_back(temp);
                sem_post(&job_mutex);
                sem_post(&event);
            }
        }
        sem_wait(&semaphore[1]);


        gettimeofday(&end, 0);
        string filename = "output_" ;
        filename += (char)'0' + t_num;
        filename +=  ".txt";
        printf("Using is_sorted to check the result is %d", is_sorted(working.begin(), working.end()));
        ofstream outfile(filename);
        for(int i = 0; i < n; i++){
            outfile<< working[i]<<" ";
        }
        int sec = end.tv_sec - start.tv_sec;
        int usec = end.tv_usec - start.tv_usec;
        printf("\nElapsed time: %f sec\n", sec+(usec/1000000.0));
    }
}


void* sub_thread(void *)
{
    while(1){
        sem_wait(&event);//waiting for a job comming
        sem_wait(&job_mutex);//request the mutex of job list
        job my_job = job_list.front();
        job_list.pop_front();
        sem_post(&job_mutex);

        int id = my_job.number;
        if(my_job.type == 1){//merge
            int l = inteval[0][id], m = (inteval[0][id] + inteval[1][id]) / 2, r = inteval[1][id];
            inplace_merge(working.begin() + l, working.begin() + m, working.begin() + r);
        }
        else if(my_job.type == 0){//bubble_sort
            int l = inteval[0][id], r = inteval[1][id];
            for(int i = l; i < r; i++)
                for(int j = l; j < r - i - 1 + l; j++)
                    if(working[j] > working[j+1])
                        swap(working[j], working[j+1]);
        }
        sem_post(&semaphore[id]);
    }
}




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
#define THREAD_NUM 16
using namespace std;

sem_t semaphore[2][THREAD_NUM];
int idd[2][THREAD_NUM] = {0};///record for the inteval of k-th thread [l,r)
vector <int> arr, arr2;

void* bottom(void *in);
void* high(void *in);
int myST_merge_sort(int l, int r, int layer);


int main()
{
    pthread_t thread[THREAD_NUM];
    int id[THREAD_NUM] = {0};
    for(int i = 2; i < THREAD_NUM; i++){
        id[i] = i;
        sem_init(&semaphore[0][i], 0, 0);
        sem_init(&semaphore[1][i], 0, 0);
        if(i <= 7)
            pthread_create(&thread[i], NULL, high, (void*) &id[i]);
        else
            pthread_create(&thread[i], NULL, bottom, (void*) &id[i]);
    }

    cout<<"Please enter the input file name: ";
    string file;
    cin>>file;
    ifstream infile(file);


    int n = 0;
    infile>>n;
    arr.resize(n);
    arr2.resize(n);
    for(int i = 0; i < n; i++){
        infile>>arr[i];
        arr2[i] = arr[i];
    }


    struct timeval start1, end1, start2, end2;
    gettimeofday(&start1, 0);
    idd[0][2] = 0;
    idd[1][2] = n / 2;
    idd[0][3] = n / 2;
    idd[1][3] = n;
    sem_post(&semaphore[0][2]);
    sem_post(&semaphore[0][3]);

    sem_wait(&semaphore[1][2]);
    sem_wait(&semaphore[1][3]);
    inplace_merge(arr.begin(), arr.begin() + n / 2, arr.end());
    gettimeofday(&end1, 0);


    gettimeofday(&start2, 0);
    myST_merge_sort(0, n, 1);
    gettimeofday(&end2, 0);


    ofstream outfileMT("output1.txt");
    ofstream outfileST("output2.txt");
    for(int i = 0; i < n; i++){
        outfileMT<< arr[i]<<" ";
        outfileST<<arr2[i]<<" ";
    }
    int sec1 = end1.tv_sec - start1.tv_sec;
    int usec1 = end1.tv_usec - start1.tv_usec;
    int sec2 = end2.tv_sec - start2.tv_sec;
    int usec2 = end2.tv_usec - start2.tv_usec;
    printf("\nParallel merge-sort using 16 threads.\nElapsed time: %f sec\n", sec1+(usec1/1000000.0));
    printf("\nMerge-sort using single thread.\nElapsed time: %f sec\n", sec2+(usec2/1000000.0));
/*
    bool ss = is_sorted(arr.begin(), arr.end());
    cout<<endl<<ss<<endl;
    ss = is_sorted(arr2.begin(), arr2.end());
    cout<<ss<<endl;*/
}

void* bottom(void *in)
{
    pthread_detach(pthread_self());
    int id = * (int *) in;
    sem_wait(&semaphore[0][id]);
    int l = idd[0][id], r = idd[1][id];
    for(int i = l; i < r; i++){
        for(int j = l; j < r - i - 1 + l; j++){
            if(arr[j] > arr[j+1]){
                swap(arr[j], arr[j+1]);
            }
        }
    }
    sem_post(&semaphore[1][id]);
}

void* high(void *in)
{
    pthread_detach(pthread_self());
    int id = * (int *) in;
    sem_wait(&semaphore[0][id]);

    int size = idd[1][id] - idd[0][id];
    int l = idd[0][id], m = (idd[0][id] + idd[1][id]) / 2, r = idd[1][id];
    idd[0][2*id]    = l;
    idd[1][2*id]    = m;
    idd[0][2*id+1]  = m;
    idd[1][2*id+1]  = r;
    sem_post(&semaphore[0][2*id]);
    sem_post(&semaphore[0][2*id+1]);

    sem_wait(&semaphore[1][2*id]);
    sem_wait(&semaphore[1][2*id+1]);
    inplace_merge(arr.begin() + l, arr.begin() + m, arr.begin() + r);
    sem_post(&semaphore[1][id]);
}


int myST_merge_sort(int l, int r, int layer)
{
    if(layer == 3){
        for(int i = l; i < r; i++){
            for(int j = l; j < r - i - 1 + l; j++){
                if(arr2[j] > arr2[j+1]){
                    swap(arr2[j], arr2[j+1]);
                }
            }
        }
    }
    else{
        int m = (l + r) / 2;
        myST_merge_sort(l, m, layer + 1);
        myST_merge_sort(m, r, layer + 1);
        inplace_merge(arr2.begin() + l, arr2.begin() + m, arr2.begin() + r);
    }
    return 0;
}



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
#define TEST_NUM 4
using namespace std;

struct LFU_node{
    int freq;
    set<int> num_set;//RBtree to record reference_sequence_num
};

class LFU{
public:
    LFU(int size){//constructor
        this->capacity = size;
        this->size = 0;
        this->minfreq = 0;
    }

    void insert(int n, int seq){
        if(this->minfreq != 1){//when the minfreq != 1, insert new node into list.
            struct LFU_node *new_node = new struct LFU_node;
            new_node->freq = 1;
            new_node->num_set.insert(seq);
            LFU_list.push_front(new_node);
        }
        else{//when the minfreq == 1, push into the first node
            struct LFU_node*ptr = LFU_list.front();
            ptr->num_set.insert(seq);
        }
        this->hash[n] = LFU_list.begin();
        this->minfreq = 1;
        this->size++;
        return;
    }

    void pop(){
        list<struct LFU_node*>::iterator it = LFU_list.begin();
        int out = *((LFU_list.front())->num_set.begin());
        LFU_list.front()->num_set.erase(out);
        out = hash_to_seq_inv[out];
        this->hash.erase(out);
        this->size--;
        if((*it)->num_set.size() == 0){
            LFU_list.pop_front();
            this->minfreq = LFU_list.front()->freq;
        }
        return;
    }

    bool acess(int n, int seq){
        unordered_map<int, list<struct LFU_node*>::iterator>::iterator it = this->hash.find(n);
        if(it == hash.end()){// LFU page miss
            if(this->size == this->capacity)//page is full
                pop();
            hash_to_seq[n] = seq;
            hash_to_seq_inv[seq] = n;
            insert(n, seq);
            this->minfreq = LFU_list.front()->freq;
            return false;
        }
        else{// LFU page hit, increase count.
            list<struct LFU_node*>::iterator itt = it->second;
            list<struct LFU_node*>::iterator temp = itt;
            temp++;
            
            struct LFU_node *ptr = *itt;
            struct LFU_node *ptr2 = *temp;
            
            vector<int>::iterator prev_pos;
            ptr->num_set.erase(hash_to_seq[n]);//remove element from hash table
            if(ptr->num_set.empty())
                LFU_list.erase(itt);
            
            hash_to_seq[n] = seq;
            hash_to_seq_inv[seq] = n;//update the reference_sequence_num

            if(temp == LFU_list.end()  || ptr2 -> freq > (ptr -> freq)+ 1 ){//no node has freq = (ptr -> freq)+ 1. new node!!
                struct LFU_node *new_node = new struct LFU_node;
                new_node->freq = (ptr -> freq)+ 1;
                new_node->num_set.insert(hash_to_seq[n]);
                LFU_list.insert(temp, new_node);
                temp--;
                this->hash[n] = temp;
            }
            else{//push into next node
                ptr2->num_set.insert(hash_to_seq[n]);
                this->hash[n] = temp;
            }
            this->minfreq = LFU_list.front()->freq;
            return true;
        }
    }

private:
    int size;
    int capacity;
    int minfreq;
    list<struct LFU_node*>  LFU_list;
    unordered_map<int, list<struct LFU_node*>::iterator > hash;
    unordered_map<int, int> hash_to_seq, hash_to_seq_inv;
};


class LRU{
public:
    LRU(int size){//constructor
        this->capacity = size;
        this->size = 0;
    }

    void insert(int n){
        LRU_list.push_front(n);
        this->size++;
    }

    bool acess(int n){
        unordered_map<int, list<int>::iterator >::iterator it = this->hash.find(n);
        if(it == hash.end()){// LRU page miss
            if(this->size == this->capacity){//page is full
                int temp = LRU_list.back();
                LRU_list.pop_back();
                hash.erase(temp);
                this->size--;
            }
            insert(n);
            this->hash[n] = LRU_list.begin();
            return false;
        }
        else{// LRU page hit
            list<int>::iterator itt = it->second;
            int temp = *itt;
            LRU_list.erase(itt);
            LRU_list.push_front(temp);
            this->hash[n] = LRU_list.begin();
            return true;
        }
    }
private:
    int size;
    int capacity;
    list<int>  LRU_list;
    unordered_map<int, list<int>::iterator > hash;
};



int frame_num[TEST_NUM] = {64, 128, 256, 512};

int main(int argc, char* argv[])
{
    printf("LFU policy:\nFrame\tHit\t\tMiss\t\tPage fault ratio\n");
    struct timeval start, end;
    gettimeofday(&start, 0);
    for(int i = 0; i < TEST_NUM; i++){
        int hit_num = 0, miss_num = 0;
        double page_fault_ratio;
        ifstream infile(argv[1]);
        LFU test(frame_num[i]);
        int n = 0, seq = 0;
        while(infile >> n){
            if(test.acess(n, seq))
                hit_num++;
            else
                miss_num++;
            seq++;
        }
        infile.close();
        page_fault_ratio = (double) miss_num / (hit_num+miss_num);
        printf("%d\t%d\t\t%d\t\t%.10f\n", frame_num[i], hit_num, miss_num, page_fault_ratio);
    }
    gettimeofday(&end, 0);
    int sec = end.tv_sec - start.tv_sec;
    int usec = end.tv_usec - start.tv_usec;
    printf("Total elapsed time %.4f sec\n\n", sec+(usec/1000000.0));


    printf("LRU policy:\nFrame\tHit\t\tMiss\t\tPage fault ratio\n");
    gettimeofday(&start, 0);
    for(int i = 0; i < TEST_NUM; i++){
        int hit_num = 0, miss_num = 0;
        double page_fault_ratio;
        ifstream infile(argv[1]);
        LRU test(frame_num[i]);
        int n = 0;
        while(infile >> n){
            if(test.acess(n))
                hit_num++;
            else
                miss_num++;
        }
        infile.close();
        page_fault_ratio = (double) miss_num / (hit_num+miss_num);
        printf("%d\t%d\t\t%d\t\t%.10f\n", frame_num[i], hit_num, miss_num, page_fault_ratio);
    }
    gettimeofday(&end, 0);
    sec = end.tv_sec - start.tv_sec;
    usec = end.tv_usec - start.tv_usec;
    printf("Total elapsed time %.4f sec\n", sec+(usec/1000000.0));
    return 0;
}
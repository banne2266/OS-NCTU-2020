/*
Student No.: 07xx3xx
Student Name: xxxxxxx
Email: b-----------7@nctu.edu.tw
SE tag: xnxcxtxuxoxsx
Statement: I am fully aware that this program is not
supposed to be posted to a public server, such as a
public GitHub repository or a public web page.
*/

#define FUSE_USE_VERSION 30
#include <fuse.h>
#include<bits/stdc++.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
using namespace std;

struct file_node{
        char name[100];
        int size;
        int type;
        int uid;
        int gid;
        int mtime;
        int access_mode;
        char *content;
};

vector<struct file_node> tar_file;
struct timeval mount_time;

int my_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
int my_getattr(const char *path, struct stat *st);
int my_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi);
bool is_in_path(const char *path, const char * name);
string get_end_name(const char * name);
int path_cmp(const char *path, const char * name);

static struct fuse_operations op;

int main(int argc, char *argv[])
{
        printf("program start\n");
        ifstream infile("test.tar");
        while(!infile.eof()){
                struct file_node temp;
                temp.size = 0;
                temp.mtime = 0;
                temp.uid = 0;
                temp.gid = 0;
                temp.type = 0;
                temp.access_mode = 0;

                char buffer[512+1];
                memset(buffer, 0, 512+1);
                for(int i = 0; i < 512; i++){
                        infile.get(buffer[i]);
                }
                if(buffer[0] == 0)
                        break;
                for(int i = 0; i < 100; i++){//read name
                        temp.name[i] = buffer[i];
                }
                for(int i = 0; i < 8; i++){//read access_mode
                        if(!isdigit(buffer[i+100]))
                                break;
                        temp.access_mode = temp.access_mode * 8 + (buffer[i+100] - '0');
                }
                for(int i = 0; i < 8; i++){//read uid
                        if(!isdigit(buffer[i+108]))
                                break;
                        temp.uid = temp.uid * 8 + (buffer[i+108] - '0');
                }
                for(int i = 0; i < 8; i++){//read gid
                        if(!isdigit(buffer[i+116]))
                                break;
                        temp.gid = temp.gid * 8 + (buffer[i+116] - '0');
                }
                for(int i = 0; i < 12; i++){//read size
                        if(!isdigit(buffer[i+124]))
                                break;
                        temp.size = temp.size * 8 + (buffer[i+124] - '0');
                }
                for(int i = 0; i < 12; i++){//read modify time
                        if(!isdigit(buffer[i+136]))
                                break;
                        temp.mtime = temp.mtime * 8 + (buffer[i+136] - '0');
                }
                temp.type = buffer[156] - '0';//read type(0 for file, 5 for directory)

                
                int iter = temp.size / 512;//calculate how many blocks to read content
                if(temp.size % 512 != 0)
                        iter++;
                int ti = 0;
                temp.content = new char[iter * 512 +1];
                memset(temp.content, 0, iter * 512 +1);

                while(iter--){
                        memset(buffer, 0, 512+1);
                        for(int i = 0; i < 512; i++){
                                infile.get(buffer[i]);
                                temp.content[ti * 512 + i] = buffer[i];
                        }
                        ti++;
                }
 
                for(vector<struct file_node>::iterator it = tar_file.begin(); it < tar_file.end(); it++){
                        if(strcmp(it->name, temp.name) == 0 && it->mtime < temp.mtime){
                                tar_file.erase(it);
                        }
                }
                tar_file.push_back(temp);
                printf("Insert file into vector, name:%s, type: %d\n", tar_file.back().name, tar_file.back().type);
        }
        memset(&op, 0, sizeof(op));
        op.getattr = my_getattr;
        op.readdir = my_readdir;
        op.read = my_read;
        printf("fuse start\n");
        gettimeofday(&mount_time, 0);
        return fuse_main(argc, argv, &op, NULL);
}

int my_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) 
{ 
        /*do something*/ 
        printf("My_readdir\n");
        printf("\tpath: %s\n", path);
        filler(buffer, ".", NULL, 0);
        filler(buffer, "..", NULL, 0);
        for(int i = 0; i < tar_file.size(); i++){
                if(is_in_path(path+1, tar_file[i].name)){
                        filler(buffer, get_end_name(tar_file[i].name).c_str(), NULL, 0);
                }
        }
        return 0;
}

int my_getattr(const char *path, struct stat *st) 
{
        /*do something*/ 
        printf("My_getattr\n");
        printf("\tpath: %s\n", path);
        if(strcmp(path, "/") == 0){
                st->st_uid = getuid();
                st->st_gid = getgid();
                st->st_size = 0;
                st->st_mode =  S_IFDIR | 0444;
                st->st_mtim.tv_sec = mount_time.tv_sec;
                return 0;
        }
        for(int i = 0; i < tar_file.size(); i++){
                if(path_cmp(path, tar_file[i].name) == 0){
                        st->st_uid = tar_file[i].uid;
                        st->st_gid = tar_file[i].gid;
                        st->st_mtim.tv_sec = tar_file[i].mtime;
                        if(tar_file[i].type == 0){//regular file
                                st->st_mode = S_IFREG | tar_file[i].access_mode;
                                st->st_size = tar_file[i].size;
                        }
                        else if(tar_file[i].type >= 5){//diractory file
                               st->st_mode =  S_IFDIR | tar_file[i].access_mode;
                               st->st_size = 0;
                        }
                        return 0;
                }
        }
        return -ENOENT;
}

int my_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi) 
{
        /*do something*/ 
        printf("My_read\n");
        
        memset(buffer, 0, size+1);
        for(int i = 0; i < tar_file.size(); i++){
                if(path_cmp(path, tar_file[i].name) == 0){
                        int len = tar_file[i].size;
                        if(offset >= len)
                                return 0;
                        if(offset + size > len){
                                for(int j = 0; j < len - offset; j++){
                                        buffer[j] = tar_file[i].content[j+offset];
                                }
                                return (len - offset);
                        }
                        else{
                                for(int j = 0; j < size; j++){
                                        buffer[j] = tar_file[i].content[j+offset];
                                }
                                return size;
                        }
                }
        }
        return -ENOENT;
}

bool is_in_path(const char *path, const char * name)
{
        string a(path);
        string b(name);
        if(b.back() == '/')
                b.pop_back();

        if(a.length() >= b.length())
                return false;
        for(int i = 0; i < a.length(); i++){
                if(a[i] != b[i])
                        return false;
        }
        if(b[a.length()] != '/' && a.length() != 0)
                return false;
        for(int i = a.length() + 1; i < b.length() - 1; i++){
                if(name[i] == '/')
                        return false;
        }
        return true;
}

string get_end_name(const char * name)
{
        string temp(name);
        if(temp.back() == '/')
                temp.pop_back();
        string ret;
        int p = temp.size()-1;
        while( p >= 0 && temp[p] != '/'){
                p--;
        }
        p++;
        for(; p < temp.size(); p++){
                ret.push_back(temp[p]);
        }
        return ret;
}

int path_cmp(const char *path, const char * name)
{
        string a(path+1);
        string b(name);
        if(b.back() == '/')
                b.pop_back();
        return a != b;
}

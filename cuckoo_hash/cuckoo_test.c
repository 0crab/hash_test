#include <stdio.h>
#include <pthread.h>
#include "memcached_define.h"
#include "libcuckoo_hash_table.h"
#include "cuckoo.h"
#include <stdlib.h>
#include <sys/time.h>

#define SET_THREAD 1
#define GET_THREAD 0

#define DATA_NUM 100000
#define THREAD_NUM 4
#define SET_OP 0

uint64_t runtimelist[THREAD_NUM];

item * database[DATA_NUM];

void con_database();


uint64_t get_runtime(struct timeval start,struct timeval end){
    return (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
}


void *thread_set(int id){
    printf("starting set %d\n",id);
    //uint64_t insert_count=0;
    struct timeval t1,t2;
    gettimeofday(&t1,NULL);
    for(int i=0;i<DATA_NUM;i++){
        cuckoo_upsert(database[i]);
    }
    gettimeofday(&t2,NULL);
    if(id!=-1){
        runtimelist[id]+=get_runtime(t1,t2);
      //  printf("%d finish set :%ld\n",id,insert_count);
    }
}

void *thread_get(int id){
    printf("starting get %d\n",id);
    int success_num=0;
    uint64_t tmpkey;
    struct timeval t1,t2;
    gettimeofday(&t1,NULL);
    for(int i=0;i<DATA_NUM;i++){
        tmpkey=(uint64_t)i;
        if(cuckoo_find(&tmpkey)!=NULL){
            success_num++;
        }
    }
    gettimeofday(&t2,NULL);
    runtimelist[id]+=get_runtime(t1,t2);
    printf("thread %d,total %d,find %d\n",id,DATA_NUM,success_num);
}

int main(){
    con_database();
    cuckoo_init();
    pthread_t pid[THREAD_NUM];
    if(SET_OP){
        for(int i=0;i<THREAD_NUM;i++){
            if(pthread_create(&pid[i],NULL,thread_set,i)!=0){
                printf("create pthread error\n");
                return 0;
            }
        }
    } else{
        thread_set(-1);
        for(int i=0;i<THREAD_NUM;i++){
            if(pthread_create(&pid[i],NULL,thread_get,i)!=0){
                printf("create pthread error\n");
                return 0;
            }
        }
    }

    for(int i=0;i<THREAD_NUM;i++){
        pthread_join(pid[i],NULL);
    }


    uint64_t find_count=0;
    for(int i=0;i<10000;i++){
        int tmp=rand()%DATA_NUM;
        uint64_t key=tmp;
        item * it=cuckoo_find(&key);
        if(it!=NULL){
            find_count++;
            //printf("%ld\t%ld\t%ld\n",key,*(uint64_t*)ITEM_key(it),*(uint64_t*)ITEM_data(it));
        }else{
            //printf("%d  %ld null\n",i,key);
        }
    }
    printf("find:%d\n",find_count);

    uint64_t runtime=0;
    for(int i=0;i<THREAD_NUM;i++){
        runtime+=runtimelist[i];
    }
    printf("runtime:%ld\n", runtime/THREAD_NUM);

}

void con_database(){
    char value[8]="hello";
    for(int i=0;i<DATA_NUM;i++){
        uint64_t tmpkey=i;
        database[i]=item_alloc((char *)&tmpkey,8,0,0,6);
        memcpy(ITEM_data(database[i]),value,5);
        sprintf(ITEM_data(database[i])+5,"%d",i);
    }
}
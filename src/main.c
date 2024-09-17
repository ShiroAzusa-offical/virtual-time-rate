#include<unistd.h>
#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <fcntl.h>
#include<syscall.h>
#include <x86_64-linux-gnu/sys/types.h>
#include <x86_64-linux-gnu/sys/stat.h>
#include <x86_64-linux-gnu/sys/mman.h>
#define RATION ((char*)ration_ptr)[0]/((char*)ration_ptr)[1]
#define __NR_clock_gettime 228
int *ration_ptr;
int detect;

void initfunc(){
    int fd;
    fd = open("/tmp/ration", O_RDWR|O_CREAT, 0666);
    ftruncate(fd, sizeof(int));
    ration_ptr = (int *)mmap(NULL, sizeof(int)*2, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    *ration_ptr=0;
    ((char*)ration_ptr)[1]=1;
    ((char*)ration_ptr)[0]=1;
    detect=*ration_ptr;
}
__attribute__((constructor)) void initfunc();

char gettimeofdayflag=0;
char clock_gettimeflag=0;
long cache_interval;
long cache_timespec;
long base_interval;
long base_timespec;
long system_timestamp[6];

unsigned int sleep(unsigned int seconds);
int nanosleep(const struct timespec *req, struct timespec *rem);
int usleep(useconds_t usec);
inline long conbined(long a,long b,long ration);

inline long conbined(long a,long b,long ration){
    return (a*ration)+b;
}
int gettimeofday(struct timeval *tv, struct timezone *tz){
    int ret;
    if(gettimeofdayflag==0){
        struct timeval tmp;
        ret=syscall(__NR_gettimeofday,&tmp,tz);
        cache_interval=conbined(tmp.tv_sec,tmp.tv_usec,1000000);
        base_interval=cache_interval;
        gettimeofdayflag=1;
    }
    ret=syscall(__NR_gettimeofday,tv,tz);
    long now=conbined(tv->tv_sec,tv->tv_usec,1000000);
    if(detect!=*ration_ptr){
            base_interval=base_interval+((now-cache_interval)*((char*)&detect)[0]/((char*)&detect)[1]);
            detect=*ration_ptr;
            cache_interval=now;
    }
    now=base_interval+((now-cache_interval)*RATION);
    tv->tv_sec=now/1000000;
    tv->tv_usec=now%1000000;

    return ret;
}


int clock_gettime(clockid_t clk_id, struct timespec *tp){
    int ret;
    if(clock_gettimeflag==0){
        struct timespec tmp;
        int index=0;
        while(index<6){
            ret=syscall(__NR_clock_gettime,index,&tmp);
            system_timestamp[index]=conbined(tmp.tv_sec,tmp.tv_nsec,1000000000);
            index++;
        }
        cache_timespec=system_timestamp[CLOCK_MONOTONIC_RAW];
        base_timespec=cache_timespec;
        clock_gettimeflag=1;
    }
    ret = syscall(__NR_clock_gettime, CLOCK_MONOTONIC_RAW, tp);
    long now=conbined(tp->tv_sec,tp->tv_nsec,1000000000);
    if(detect!=*ration_ptr){
            base_timespec=base_timespec+((now-cache_timespec)*((char*)&detect)[0]/((char*)&detect)[1]);
            detect=*ration_ptr;
            cache_timespec=now;
    }
    now=base_timespec+((now-cache_timespec)*((char*)&detect)[0]/((char*)&detect)[1]);
    now=now-system_timestamp[CLOCK_MONOTONIC_RAW]+system_timestamp[clk_id];
    tp->tv_sec=now/1000000000;
    tp->tv_nsec=now%1000000000;
    return ret;
}

unsigned int sleep(unsigned int seconds){
    struct timespec req;
    struct timespec rem;
    req.tv_sec=seconds;
    req.tv_nsec=0;
    if(nanosleep(&req,&rem)==-1){
        return rem.tv_sec;
    }else{
        return 0;
    }
}
int nanosleep(const struct timespec *req, struct timespec *rem){
    struct timespec modifiedTimespec;
    memcpy(&modifiedTimespec,req,sizeof(struct timespec));
    modifiedTimespec.tv_sec=modifiedTimespec.tv_sec/RATION;
    modifiedTimespec.tv_nsec=modifiedTimespec.tv_nsec/RATION;
    int result=0;
    result=syscall(SYS_nanosleep,&modifiedTimespec,rem);
    if(result==-1){
        if(rem!=NULL){
            rem->tv_sec=rem->tv_sec*RATION;
            rem->tv_nsec=rem->tv_nsec*RATION;
            result=-1;
        }
    }
    return result;
}
int usleep(useconds_t usec){
    struct timespec req;
    struct timespec rem;
    req.tv_sec=usec/1000000;
    req.tv_nsec=(usec%1000000)*1000;
    return nanosleep(&req,&rem);
}

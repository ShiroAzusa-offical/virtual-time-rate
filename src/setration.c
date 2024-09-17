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

int main(int argc, char *argv[]) {
    int value=0;
    int valuea=0;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <integer> or <integer>/<integer>\n", argv[0]);
        return 1;
    }
    if (sscanf(argv[1], "%d/%d", &value,&valuea) !=2) {
        if (sscanf(argv[1], "%d", &value) !=1) {
            fprintf(stderr, "Error: Invalid number format.\n");
            return 1;
        }else{
            valuea=1;
    }
        if((value==0) || (valuea==0)){
            fprintf(stderr, "Error: Value 0 detected\n");
            return 1;
        }
    }
    int fd;
    fd = open("/tmp/ration", O_RDWR|O_CREAT, 0666);
    ftruncate(fd, sizeof(int));
    int* ration_ptr = (int *)mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    ((char*)ration_ptr)[0]=(char)value;
    ((char*)ration_ptr)[1]=(char)valuea;
    munmap(ration_ptr, sizeof(int));
    close(fd);
    return 0;
}

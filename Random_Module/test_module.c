#include<stdio.h>
#include<errno.h>  
#include<fcntl.h>   // Required for O_RDONLY
#include <unistd.h> // Required for read

void random_number(int* ptr_n, int fd) {
    read(fd, ptr_n, sizeof(ptr_n));
}

int main(){
    int fd;
    printf("Starting device test code example...\n");
    fd = open("/dev/random_module_device", O_RDONLY);// Open the device with read only access
    if (fd < 0) {
        perror("Failed to open the device...");
        return errno;
    }

    int n;
    random_number(&n, fd);
    
    printf("Random number generated: %d\n", n);

    printf("End of the program\n");
    return 0;
}
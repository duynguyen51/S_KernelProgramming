#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() {
      int fd =0, ret = 0;
      char buf[100] = "Hooking...";
      fd = open("./output.txt", O_WRONLY | O_CREAT);
      ret = write(fd, buf, 10);
      buf[ret] = '\0';
      printf("Buffer: %s\n", buf);
      printf("Bytes:  %d\n", ret);
      close(fd);
      return 0;
}

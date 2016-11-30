#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>

#define C_SIZE (sizeof(char) * 20)
    
void main ()  {
    int fd;
    char str[C_SIZE];
    
    fd = open("/dev/stupidchar", O_RDWR, S_IRUSR|S_IWUSR);

    if (fd != -1) {
        read(fd, &str, C_SIZE);
        printf("The stupidchar is %s\n", &str);
        printf("Please input the str written to stupidchar\n");
        
        scanf("%s", &str);
        write(fd, &str, C_SIZE);
        read(fd, &str, C_SIZE);
        
        printf("The stupidchar now is %s\n", &str);
        close(fd);
    } else {
        printf("Error when invoke driver\n");
    }

    /**
     *  在 open 设备的时候会检查是否有效
     *  如果无效是不能够进入输入界面的
     */
}
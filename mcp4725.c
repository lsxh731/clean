#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
int main(int argc, char *argv[])
{
	int fd;
	/**********************i2cdev Open***************************/
    char *i2cdev_node = "/dev/i2cdev";
    if((fd = open(i2cdev_node, O_RDWR|O_NOCTTY|O_NDELAY))<0)
    {
        printf("i2cdev Open failed!\n");
    }
    else
    {
        printf("i2cdev Open success!\n");
    }
	
return 0;
}

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "comm.h"

int fd;

void comm_open()
{
    mkfifo("/tmp/myfifo", 0666);
    if ((fd = open("/tmp/myfifo", O_WRONLY)) < 0)
    {
        err("open")
    }
}

void comm_write(char *format, ...)
{
    char buffer[512];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);

    if (write(fd, buffer, strlen(buffer)) != strlen(buffer))
    {
        err("write");
    }

    va_end(args);
}

void comm_close()
{
    close(fd);
}

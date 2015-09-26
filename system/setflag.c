#include <fcntl.h>

int sys_setflag(int fd, int flags)
{
    int val;

    if ((val = fcntl(fd, F_GETFL, 0)) < 0)
        return -1;

    val |= flags;

    if (fcntl(fd, F_SETFL, val) < 0)
        return -1;

    return 0;
}

int sys_clearflag(int fd, int flags)
{
    int val;

    if ((val = fcntl(fd, F_GETFL, 0)) < 0)
        return -1;

    val &= ~flags;

    if (fcntl(fd, F_SETFL, val) < 0)
        return -1;

    return 0;
}

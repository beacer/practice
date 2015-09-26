#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

int sys_readn(int fd, void *vptr, int n)
{
    int nleft, nread;
    char *ptr;

    ptr = vptr;
    nleft = n;
    while (nleft > 0) {
        if ((nread = read(fd, ptr, nleft)) < 0) {
            if (errno == EINTR)
                nread = 0;
            else
                return -1;
        }
        else if (nread == 0)
            break;

        nleft -= nread;
        ptr += nread;
    }

    return n - nleft;
}

int sys_writen(int fd, const void *vptr, int n)
{
    int nleft;
    int nwritten;
    const char *ptr;

    ptr = vptr;
    nleft = n;
    while (nleft > 0) {
        if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
            if (nwritten < 0 && errno == EINTR)
                nwritten = 0;   /* and call write() again */
            else
                return (-1);    /* error */
        }

        nleft -= nwritten;
        ptr += nwritten;
    }

    return n;
}

#define RLMAXLINE   1024
static pthread_key_t rl_key;
static pthread_once_t rl_once = PTHREAD_ONCE_INIT;

typedef struct {
    int rl_cnt;
    char *rl_bufptr;
    char rl_buf[RLMAXLINE];
} Rline;

static void
readline_destructor(void *ptr)
{
    free(ptr);
}

static void
readline_once(void)
{
    /* this function can return nothing */
    pthread_key_create(&rl_key, readline_destructor);
    return;
}

static ssize_t
my_read(Rline *tsd, int fd, char *ptr)
{
    if (tsd->rl_cnt <= 0) {
again:
        if ((tsd->rl_cnt = read(fd, tsd->rl_buf, RLMAXLINE)) < 0) {
            if (errno == EINTR)
                goto again;
            return -1;
        } else if (tsd->rl_cnt == 0)
            return 0;

        tsd->rl_bufptr = tsd->rl_buf;
    }

    tsd->rl_cnt--;
    *ptr = *tsd->rl_bufptr++;
    return 1;
}

/* readline (multi-thread safe version) */
ssize_t 
sys_readline(int fd, void *vptr, size_t maxlen)
{
    int n, rc;
    char c, *ptr;
    Rline *tsd;

    if (pthread_once(&rl_once, readline_once) != 0)
        return -1;

    if ((tsd = pthread_getspecific(rl_key)) == NULL) {
        if ((tsd = calloc(1, sizeof(Rline))) == NULL)
            return -1;
        if (pthread_setspecific(rl_key, tsd) != 0)
            return -1;
    }

    ptr = vptr;
    for (n = 1; n < maxlen; n++) {
        if ((rc = my_read(tsd, fd, &c)) == 1) {
            *ptr++ = c;
            if (c == '\n')
                break;
        } else if (rc == 0) {
            *ptr = 0;
            return n - 1;
        } else {
            return -1;
        }
    }

    *ptr = 0;
    return n;
}

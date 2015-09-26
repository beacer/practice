#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include "sysutil.h"

static int dump_stream(int sockfd)
{
    char buf[1024 * 32];
    int n;
    unsigned int bytes = 0;
    int bytes_inter = 0;
    struct timeval time_last, time_curr;
    unsigned int delta;

    gettimeofday(&time_last, NULL);
    fprintf(stderr, "dumping ...\n");
    if ((n = read(sockfd, buf, 16)) > 0) {
        bytes += n;
        bytes_inter += n;
        if (fwrite(buf, 1, n, stdout) != n) {
            fprintf(stderr, "fwrite error\n");
            return -1;
        }

        buf[n] = '\0';
        fprintf(stderr, "%s\n", buf);
    } else {
        goto out;
    }

    while ((n = read(sockfd, buf, sizeof(buf))) > 0) {
        bytes += n;
        bytes_inter += n;

        gettimeofday(&time_curr, NULL);
        delta = (time_curr.tv_sec - time_last.tv_sec) * 1000000 \
                + time_curr.tv_usec - time_last.tv_usec;
        if (delta >= 1000000) {
            if (delta < 2000000)
                fprintf(stderr, "%u bytes received\n", bytes_inter);
            else
                fprintf(stderr, "%u bytes received at long time %08u\n", bytes_inter, delta);
            time_last = time_curr;
            bytes_inter = 0;

            /*
            {
                static int aaaaabbbbb = 0;
                if (++aaaaabbbbb % 10 == 0) {
                    fprintf(stderr, "sleep a while\n");
                    sys_usleep(1, 0);
                }
            }
            */
        }

        if (fwrite(buf, n, 1, stdout) != 1) {
            fprintf(stderr, "fwrite error\n");
            return -1;
        }
    }

    fprintf(stderr, "dumped %u bytes (total)\n", bytes);
    fflush(stdout);
out:
    if (n == 0) {
        fprintf(stderr, "end of file\n");
        return 0;
    } else {
        perror(NULL);
        return -1;
    }
}

static void sig_chld(int signo)
{
    pid_t pid;
    int stat;

    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        ;
    return;
}

int main(int argc, char *argv[])
{
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    char cliip[20];

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(0);
    }

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror(NULL);
        exit(1);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[1]));
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listenfd, (SA *)&servaddr, sizeof(servaddr)) != 0) {
        perror(NULL);
        exit(1);
    }

    if (listen(listenfd, 15) != 0) {
        perror(NULL);
        exit(1);
    }

    sys_signal(SIGCHLD, sig_chld);
    fprintf(stderr, "listening on port %s ...\n", argv[1]);

    while (1) {
        clilen = sizeof(cliaddr);
        if ((connfd = accept(listenfd, (SA *)&cliaddr, &clilen)) < 0) {
            if (errno == EINTR)
                continue;
            else {
                perror(NULL);
                exit(1);
            }
        }

        if (inet_ntop(AF_INET, &cliaddr.sin_addr, cliip, sizeof(cliip)) != NULL) {
            fprintf(stderr, "%s is connected on by port %d\n", 
                    cliip, htons(cliaddr.sin_port));
        }

        if ((childpid = fork()) < 0) {
            perror(NULL);
            exit(1);
        }

        if (childpid == 0) { /* child */
            close(listenfd);
            dump_stream(connfd);
            exit(0);
        } else { /* parent */
            close(connfd);
        }
    }
}

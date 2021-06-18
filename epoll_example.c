//
// Created by Administrator on 2021/6/18.
//

#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include "epoll_example.h"

#define MAX_BUF 1000
#define MAX_EVENTS 5

int
main(int argc, char *argv[]){
    printf("Epoll example\n\r");

    int epfd, ready, fd, s, j, numOpenFds;
    struct epoll_event ev;
    struct epoll_event evlist[MAX_EVENTS];
    char buf[MAX_BUF];

    if(argc < 2 || strcmp(argv[1], "--help") == 0){
        printf("Usage: %s file...\n\r", argv[0]);
    }

    epfd = epoll_create(argc - 1);
    if(epfd == -1){
        printf("Error: epoll_create\n\r");
        return -1;
    }

    for(j =1; j<argc; j++){
        fd = open(argv[j], O_RDONLY);
        if(fd == -1) {
            printf("Error: open\n\r");
            return -1;
        }

        printf("Opened \"%s\" on fd %d\n", argv[j], fd);

        ev.events = EPOLLIN;
        ev.data.fd = fd;
        if(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1){
            printf("Error: epoll_ctl\n\r");
            return -1;
        }
    }

    numOpenFds = argc - 1;

    while(numOpenFds > 0) {
        printf("About to epoll_wait()\n\r");
        ready = epoll_wait(epfd, evlist, MAX_EVENTS, -1);
        if (ready == -1) {
            if (errno == EINTR)
                continue;
            else {
                printf("Error: epoll_wait");
                return -1;
            }
        }

        printf("Ready: %d\n", ready);

        for (j = 0; j < ready; j++) {
            printf(" fd=%d; events: %s%s%s\n", evlist[j].data.fd,
                   (evlist[j].events & EPOLLIN) ? "EPOLLIN " : "",
                   (evlist[j].events & EPOLLHUP) ? "EPOLLHUP " : "",
                   (evlist[j].events & EPOLLERR) ? "EPOLLERR " : "");

            if (evlist[j].events & EPOLLIN) {
                s = read(evlist[j].data.fd, buf, MAX_BUF);
                if (s == -1) {
                    printf("Error: read");
                    return -1;
                }
                printf(" read %d bytes: %.*s\n", s, s, buf);

            } else if (evlist[j].events & (EPOLLHUP | EPOLLERR)) {
                /* If EPOLLIN and EPOLLHUP were both set, then there might
                be more than MAX_BUF bytes to read. Therefore, we close
                the file descriptor only if EPOLLIN was not set.
                We'll read further bytes after the next epoll_wait(). */

                printf(" closing fd %d\n", evlist[j].data.fd);
                if (close(evlist[j].data.fd) == -1) {
                    printf("Error: close");
                    return -1;
                }
                numOpenFds--;
            }
        }
    }
    printf("All file descriptors closed; bye\n");
    return 0;
}
















# Terminal 1                                       
```shell                                                        
$ mkfifo p q
$ ./epoll_example p q
```

# Terminal 2
```shell
$ cat > p
ppp

$ cat > q
qqq
```
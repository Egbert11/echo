# echo
echo server &amp; client
### 实现功能
* 可以供服务端与单个客户端进行通信，基于socket，实现兼容了windows和linux，在两个平台上均可正常运行。

### 运行方式
```
# server
cd echoserver
gcc server.c -o server
./server
```

```
# client
cd echoclient
gcc client.c -o client
./client
```

### 实现效果
```
jmhuang@cc-offline09-30002:~/echo/echoserver$ ./server  
sockserver:3, conn:4                    
hello world!
nice to meet you                                                
你好啊
```

```
jmhuang@cc-offline09-30002:~/echo/echoclient$ ./client
connect success. conn:3                                                                                                        
hello world!                                                                                                                 
hello world!                                                                                                                 
nice to meet you                                                                                                                
nice to meet you                                                                                                                
你好啊                                                                                                                  
你好啊                                                                                                                  
exit
```
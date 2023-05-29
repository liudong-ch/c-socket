# C-Socket

## 介绍
c语言socket编程

linux的socket编程，io多路复用，select、poll、epoll使用

## 文件目录
client 文件夹：
  client.c：客户端文件代码
  main.c：客户端启动入口
service 文件夹：
  main.c：服务端启动入口
  multi_process.c：多进程版socket
  multi_thread.c：多线程版socket
  io_select.c：多路复用select版
  io_epoll.c：多路复用epoll版

## 测试方式
service.main.c更改调用函数。
编译、运行service.main.c文件启动服务

编译、运行client.main.c文件启动客户端

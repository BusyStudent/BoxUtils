# Box

BoxUtils  
一个自己用的小工具箱  
封装了一些库 自己也加入了一些东西  

## 功能

> 解析json xml  
> 发起HTTP请求  
> 创建Socket  
> 线程的同步和通讯  
> 协程  
> 创建子进程  

## 构建方法

> `cd ./src && make` 当找不到头文件时 **要稍微修改一下Makefile**  
> 或者
> `xmake`

## 依赖于下面的库

[cJSON](https://github.com/DaveGamble/cJSON)
[libucontext](https://github.com/kaniini/libucontext)
[ucontext-win](https://github.com/brock7/ucontext-win)
[curl](https://github.com/curl/curl)
[libxml2](https://github.com/GNOME/libxml2)

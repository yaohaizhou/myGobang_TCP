# TODO list：

- [x] 创建开局客户端发包
- [x] 客户端接包分配先后手
- [x] 客户端发包落子
- [x] 客户端接包显示

# 程序整理

整体先不单独开创socket类结构，现在board类结构里面全部实现再考虑分类

新增函数

1. create_game()
2. offense_or_defense
3. sendPackage
4. recvPackage
5. TCP_package

新增成员

1. sock
2. *recv_package
3. *send_package







#### 创建开局客户端发包

在main函数添加函数

board.create_game()



```
int create_game()
{
	TODO:构造函数，创建socket的时候完成与服务端的通信
}
```

修改参数或者返回值

1. sock保存与服务端通信的套接字

**进度**

创建开局没有问题

可以服务器发包给客户端并且打印，暂时没有做落子操作

同时在创建的时候直接把socket关闭了，这导致后面不能再发包了，这个需要修改



#### 客户端接包分配先后手

宗旨是在最开始的地方调用offense_or_defense函数修改FIRST

在main函数里添加函数

board.offense_or_defense()



```
int offense_or_defense(sock)
{
	TODO:接包得到先后手的分配
	
}
```

FIRST返回值

1	player1执黑先走

0	player1执白后走

**进度**

未开展



#### 客户端发包落子

在getMouseLoc函数里面添加sendPackage部分



```
int sendPackage(sock)
{
	TODO:客户端发包落子
}
```

**进度**

由于socket在创建开局函数中被关闭了，所以这里的包一直发出去

但是在开局的时候有尝试是可以发送到服务端并且解析的



#### 客户端接包显示

在board里添加函数TCP_package()

TCP_package里面添加recvPackage函数



```
int recvPackage(sock)
{
	TODO:客户端接包显示
}
```

**进度**

可以显示，暂时没做棋盘落子



#### 5.14

下一步计划

1. 把落子打印文字信息完成，对应send_package函数，也就是每落一子可以在服务端看到落子信息。现在遇到的问题是socket被关闭了导致不能继续发包。解决路径：重看服务端和客户端对话的流程，在一开始创建对局的时候保留socket，在后面发送信息的时候调用socket
2. 把接收落子在棋盘上打印完成，对应recv_package函数
3. 先后手简单，offense_or_defense函数
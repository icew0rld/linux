# IO

## IO是什么？

IO指在内存和IO设备之间拷贝数据的过程。



## IO包含一些什么？

CPU

内存

IO设备

- 电子部件（设备控制器）
  - ​寄存器

- 机械部件

中断控制器

DMA控制器

总线

操作系统

- 驱动程序
- 中断向量



IO的种类：

磁盘IO、网络IO、终端IO

## IO是怎么实现的？



## IO设备

IO设备的类型

块设备：把信息存储在固定大小的块中，每个块有自己的地址，所有传输以一个或多个完整的块为单位。每个块都能独立读写。例子：硬盘。

字符设备：以字符为单位发送或接收一个字符流，而不考虑任何块结构。字符设备不可寻址。例子：网络接口。



设备控制器

​	IO设备的电子部件

寄存器

数据缓冲区

IO端口

内存映射IO：每个设备控制器的寄存器被分配一个内存地址

cpu想要读一个字的时候，它将需要的地址放到总线的地址线上，然后在总线的一条控制线上置起一个READ信号

DMA控制器：直接存储器存取，IO设备不经过CPU，直接和内存交换数据。

中断控制器：IO设备完成工作，向中断控制器发中断（通过总线的中断线）。中断控制器在地址线上放置一个数字表明哪个设备需要处理，并置气一个中断CPU信号。CPU停止当前工作，地址线上的数字，被用作指向一个中断向量的表格的索引，这个索引对应的中断服务程序开始运行。运行后或运行完后，CPU向中断控制器发应答，告诉中断控制器可以发另一个中断了。



异步传输和同步传输

同步传输：阻塞。如果IO操作是阻塞的，那么用户更容易编写。在read系统调用后，程序自动被阻塞，直到缓冲区的数据准备好。

异步传输：中断驱动

物理IO是异步的：CPU启动传输后便去做其他工作，直到中断发生。操作系统使实际上是中断驱动的操作变成在用户看来是阻塞式的操作。



程序控制IO

进程在用户空间缓冲区组装数据

调用系统调用打开设备

调用系统调用写数据

​	操作系统将数据复制到内核空间的缓冲区

​	检测设备可用直到设备ready（轮询polling或忙等待busy waiting读设备状态寄存器）

​		写数据（复制到数据寄存器）

​		还有数据，返回检测设备可用



中断驱动IO

使用DMA的IO：在中断的基础上，用DMA控制让设备从内存直接取数据，直到全部传输完成再中断CPU，减少了中断次数。



IO软件的组织



设备驱动程序：对设备进行控制的设备特定代码

设备驱动程序一般被放在操作系统内核中。操作系统定义了驱动程序必须支持的标准接口。驱动程序可以在操作系统运行时被动态装载到操作系统中。驱动程序的主要职责是翻译上层发来的抽象读写请求成设备控制器指令，并通过总线与其通讯。其交互过程一般是中断驱动（代表操作系统内核）。驱动程序内部可被运行调用其他内核过程。



磁盘

柱面、磁道、扇区、块



终端

在大型机上，通常存在许多远程用户，每个用户拥有一个终端，该设备包含一个键盘和一个连载一起的显示器，作为一个单位。



键盘

每当一个键被按下时，会产生一个中断。一个键被释放时，也会产生一个中断。每当一个键盘中断发生时，键盘驱动程序都要从与键盘相关的IO端口提取信息，以了解发生了什么。





非阻塞IO

使我们可以发出open、read、write这也的IO操作，并使这些操作不会永远阻塞。如果这种操作不能完成，则调用立即出错返回，表示改操作如继续执行将阻塞。



记录锁（record locking）：（@todo考虑放到文件或进程或并发中）

当第一个进程正在读或修改文件的某个部分时，使用记录锁可以阻止其他进程修改共一个文件区域。记录是一种误用，更合适的术语可能是字节范围锁by-range locking，因为它锁定的只是文件中的一个区域。

fcntl

锁类型

​	共享读锁

​	独占性写锁

区域

​	系统会组合或分裂相邻区域

进程终止，锁全部释放

如果文件描述符被关闭，则该进程通过这个描述符引用的文件上的锁都会被释放。

fork子进程不继承锁。

exec可以继承原程序的锁。

FreeBSD的锁信息，是放在vnode中的。

强制性锁：内核检测每个open、read、write，验证调用进程是否违背了正在访问的文件上的某一把锁。



IO多路转接（IO multiplexing）

是什么？

先构造一张我们感兴趣的描述符的列表，然后调用一个函数，直到这些描述符中的一个已经准备好进行IO时，该函数才返回。



有什么用？

select 

给他描述符、条件、等待时间，在时间到期之前如果没有条件发生，阻塞，否则，返回，内核会返回满足条件的描述符。

poll

和select类似，函数原型有点差异



epoll @todo

和异步IO有什么区别？



异步IO

当描述符准备好可以进行IO时，用一个信号通知它。

POSIX异步IO

定义一个数据结构，aiocb，里面包含要读或写的文件描述符、数据、回调函数。

然后调用aio_read或aio_write把它交给内核请求IO，内核会在IO请求完成后，用信号通知调用进程，或在一个单独的线程中执行回调函数。



Glibc AIO

Kernel Native AIO



## 存储映射IO（memory-mapped IO）

将一个磁盘文件映射到存储空间中的一个缓冲区上，当从缓冲区中取数据，就相当于读文件中的相应字节。与此类似，将数据存入缓冲区时，相应字节就自动写入文件。

mmap

fork继承，exec不继承

什么时候用？



ioctl：IO操作的杂物箱，不仅仅是文件



## IO多路复用(multiplexing)

### epoll

是什么：

- monitoring multiple file descriptors to see if I/O is possible on any of them 的Linux系统api，这些api构成一种I/O event notification facility

为什么：

- scales well to **large numbers** of watched file descriptors。所支持的文件描述符数量是最大可以打开的文件数目，1GB的内存约10万左右（未证实）

有什么：

- 文件描述符
- 内核中epoll实例
  - evertpoll结构体struct eventpoll
    - struct rb_root rbr：监听事件红黑树，RB tree root used to store monitored fd structs 
    - struct list_head visited_list_link：就绪事件链表

怎么用

*  **epoll_create：创建epoll实例，**creates a new epoll instance and returns a file descriptor referring to that instance.  (The more recent epoll_create1 extends the functionality of epoll_create(2).)
  *  **epoll_ctl: 注册文件描述符，**Interest in particular file descriptors is then registered via epoll_ctl.  The set of file descriptors currently registered on an epoll instance is sometimes called an epoll set.
  *  **epoll_wait：等待IO事件，**waits for I/O events, blocking the calling thread if no events are currently available.

Triggering modes

​	edge-triggered interface: epoll_wait will return only when a new event is enqueued with the epoll object。性能更好些。只能和非阻塞IO一起使用。

​	level-triggered interface: epoll_wait will return as long as the condition holds. 默认

怎么实现

- 红黑树、链表、中断处理程序中的回调函数

  当我们执行epoll_ctl时，除了把socket放到epoll文件系统里file对象对应的红黑树上之外，还会给内核中断处理程序注册一个回调函数，告诉内核，如果这个句柄的中断到了，就把它放到准备就绪list链表里。

- 共享内存：内核需要把fd通知到用户地址空间，是通过映射同一块内存实现的（未证实）



常见面试题

- select、poll、epoll有什么区别
  - select：监听fd_set（分为读、写、异常三个集）；不返回哪个fd完成，需要遍历所有fd
  - poll：监听struct pollfd数组，每个pollfd中指定了IO事件类型；不返回哪个fd完成，需要遍历所有fd
  - epoll：监听之前注册的fd以及对应的struct epoll_event；返回发生事件的fd和对应的事件，不需要遍历所有fd

资料：

[wiki epoll](https://en.wikipedia.org/wiki/Epoll)

[epoll详解](https://blog.csdn.net/xiajun07061225/article/details/9250579)

[man epoll](http://man7.org/linux/man-pages/man7/epoll.7.html)
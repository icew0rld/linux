# 一个Linux C程序的生命周期

## 从源代码到进程

### 源代码

`Hello.c`

```c
#include <stdio.h>
int main()
{
    printf("hello\n");
    return 0;
}
```

### 预处理

将include依赖的头文件包含进来（以及宏扩展等预处理动作）

```shell
gcc -E Hello.c -o Hello.i
or
cpp Hello.c Hello.i
```

[Hello.i](./code/Hello.i)

### 编译

翻译成汇编代码

```shell
gcc -S Hello.c 
```

`Hello.s`

```assembly
        .file   "Hello.c"
        .section        .rodata
.LC0:
        .string "hello"
        .text
        .globl  main
        .type   main, @function
main:
.LFB0:
        .cfi_startproc
        pushq   %rbp
        .cfi_def_cfa_offset 16
        .cfi_offset 6, -16
        movq    %rsp, %rbp
        .cfi_def_cfa_register 6
        movl    $.LC0, %edi
        call    puts
        movl    $0, %eax
        popq    %rbp
        .cfi_def_cfa 7, 8
        ret
        .cfi_endproc
.LFE0:
        .size   main, .-main
        .ident  "GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.5) 5.4.0 20160609"
        .section        .note.GNU-stack,"",@progbits
```

### 汇编

翻译成可重定位目标文件

```shell
gcc -c Hello.c
or
as -o Hello.o Hello.s
```

[Hello.o](code/Hello.o)

ELF可重定位目标文件格式：

（ELF: Executable and Linkable Format，Linux下的格式 ）

![](/Users/acer/project/os/pic 8.png)

.text：程序的机器码

.rodata：只读数据，比如printf语句中的格式字符串（一般而言的代码段包括.text和.rodata等）

.data：已初始化的全局变量

.bss：未初始化的全局变量（不占空间，仅仅是占位符，可执行目标文件加载时被初始化为0）(一般而言的数据段包括.data和.bss等)

.symtab：符号表，在程序中定义和引用的函数和全局变量的信息

.rel.text：.text中的引用的列表，每项称为重定向条目

.rel.data：.data中的引用的列表，每项称为重定向条目

### 链接

#### 符号（Symbol）

设m模块为本可重定位目标文件

- 全局符号：由m模块定义的可被其他模块引用的非static函数和非static全局变量
- 外部符号：其他模块定义的，被m模块引用的全局符号
- 本地符号：由m模块定义的static函数和static全局变量

#### 符号表条目

![](/Users/acer/project/os/pic 13.png)

如果是在m模块中定义的符号，section+value可确定其位置。

如果是引用其他模块中的符号，section为UNDEF。

#### 符号解析（symbol resolution）

将符合引用和符号定义联系起来。

如果是m模块中定义的符号，直接就知道其定义的位置。

如果是其他模块中定义的符号，链接器会检查其他模块的符号表，找到符号的定义条目，于是便知道符号定义的位置。

#### 重定位（relocation）

**重定位节和符号定义**

链接器将可重定位目标文件所有相同类型的节合并为同一类型的新的聚合节。然后将运行时地址赋予新的聚合节、输入模块中定义的每个节、输入模块定义的每个符号。这时，程序中每个符号都有了运行时地址。

**重定位符号引用**

**重定位条目**

![](/Users/acer/project/os/pic 14.png)

offset确定了引用的位置，相对于.text或.data。symbol确定了所引用的符号。

有2种最基本的重定位type：

- R_386_PC32，引用使用的是PC相对地址。比如: call addr，cpu会跳到pc+addr的位置，pc此时是call的下一条指令的地址。

  在这种情况下，给引用重新定位的地址，就是符号所在地址，减去引用所在的地址。（细节上，由于pc当前如果是下一条指令，则还需要减去引用本身所占用的字节数）

- R_386_32，引用使用绝对地址。

  这种情况下，给引用重新新定位的地址，就是符号所在的地址。

#### 链接到静态库

静态库（static library）相关可重定位目标文件打包而成的文件。链接时，只拷贝被引用的目标文件。Linux下文件格式：archive，比如：

```shell
ar rcs libvector.a addvec.o multvec.o
```

使用静态库解析引用：反复扫描静态库中的目标文件。

#### 链接到共享库

**共享库（shared library）**

是object file的一种，通常以.so结尾。在任何给定的文件系统中，一个共享库只有一个.so文件，所有引用该库的可执行目标文件共享这个.so文件中的代码和数据，而不是像静态库的内容那样被拷贝和嵌入到引用它们的可执行文件。其次，在内存中，一个共享文件的.text节的一个副本被不同的进程共享。（**注意，这里只有.text，而没有.data**）

**动态链接（dynamic linking）**

在运行时，共享库可以加载到任意的存储器地址，并和一个在存储器中的程序链接起来。

动态链接的过程：

![](/Users/acer/project/os/pic 15.png)

注意：

链接被分为不同时的两步，ld做一部分事情（部分链接），ld-linux.so做一部分事情（完全链接）。

部分链接和之前的相同，即完成**符号解析**。

当加载器加载和执行可执行目标文件时，它注意到可执行目标目标文件包含.interp节，其中包含了动态链接器的路径名（比如ld-linux.so，它本身是一个共享库），它不再像通常那样将控制权交给应用，而是加载和运行这个动态链接器。它执行以下**重定位**任务：

- 重定位动态库本身的文本和数据到某个内存段
- 重定位可执行目标文件中所有对动态库的引用

最后，动态链接器将控制传递给应用程序。

（以上是在加载时加载和链接到共享库，实际上还可以在运行时加载和链接共享库，相关系统调用：dlopen/dlsym/dlclose/dlerror，不详述）

**位置无关代码**

PIC：Position Independent Code

动态库，物理内存中同一份.text，可以被映射到不同进程的不同地址段，这需要共享库.text中的对本共享库的数据和函数的引用的地址，是位置无关的。否则，不同进程的不同地址段，.text的引用的地址必须不同，导致无法共享。

PIC技术提供了这样的支持。

数据引用：

- GOT=全局偏移量表，GOT的条目的值是数据的address（仍然是由重定位得到）
- .text中数据引用的值是GOT中对应数据条目的偏移量
- 因此，mov会被替换为另外一些指令：
  - lea ebx, ADDR_OF_GOT
  - mov edx, DWORD PTR [ebx + offset]
  - mov edx, DWORD PTR [edx]

这样，利用一重间接访问，就使.text中对数据的引用的值，和.text在地址空间中的位置无关了。

![](/Users/acer/project/os/pic 16.png)

函数引用：

- 也是利用了GOT间接存放地址来实现的，概要如下图，具体见参考资料[pic][pic]

![](/Users/acer/project/os/pic 17.png)

![](/Users/acer/project/os/pic 18.png)

#### 可执行目标文件

链接的输出产生可执行文件，Linux下ELF可执行文件数据结构如下：

![](/Users/acer/project/os/pic 9.png)

注：

- 只有代码段和数据段被加载到内存中。
- 这个图不包含动态链接需要的段，比如动态链接需要的.interp、符号表、重定位条目等。

### 加载

shell中运行可执行目标文件：

- 调用系统调用fork创建子进程

- 调用execve调用操作系统中的加载器（loader）

  - 新建地址空间，将其中的页映射到可执行目标文件的页大小的片（chunk），新的代码和数据段会被初始化为可执行文件的内容。除了一些头部信息，加载过程没有任何从磁盘到内存的数据拷贝，直到cpu引用一个被存储器映射的虚拟页才会进行拷贝，此时，操作系统利用它的页面调度进制自动将页面从磁盘传送到内存。

  - 跳转到程序的入口运行程序，调用细节：

    ![](/Users/acer/project/os/pic 19.png)

加载完成后，程序就变成了**进程**，具有如下地址空间：

- 细节地，栈上面，内核下，还有命令行参数和环境变量

![](/Users/acer/project/os/pic 10.png)

### 相关命令

gcc：编译工具集

objdump：读目标文件，反汇编

ar：创建静态库

readelf：读elf目标文件

strings：列出目标文件中可打印的字符串

strip：从目标文件删除符号表

nm：列出目标文件符号表中的符号

size：列出目标文件节的名字和大小

ldd：列出可执行目标文件所需共享库

## 进程的一生

@todo

在进程中可以做什么

能力范围：通用图灵机、调用其他库、调用系统调用、操作内存、文件系统、标准输入输出、图形?、网络输入输出、和其他进程通信或同步，用多线程并发做事情



怎么死掉的

​	main中return

如果一直运行呢，后台进程是怎么回事

## 源代码的诞生：从无到源代码

@todo

从无到放在硬盘上

​	一台计算机怎么组成，是怎么工作的（微机原理、芯片、单片机、8086等）

​		cpu、内存、硬盘、网卡、总线、键盘、鼠标、电源、图灵机、冯诺依曼体系结构

​	启动计算机（有哪些过程）

​		自举

​	打开shell

​	用vim创建文件hello.c

​	保存到磁盘

启动过程@todo init进程、自举、bootstrap之类

## 有用的资源

[pic]: https://blog.csdn.net/wuhui_gdnt/article/details/51094732	"共享库中的位置无关代码（PIC）"




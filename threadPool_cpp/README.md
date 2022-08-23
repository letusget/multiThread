# C++实现的高效线程池

[C++实现线程池](https://www.bilibili.com/video/BV1364y1i7uo?vd_source=f3313eb2bd9a48578392fe2d57c328f8)

> 记录一个奇怪的报错：`vscode 无法打开 源 文件 "queue"C/C++(1696)`
> 
> 姑且算是参考链接吧：[CSDN:远程连接-头文件报错](https://blog.csdn.net/weixin_44317124/article/details/124560046) 
> ---  [CSDN:头文件报错](https://blog.csdn.net/wbvalid/article/details/115001149)
> 
> 反正是参考头文件的问题，访问不到头文件，我首先确定了`/usr/include` 目录下确实都是我的头文件，个人猜测可能是头文件访问错误等原因导致的，所以将 配置文件 `c_cpp_properties.json` 文件中的配置进行修改，修改后如下：
> ```json
> {
>    "configurations": [
>        {
>            "name": "Linux",
>            "includePath": [
>                "${workspaceFolder}/**",
>                //添加的这两个部分，就解决了头文件包含问题
>                "${default}",
>                "/usr/include"
>            ],
>            "defines": [],
>            "compilerPath": "/usr/bin/gcc",
>            "cStandard": "gnu17",
>            "cppStandard": "gnu++17",
>            "intelliSenseMode": "linux-gcc-x64"
>        }
>    ],
>    "version": 4
>}
> ```
> 反正头文件包含报错的问题，就这样的解决了！
> **并没有完全解决！！！**

完成！！

线程池是一种多线程处理形式，处理过程中将任务添加到队列，然后在创建线程后自动启动这些任务。线程池线程都是后台线程。每个线程都使用默认的堆栈大小，以默认的优先级运行，并处于多线程单元中。如果某个线程在托管代码中空闲（如正在等待某个事件）, 则线程池将插入另一个辅助线程来使所有处理器保持繁忙。如果所有线程池线程都始终保持繁忙，但队列中包含挂起的工作，则线程池将在一段时间后创建另一个辅助线程但线程的数目永远不会超过最大值。超过最大值的线程可以排队，但他们要等到其他线程完成后才启动。


## 运行项目

进入项目文件夹，并运行如下命令：
```cpp
//编译项目
gcc *.c -o threadpool -pthread
//运行项目
./threadpool
```


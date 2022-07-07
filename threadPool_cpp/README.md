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


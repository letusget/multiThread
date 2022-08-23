连接数据库需要配置数据量连接文件，也就是要使用 mysql.h
当时安装mysql时，所有的安装文件，全部如下：
```sh
[root@centos7 mysql]# pwd
/usr/local/mysql
[root@centos7 mysql]# ll
æ»ç¨é 1646496
-rw-r--r--. 1 root root  843008000 4æ  13 20:20 mysql-8.0.28-1.el7.x86_64.rpm-bundle.tar
-rw-r--r--. 1 7155 31415  55199948 12æ 18 2021 mysql-community-client-8.0.28-1.el7.x86_64.rpm
-rw-r--r--. 1 7155 31415   5933684 12æ 18 2021 mysql-community-client-plugins-8.0.28-1.el7.x86_64.rpm
-rw-r--r--. 1 7155 31415    645388 12æ 18 2021 mysql-community-common-8.0.28-1.el7.x86_64.rpm
-rw-r--r--. 1 7155 31415   7763684 12æ 18 2021 mysql-community-devel-8.0.28-1.el7.x86_64.rpm
-rw-r--r--. 1 7155 31415  23637584 12æ 18 2021 mysql-community-embedded-compat-8.0.28-1.el7.x86_64.rpm
-rw-r--r--. 1 7155 31415   2215928 12æ 18 2021 mysql-community-icu-data-files-8.0.28-1.el7.x86_64.rpm
-rw-r--r--. 1 7155 31415   4935572 12æ 18 2021 mysql-community-libs-8.0.28-1.el7.x86_64.rpm
-rw-r--r--. 1 7155 31415   1265072 12æ 18 2021 mysql-community-libs-compat-8.0.28-1.el7.x86_64.rpm
-rw-r--r--. 1 7155 31415 473116268 12æ 18 2021 mysql-community-server-8.0.28-1.el7.x86_64.rpm
-rw-r--r--. 1 7155 31415 268279684 12æ 18 2021 mysql-community-test-8.0.28-1.el7.x86_64.rpm
[root@centos7 mysql]#

```

首先要找到mysql的安装目录，在 `./mysql/mysql.h` 中包含了需要使用的mysql的API
但是我的虚拟机中并没有这个文件（mysql可以正常使用），猜测是缺少 API包，使用`yum install mysql-devel`  进行安装，但是会报错：
>** 发现 5 个已存在的 RPM 数据库问题， 'yum check' 输出如下：
mysql-community-client-8.0.28-1.el7.x86_64 有缺少的需求 >mysql-community-client-plugins = ('0', '8.0.28', '1.el7')
mysql-community-libs-8.0.28-1.el7.x86_64 有缺少的需求 >mysql-community-client-plugins = ('0', '8.0.28', '1.el7')
mysql-community-server-8.0.28-1.el7.x86_64 有缺少的需求 >mysql-community-icu-data-files = ('0', '8.0.28', '1.el7')
2:postfix-2.10.1-9.el7.x86_64 有缺少的需求 libmysqlclient.so.18()(64bit)
2:postfix-2.10.1-9.el7.x86_64 有缺少的需求 libmysqlclient.so.18(libmysqlclient_18)(64bit)
>

遇到问题就要解决！
个人感觉是因为缺少 配置文件包，所以需要安装文件包：`rpm -ivh mysql-community-libs-compat-8.0.28-1.el7.x86_64.rpm` 这个文件包需要下载，很容易就可以下载到

仍然是报错缺少包：
>** 发现 3 个已存在的 RPM 数据库问题， 'yum check' 输出如下：
mysql-community-client-8.0.28-1.el7.x86_64 有缺少的需求 mysql-community-client-plugins = ('0', '8.0.28', '1.el7')
mysql-community-libs-8.0.28-1.el7.x86_64 有缺少的需求 mysql-community-client-plugins = ('0', '8.0.28', '1.el7')
mysql-community-server-8.0.28-1.el7.x86_64 有缺少的需求 mysql-community-icu-data-files = ('0', '8.0.28', '1.el7')
>
没有办法，继续安装包：`rpm -ivh mysql-community-client-plugins-8.0.28-1.el7.x86_64.rpm
`
再次运行，报错信息减少了，证明这个方法是有效的，报错信息如下：
>** 发现 1 个已存在的 RPM 数据库问题， 'yum check' 输出如下：
mysql-community-server-8.0.28-1.el7.x86_64 有缺少的需求 mysql-community-icu-data-files = ('0', '8.0.28', '1.el7')
>
那就继续安装包：`rpm -ivh mysql-community-icu-data-files-8.0.28-1.el7.x86_64.rpm` 

继续报错：
>错误：软件包：1:mariadb-devel-5.5.68-1.el7.x86_64 (base)
          需要：mariadb-libs(x86-64) = 1:5.5.68-1.el7
 您可以尝试添加 --skip-broken 选项来解决该问题
 您可以尝试执行：rpm -Va --nofiles --nodigest
>

根据 [CSDN:mysql安装报错](https://blog.csdn.net/rdp1305442102/article/details/105530240) 解决：

继续`rpm -ivh mysql-community-devel-8.0.28-1.el7.x86_64.rpm` 和 `rpm -ivh mysql-community-embedded-compat-8.0.28-1.el7.x86_64.rpm` 之后，再次运行 `yum install mysql-devel`就不再报错了：
![image](https://tva1.sinaimg.cn/large/006x3t5Xgy1h3zjodmr5uj31cg0n01kx.jpg)

我想我找到我的mysql安装路径了：`/usr/include/mysql`,在这里有很多我需要的文件：
![image](https://tva3.sinaimg.cn/large/006x3t5Xgy1h3zk219qccj30eb0iidm9.jpg)

还有mysql的动态库的路径：`/usr/lib64/mysql` 

```sh
git push origin main:main
```








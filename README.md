#### Linux 简单字符驱动设备
+	测试环境 
	
	````
		3.10.0-123.9.3.el7.x86_64
	````

+	安装驱动过程
	
	+	C 文件和 Makefile 在目录 /src
	+ 	进入目录 make 一下
	+ 	编译 安装驱动
	
	````
		insmod stupidchar.ko
	````
	
	将这个模块加载到内核中
	
	````
		dmesg | tail -20
	````
	
	在驱动中使用 printk 函数打印的 log 
	使用 dmesg 查看
	看 log 里面是不是正确装载了这个模块（顺便里面打印了一些函数调用信息）
	
	````
		cat | grep stupidchar /proc/devices
		mknod /dev/stupidchar c 主设备号 副设备号
	````
	
	先看看刚刚装载模块的主设备号
	第一条命令打印出的编号即为 mknod 的主设备号 副设备号一般是 0
	
	+	然后进到 /run 目录用 gcc 跑一下测试程序
	
	````
		gcc test.c -o test
		./test
	````
	
	如果运行成功，程序结束之后使用 dmesg 查看内核调用信息是会打印一些函数调用相关信息出来
	
+	删除该驱动设备
	
	````
		rm -rf /dev/stupidchar
		lsmod
		rmmod stupidchar
	````
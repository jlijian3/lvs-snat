##lvs-snat

在小米的dsnat基础上修改，修复了跟NAT/FULLNAT的兼容性问题。
dsnat提供了内网机器访问外网的功能，见[https://github.com/xiaomi-sa/dsnat](https://github.com/xiaomi-sa/dsnat "DSNAT") 。

#####修复了dsnat两个问题

1. NAT不能使用，在forword钩子函数ip_vs _out中对已经存在的连接，没有判断是否NAT的连接，导致NAT转发出错

	**解决方法**： 因为dsnat是通过添加一个0.0.0.0:0的service来实现的，在做dsnat之前，判断svc->addr.ip和svc->port是否为0
  
2. FULLNAT的local address添加方式被覆盖，只能为某个zone添加laddr，不能为virtual service添加laddr
	
	**解决方法**：恢复原来为service添加local address的方式，另外增加两个接口为某个zone添加/删除local address，并相应的修改ipvsadm，-P/-Q恢复为service添加laddr，增加-U/-W是为zone添加laddr；keepalive暂时没有修改。

##change log
 - 修复跟NAT的兼容性问题
 - 修复跟FULLNAT的local address的冲突问题

##安装方法

跟dsnat相同，见[https://github.com/xiaomi-sa/dsnat](https://github.com/xiaomi-sa/dsnat "DSNAT") 。
内核patch、ipvsadm和keepalived的代码请使用本项目提供的。

###zone的ipvsadm配置有所变化
	#注意dsnat中使用-P来添加zone的laddr，这里改为-U
    ipvsadm -U --zone 0.0.0.0/0 -z 1.2.100.3
	#删除zone的laddr有-Q改为-W
    ipvsadm -W --zone 0.0.0.0/0 -z 1.2.100.3
	
	#-P/-Q恢复为原来的功能，即为service添加删除laddr

###keepalived对zone的配置展示不支持








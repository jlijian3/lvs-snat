##基于阿里lvs-v1的snat网关
#####版本说明
1.  fullnat-snat-lvs-v1-2.6.32-279.el6是在小米的dsnat基础上修改，修复了跟NAT/FULLNAT的兼容性问题。dsnat提供了内网机器访问外网的功能，但是跟NAT/FULLNAT同时使用会有一些问题，见[https://github.com/xiaomi-sa/dsnat](https://github.com/xiaomi-sa/dsnat "DSNAT") 。
dsnat的fullnat使用的是阿里的lvs-v1版本，见http://kb.linuxvirtualserver.org/wiki/IPVS_FULLNAT_and_SYNPROXY ，这个版本比较旧了，建议使用lvs-v2版本https://github.com/alibaba/LVS/tree/lvs_v2，lvs-v3依赖于万兆网卡flow director特性。
2.  snat-kernel-2.6.32-279.el6是直接在官方内核上修改的，没有使用FULLNAT补丁，在NAT基础上修改。


##change log
 - **修复跟NAT的兼容性问题**
 
 	在forword钩子函数ip_vs _out中对已经存在的连接，没有判断是否NAT的连接，导致NAT转发出错

	**解决方法**： 因为dsnat是通过添加一个0.0.0.0:0的service来实现的，在做dsnat之前，判断svc->addr.ip和svc->port是否为0，或者判断dest->addr.ip == IP_VS_DSNAT_RS_ADDR

 - **修复跟FULLNAT的local address的冲突问题**


	 FULLNAT的local address添加方式被覆盖，只能为某个zone添加laddr，不能为virtual service添加laddr
		
	 **解决方法**：恢复原来为service添加和使用local address的方式，另外增加两个接口为某个zone添加/删除local address，并相应的修改ipvsadm，-P/-Q恢复为service添加laddr，增加-U/-W是为zone添加laddr；keepalived暂时没有修改。

	
- **不使用FULLNAT补丁，直接在官方内核NAT基础上增加SNAT功能**

	不影响NAT转发，ipvsadm和keepalived不修改，不支持源地址黑白名单

##fullnat-snat安装方法

跟dsnat相同，见[https://github.com/xiaomi-sa/dsnat](https://github.com/xiaomi-sa/dsnat "DSNAT") 。
内核patch、ipvsadm和keepalived的代码请使用本项目提供的。

###zone的ipvsadm配置有所变化
	#注意dsnat中使用-P来添加zone的laddr，这里改为-U
    ipvsadm -U --zone 0.0.0.0/0 -z 1.2.100.3
	#删除zone的laddr有-Q改为-W
    ipvsadm -W --zone 0.0.0.0/0 -z 1.2.100.3
	
	#-P/-Q恢复为原来的功能，即为service添加删除laddr

###keepalived对zone的配置暂时不支持

##snat安装方法
###下载redhat 6.3内核
	wget ftp://ftp.redhat.com/pub/redhat/linux/enterprise/6Server/en/os/SRPMS/kernel-2.6.32-279.el6.src.rpm
###准备代码
  rpm -ivh kernel-2.6.32-279.23.1.el6.src.rpm
	cd ~/rpmbuild/SPECS
	rpmbuild -bp kernel.spec
###打补丁
	cd ~/rpmbuild/BUILD/
	cd kernel-2.6.32-279.23.1.el6/linux-2.6.32-279.23.1.el6.x86_64/
	wget https://raw.github.com/jlijian3/lvs-snat/master/snat-kernel-2.6.32-279.el6/lvs-snat-2.6.32-279.el6.patch
	patch -p1<lvs-snat-2.6.32-279.el6.patch
###编译安装
	make -j16
	make modules_install
	make install
	##重启使用新内核
###ipvsadm配置示例
	#直接用官方的ipvsadm即可
	#添加0.0.0.0:0的虚拟服务,加上-p参数
	#因为只有persistent service才能添加端口为0的服务，而我懒得修改ipvsadm代码了
	ipvsadm -A -t 0.0.0.0:0 -s rr -p 10
	#添加转换后的源地址，这里直接使用添加real server参数，端口为0，如下
	/sbin/ipvsadm -a -t 0.0.0.0:0 -r 10.0.5.100:0 -m
	#内网访问外网时，源地址就会被改为10.0.5.100
	
    







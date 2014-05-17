##lvs-snat
#####版本说明
1.  patches.lbg-lvs-v2是ucweb lbg项目目前使用的版本，针对alibaba lvs-v2的补丁。完整的代码在 https://github.com/jlijian3/LVS/tree/lvs_v2，阿里的代码 https://github.com/alibaba/LVS/tree/lvs_v2
2.  我们的入口负载均衡使用lvs-v2，出口网关有两种实现方案

    a) 使用iptables的SNAT，功能完善稳定，性能较差
    
    b) 基于lvs-v2开发的SNAT网关，功能简单，多isp出口不适用，性能较好
    
3.  snat-gateway-lvs-v2.patch在阿里lvs-v2的fullnat基础上实现了简单的snat网关
4.  iptable-lbg-CHROUTE-lvs-v2.patch在阿里lvs-v2内核增加了iptables扩展，实现redirect nexthop功能


##基于lvs-v2的snat网关安装方法
###在alibaba 的lvs基础上打补丁
	git clone git@github.com:alibaba/LVS.git
 	cd LVS
	git branch lvs_v2
	patch -p1 < snat-gateway-lvs-v2.patch

	#如果不想打补丁直接使用我的完整代码
	git clone https://github.com/jlijian3/LVS.git

###编译内核
	make -j16
	make modules_install
	make install
	init 6

###安装keepalived ipvsadm
	cd LVS/tools/keepalived
	./configure --with-kernel-dir="/lib/modules/`uname -r`/build"
	make
	make install
	cd ../ipvsadm
	make
	make install

###ipvsadm配置方法
	#添加fwmark为1的virtual service，开启snat网关服务
    ipvsadm -A -f 1 -s rr
	#添加rs，rs ip一定要写成外网出口网关，比如电信网关，转发模式-b选择fullnat
    ipvsadm –a -f 1 –r 1.1.2.1 -b
	#添加外网ip作为snat的ip,多个ip轮询使用
    ipvsadm –P -f 1 -z 1.1.2.100
    ipvsadm –P -f 1 -z 1.1.2.101
    ipvsadm –P -f 1 -z 1.1.2.102
    #把内外机器的默认网关指向lvs的内网ip
    
###keepalived配置方法后续补充

##iptables做snat网关的方法
如果不想用lvs做网关，直接使用iptables即可，不用安装lvs-v2内核，随便一个2.6.32的内核就ok
###iptables SNAT配置方法
	iptables -t nat -A POSTROUTING -s 192.168.100.0/24 -o eth1 -j SNAT --to-source 1.1.2.100-1.1.2.102
	iptables -t nat -A POSTROUTING -s 192.168.100.0/24 -o eth2 -j SNAT --to-source 1.1.3.100-1.1.3.102 --persitent
	
	#-s匹配内网网段，-o匹配出口网卡，多isp，多个上行网卡就有用了
	#to-source可以是一个ip，也可以使连续的ip断，ip选择算法不是轮询，默认是hash(sip,dip)
	# --persitent表示ip选择算法是hash(sip)，就是一个内网ip固定一个出口ip
	
	#同样，内网机器默认网关指向iptables所在机器的内网ip

###使用iptable_lbg CHROUTE
我们在多isp下有各种变态的选路需求，因此开发了iptable lbg扩展，实现网关重定向，内核统一使用lvs-v2
###
	git clone git@github.com:alibaba/LVS.git
	cd LVS
	git branch lvs_v2
	patch -p1 < iptable-lbg-CHROUTE-lvs-v2.patch

	#如果不想打补丁直接使用我的完整代码
	git clone https://github.com/jlijian3/LVS.git
    
###编译内核
	make -j16
	make modules_install
	make install
	init 6
	
###安装iptables
	git clone https://github.com/jlijian3/lvs-snat.git
	cd lvs-snat/iptables-1.4.7-lbg
	./configure
	make
	make install
	cp iptables_init_script /etc/init.d/iptables
	service iptables start
	#查看iptable_lbg,ipt_CHROUTE模块是否加载
	lsmod|grep ipt
	

###iptables网关重定向
比如有些业务要求电信走联通，有的要求电信走移动，这样我们不需要修改静态路由和策略路由，加几条规则即可
###
	#按照正常的路由，报文的nexthop是1.1.2.1，但是我们修改为1.1.3.1
	iptables -t lbg -A FORWARD -s 192.168.100.0/24 -o eth1 -j CHROUTE --gw 1.1.3.1 --old-gw 1.1.2.1





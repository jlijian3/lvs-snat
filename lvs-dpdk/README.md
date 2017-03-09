https://github.com/lvsgate/lvs-dpdk

lvs-dpdk把lvs-snat移植到OpenFastPath了

OpenFastPath基于odp，可以选择odp-dpdk版本，lvs-dpdk就是lvs-snat + ofp + odp-dpdk

这个版本的LVS做了lockless优化，所有资源都做成per cpu了，必须使用支持flow director的网卡，利用fdir把fullnat的local address和snat-gw source port跟网卡rx队列一一绑定，从而达到跟core一一绑定

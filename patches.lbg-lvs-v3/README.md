##rps_framework for lvs-v3
#####版本说明
阿里的lvs-v3 做了并行化优化，需要万兆网卡支持flow director特性，才能正常使用。
为了支持千兆网卡，我们增加rps框架，模拟flow director，需要设置net.core.rps_framework = 1开启rps，见https://github.com/jlijian3/LVS 。

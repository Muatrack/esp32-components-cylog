## cylog
循环写flash, 供应用层代码直接调用

使用准备:
 1 分区表, 中需要指定分区用于日志存储, 日志分区有两种用途,分别是用于存储 事件和记录
    日志-事件分区: 存储内容例如 子设备 02 发生 告警类别日志,此类日志长度固定可调, 写入事件分区
    日志-记录分区: 存储内容例如 子设备 02 电压\电流\功率... 及其数值,类此的周期性日志
    分区表增加条目例如:

    # Name,     Type, SubType, Offset,      Size, Flags
    events,     data, 0x10,             ,   16K,
    records,    data, 0x10,             ,   4K,
    其中的name, Type, SubType 字段内容不可变

    
 2 分区初始化, 已将函数装成为 宏定义
    步骤一 调用前包含头文件  cylog.h 
    步骤二 事件分区初始化调用  CYLOG_EVENT_PARTITION_INIT();  记录分区初始化调用 CYLOG_RECORD_PARTITION_INIT(); 

 3 日志读写, 已将函数装成为 宏定义
    事件日志的写入 例如告警事件 CYLOG_EALARM(devId, "....."), 
    事件日志的读取 例如告警事件 CYLOG_EREAD(blockIdx, cb), blockIdx 为日志分区(每4k)的索引, cb 为日志条目的回调函数. 
   
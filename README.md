## 日志组件基于文件系统、文件方式实现读写和管理。

### 参数定义

组件通过宏定义实现读写并发数量控制，日志根目录的路径设置:

* CYLOG_RW_CONOPTS // 定义读写存储可以同时操作的数量, 缺省值1
* CYLOG_ROOT_DIR   // 定义用于存储日志的根目录

### 如何使用

#### 初始化日志组件
```c
    // bool cylog_init(char *rootDir);

    // 初始化日志根目录， /sdb/logroot
    cylog_init("/sdb/logroot");
```
#### 初始化分类日志
```c
    // cylog_create(cylog_type_t logType, char *logPath, uint16_t fSize, uint16_t fCount);

    // 初始化 告警类型的日志， 日志文件放到目录 alarm 中， 每个文件的大小为1024, 共计使用4个文件作存储
    cylog_create(CYLOG_T_ALARM, "alarm", 1024, 4); 
```
#### 写数据到日志
```c
    // bool cylog_write(cylog_type_t logType, uint8_t pData[], uint16_t dLen, uint32_t timeoutTs );

    // 将日志内容 log，写入告警日志， 等待超时时长为15ms
    uint8_t log[16] = {0x99, 0x03, 0x04, 0x05 ... 0x22};
    if(cylog_write(CYLOG_T_ALARM, &log, sizeof(log), 15)==true) { 
        /** 写入日志成功 */ 
    } else {
        /** 写入日志失败 */ 
    }
```
### 单元测试
单元测试使用googleTest + pytest 测试框架实施。

测试内容:
1. 日志根目录、分类日志目录、各分类日志文件的数量, 创建后是否数量正确、命名正确
2. 全新目录写日志
    * 能否从00文件开始写
    * 写日志后，中途停止，再次写日志是否能够找到对应的位置
    * 00文件写满时的偏移量，是否符合预期
    * 00文件写满后，查找下一个文件是否正确
    * 01文件是否能够被正确识别写入偏移量

3. 全部文件第一次被写满后
    * 再次操作写入日志，能够找到正确的文件及偏移位置
    * 写日志多次重写，是否能够



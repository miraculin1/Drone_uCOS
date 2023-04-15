# 4.15

    - 根据官方手册加入了 `OS_CPU_C.c` 中关于 Hook 
    的函数定义，增加了详细的描述。
    - 完成 `OSTaskStkInit()` 增加相关注释

# 4.13 
    - 编写了临界区相关代码，目前正在 `OSTaskStkInit()` 的
    开始部分，解决了堆栈对齐的相关问题[参考文档][1]

[1]:https://github.com/ARM-software/abi-aa/blob/2982a9f3b512a5bfdc9e3fea5d3b298f9165c36b/aapcs32/aapcs32.rst#the-stack


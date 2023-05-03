# 4.16
    - 正在移植，到 systickISR 的时候打算先实现下 
    SystemInit

    frustrating if had to say, 
    正在实现 SystemInit()
    但是对于检查 PLL 是否就绪和检查 systick 是否以
    PLL 作为时钟源的时候，会出现 PC 起飞的情况
    而且 USAGEFUALTREG 也给出一些奇妙的报错

    折磨了


# 4.15

    - 根据官方手册加入了 `OS_CPU_C.c` 中关于 Hook 
    的函数定义，增加了详细的描述。
    - 完成 `OSTaskStkInit()` 增加相关注释

# 4.13 
    - 编写了临界区相关代码，目前正在 `OSTaskStkInit()` 的
    开始部分，解决了堆栈对齐的相关问题[参考文档][1]

[1]:https://github.com/ARM-software/abi-aa/blob/2982a9f3b512a5bfdc9e3fea5d3b298f9165c36b/aapcs32/aapcs32.rst#the-stack


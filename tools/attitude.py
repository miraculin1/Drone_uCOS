import serial

import matplotlib.pyplot as plt
import numpy as np


def three_dimension_scatter():
    """
    3d scatter plot
    """
    # 生成画布
    fig = plt.figure()

    # 打开交互模式
    plt.ion()

    # 循环
    while 1:
        # 清除原有图像
        fig.clf()

        # 设定标题等
        fig.suptitle("attitude")

        # 生成测试数据
        ser = serial.Serial('/dev/ttyACM1')

        line1 = ser.readline()
        sl1 = str(line1, 'ascii')

        t1 = sl1.split()
        if len(t1) != 9:
            continue

        cnt = 0
        tmp = np.zeros(12, dtype=float, order='C')
        print(t1)
        for i in range(0, 9):
            tmp[cnt] = float(t1[i])
            cnt += 1

        for i in range(0, 3):
            tmp[cnt] = 0
            cnt += 1

        data = np.reshape(tmp, (4, 3))
        x = data[:, 0]
        y = data[:, 1]
        z = data[:, 2]

        # 生成画布
        ax = fig.add_subplot(111, projection="3d")

        # 画三维散点图
        ax.scatter(x, y, z)

        # 设置坐标轴图标
        ax.set_xlabel("X Label")
        ax.set_ylabel("Y Label")
        ax.set_zlabel("Z Label")

        # 设置坐标轴范围
        ax.set_xlim(-1, 1)
        ax.set_ylim(-1, 1)
        ax.set_zlim(-1, 1)

        # 暂停
        plt.pause(0.0002)

    # 关闭交互模式
    plt.ioff()

    # 图形显示
    plt.show()
    return
# three_dimension_scatter()


three_dimension_scatter()

import serial

from matplotlib.animation import FuncAnimation
import matplotlib.pyplot as plt
import numpy as np

x = []
y = []
z = []


def animation(step):

    while 1:
        global fig, ax
        # 生成测试数据
        ser = serial.Serial('/dev/ttyACM1', buad=115200, timeout=1)
        print("hello")

        line1 = ser.readline()
        sl1 = str(line1, 'ascii')

        t1 = sl1.split()
        if len(t1) != 9:
            continue

        cnt = 0
        tmp = np.zeros(12, dtype=float, order='C')
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
        print(x)

        plt.cla()
        # 设置坐标轴范围
        ax.set_xlim(-1, 1)
        ax.set_ylim(-1, 1)
        ax.set_zlim(-1, 1)
        # 设置坐标轴图标
        ax.set_xlabel("X Label")
        ax.set_ylabel("Y Label")
        ax.set_zlabel("Z Label")
        ax.scatter(x, y, z)
        return


# 生成画布
fig = plt.figure()

# 设定标题等
fig.suptitle("attitude")

ax = fig.add_subplot(111, projection="3d")

ani = FuncAnimation(fig=fig, func=animation, interval=1,
                    cache_frame_data=False)

# 图形显示
plt.show()

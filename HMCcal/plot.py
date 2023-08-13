import matplotlib.pyplot as plt
import numpy as np
from mpl_toolkits.mplot3d import Axes3D

file = open("./data.txt", "r")

size = 6000
tmp = np.zeros([size], dtype=float, order='C')

s = file.readline()
cnt = 0
while len(s) != 0 and cnt < size:
    each = s.split()
    print(each)
    for i in range(0, 3):
        tmp[cnt] = int(each[i])
        cnt += 1
    s = file.readline()

data = tmp.reshape(int(size / 3), 3)

x = data[:, 0]
y = data[:, 1]
z = data[:, 2]

print(x, y, z)

# 绘制散点图
fig = plt.figure()
ax = plt.axes(projection="3d")

ax.set_xlabel('$X$', fontsize=20, rotation=150)
ax.set_ylabel('$Y$', fontsize=20)
ax.set_zlabel('$z$', fontsize=30, rotation=60)

ax.scatter(x, y, z)

plt.show()

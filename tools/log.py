# read in data from serial ports
# data structure: "(ekfmsr) (tar pos) (gyro) (extPID out)"
# all in float
import serial
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import animation

dispNum = 100
x = np.linspace(0, 100, 101)

ser = serial.Serial('/dev/ttyACM1', 115200, timeout=1000)

# using the variable axs for multiple Axes
fig, (ax1, ax2, ax3) = plt.subplots(3, 1)
line1_1, = ax1.plot([], [], lw=2, label='ekf', c='r')
line1_2, = ax1.plot([], [], ls='--', lw=2, label='conTar', c='b')
line1_3, = ax1.plot([], [], lw=2, label='extPIDCon', c='y')

line2_1, = ax2.plot([], [], lw=2, label='gyro', c='r')
line2_2, = ax2.plot([], [], ls='--', lw=2, label='posCon', c='b')
line2_3, = ax2.plot([], [], lw=2, label='inPIDCon/100', c='y')

bars = ax3.bar([0, 1, 2, 3], [100, 200, 300, 400], width=1,
               edgecolor='white', linewidth=0.7)
ax3.set(xlim=(0, 3), ylim=(0, 1000), xticks=np.arange(0, 4))
ax3.set_title('thro')

ax1.set_title('pos')
ax2.set_title('rate')
ax1.legend()
ax2.legend()
ax1.set_ylabel(r'$RAD$')
ax2.set_ylabel(r'$RAD/S$')
ax1.set_xlim((0, dispNum))
ax2.set_xlim((0, dispNum))
ax1.set_ylim((-1, 1))
ax2.set_ylim((-1, 1))

ekf = np.zeros(dispNum + 1)
tarRad = np.zeros(dispNum + 1)
extPIDCon = np.zeros(dispNum + 1)

gyro = np.zeros(dispNum + 1)
tarRate = np.zeros(dispNum + 1)
inPIDCon = np.zeros(dispNum + 1)

thros = np.zeros(4)


def is_float(element: any) -> bool:
    # If you expect None to be passed:
    if element is None:
        return False
    try:
        float(element)
        return True
    except ValueError:
        return False


def WW():
    global ekf
    global tarRad
    global gyro
    global tarRate
    global extPIDCon, inPIDCon
    global thros
    valid = False
    ser.flushInput()
    while (not valid):
        line1 = ser.readline()
        line1 = ser.readline()
        sl1 = str(line1, 'utf-8')

        t1 = sl1.split()
        if (len(t1) == 10):
            for i in range(0, 10):
                if (not is_float(t1[i])):
                    valid = False
                    break
                else:
                    valid = True

    ekf = np.insert(ekf, 0, float(t1[0]))
    tarRad = np.insert(tarRad, 0, float(t1[1]))
    gyro = np.insert(gyro, 0, float(t1[2]))
    tarRate = np.insert(tarRate, 0, float(t1[3]))
    extPIDCon = np.insert(extPIDCon, 0, float(t1[4]))
    inPIDCon = np.insert(inPIDCon, 0, float(t1[5]))
    ekf = np.delete(ekf, dispNum)
    tarRad = np.delete(tarRad, dispNum)
    gyro = np.delete(gyro, dispNum)
    tarRate = np.delete(tarRate, dispNum)
    extPIDCon = np.delete(extPIDCon, dispNum)
    inPIDCon = np.delete(inPIDCon, dispNum)
    for i in range(0, 4):
        thros[i] = float(t1[6 + i])


def update(t):
    global ekf
    global tarRad
    global gyro
    global tarRate
    global bars
    WW()
    line1_1.set_data((x, ekf))
    line1_2.set_data((x, tarRad))
    line1_3.set_data((x, extPIDCon))
    line2_1.set_data((x, gyro))
    line2_2.set_data((x, tarRate))
    line2_3.set_data((x, inPIDCon))
    for i in range(0, 4):
        bars[i].set_height(thros[i])


ani = animation.FuncAnimation(
    fig, update, fargs=(), interval=0)
#
plt.show()

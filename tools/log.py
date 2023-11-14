# read in data from serial ports
# data structure: "(ekfmsr) (tar pos) (gyro) (extPID out)"
# all in float
import serial
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import animation

dispNum = 100
x = np.linspace(0, 100, 101)

ser = serial.Serial('/dev/ttyACM1', 115200)

# using the variable axs for multiple Axes
fig, (ax1, ax2) = plt.subplots(2, 1)
line1_1, = ax1.plot([], [], lw=2, label='ekf', c='r')
line1_2, = ax1.plot([], [], ls='--', lw=2, label='tar', c='b')

line2_1, = ax2.plot([], [], lw=2, label='gyro', c='r')
line2_2, = ax2.plot([], [], ls='--', lw=2, label='tar', c='b')

ax1.set_title('pos')
ax2.set_title('rate')
ax1.legend()
ax2.legend()
ax1.set_ylabel(r'$RAD$')
ax2.set_ylabel(r'$RAD/S$')
ax1.set_xlim((0, dispNum))
ax2.set_xlim((0, dispNum))
ax1.set_ylim((-0.6, 0.6))
ax2.set_ylim((-np.pi, np.pi))

ekf = np.zeros(dispNum + 1)
tarRad = np.zeros(dispNum + 1)
gyro = np.zeros(dispNum + 1)
extPID = np.zeros(dispNum + 1)


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
    global extPID
    valid = False
    ser.flushInput()
    while (not valid):
        line1 = ser.readline()
        line1 = ser.readline()
        sl1 = str(line1, 'utf-8')

        t1 = sl1.split()
        if (len(t1) == 4):
            for i in range(0, 4):
                if (not is_float(t1[i])):
                    valid = False
                    break
                else:
                    valid = True

    ekf = np.insert(ekf, 0, float(t1[0]))
    tarRad = np.insert(tarRad, 0, float(t1[1]))
    gyro = np.insert(gyro, 0, float(t1[2]))
    extPID = np.insert(extPID, 0, float(t1[3]))
    ekf = np.delete(ekf, dispNum)
    tarRad = np.delete(tarRad, dispNum)
    gyro = np.delete(gyro, dispNum)
    extPID = np.delete(extPID, dispNum)

    return (x, ekf, tarRad, gyro, extPID)


def update(t):
    global ekf
    global tarRad
    global gyro
    global extPID
    x, ekf, tarRad, gyro, extPID = WW()
    line1_1.set_data((x, ekf))
    line1_2.set_data((x, tarRad))
    line2_1.set_data((x, gyro))
    line2_2.set_data((x, extPID))


ani = animation.FuncAnimation(
    fig, update, fargs=(), interval=10)
#
plt.show()

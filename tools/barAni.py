import matplotlib.pyplot as plt
from matplotlib import animation


def barlist(n):
    return [1/float(n*k) for k in range(1, 6)]


fig = plt.figure()

n = 100  # Number of frames
x = range(1, 6)
barcollection = plt.bar(x, barlist(1))


def animate(i):
    y = barlist(i+1)
    for i, b in enumerate(barcollection):
        b.set_height(y[i])


anim = animation.FuncAnimation(fig, animate, repeat=False, blit=False, frames=n,
                               interval=100)

plt.show()

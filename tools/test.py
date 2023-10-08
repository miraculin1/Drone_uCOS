from matplotlib import pyplot as plt

ages_x = [0, 1, 2, 3]

dev_y = [23, 545, 21, 3]

plt.plot(ages_x, dev_y, color='k', linestyle='--', marker='.', label='aa')
plt.legend()

plt.show()

import matplotlib
import matplotlib.pyplot as plt
import  numpy as np

arr = np.array([[3, 2],[2, 1]])
a = np.linalg.eig(arr)

plt.plot([0, a[1][0][0]], [0, a[1][1][0]])
plt.plot([0, a[1][0][1]], [0, a[1][1][1]])
plt.ylabel("y-axis")
plt.xlabel("x-axis")
plt.show()
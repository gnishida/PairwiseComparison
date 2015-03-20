import numpy as np
import matplotlib.pyplot as plt

f = open("scores.txt")
scores = list()
while True:
	line = f.readline()
	if len(line) == 0:
		break
	scores.append(float(line))
	

x = range(1, 28)

plt.plot(x, scores, "o")
plt.title("Ranking estimation")
plt.xlabel("Ranking")
plt.ylabel("estimated score")
plt.xlim(1, 27)

plt.savefig("scores.png")
plt.show()

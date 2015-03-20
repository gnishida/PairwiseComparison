import numpy as np
import matplotlib.pyplot as plt

f = open("scores.txt")
scores = list()
while True:
	line = f.readline()
	if len(line) == 0:
		break
	scores.append(float(line))
	

x = range(0, 27)
print(x)
print(scores)

plt.plot(x, scores, "o")


import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv("samples.csv")


plt.scatter(data["sample_x"],data["sample_y"])
plt.show()

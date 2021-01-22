from math import *
import matplotlib.pyplot as plt
import numpy as np

def beckmann_distribution(half_theta,alpha):
    exponent = np.exp(- (np.tan(half_theta)**2)/ (alpha**2))
    d = 1 / (pi * alpha**2 * np.cos(half_theta)**4) * exponent
    return d

if __name__ == '__main__':
    beckmann_distribution(0,0.023)
    x = np.linspace(-1, 1, 100)
    y_1 = beckmann_distribution(x,0.023)
    y_2 = beckmann_distribution(x, 0.2)
    y_3 = beckmann_distribution(x, 0.5)
    #plt.plot(x,y_1)
    plt.plot(x, y_2)
    plt.plot(x,y_3)
    plt.show()
from math import *
import matplotlib.pyplot as plt
import numpy as np
import scipy.special as ssp

def beckmann_shadow(half_theta,alpha):
    a = 1 / (alpha * np.tan(half_theta))
    g = 2 / (1+ssp.erf(a) + 1 / (a*np.sqrt(pi)) * np.exp(-a**2))
    return g

def beckmann_shadow_aprox(half_theta,alpha):
    a = 1 / (alpha * np.tan(half_theta))


    g = (3.535 * a + 2.181* a**2 ) / (1+2.276*a+2.577*a**2)
    g[a>=1.6] = 1
    return g

def beckmann_shadow_pbrtapprox(half_theta,alpha):
    a = 1 / (alpha * np.tan(half_theta))
    l = (1-1.259*a+0.396*a**2) / (3.535 *a + 2.181 *a **2)
    l[a>=1.6] = 0
    return 1 / (1+l)

if __name__ == '__main__':
    x = np.linspace(0, 1, 100)
    y_1 = beckmann_shadow(x,0.1)
    y_2 = beckmann_shadow(x, 0.6)
    y_3 = beckmann_shadow(x, 1)
    y_4 = beckmann_shadow(x, 1.6)
    y_a1 = beckmann_shadow_pbrtapprox(x, 0.1)
    y_a2 = beckmann_shadow_pbrtapprox(x, 0.6)
    y_a3 = beckmann_shadow_pbrtapprox(x, 1)
    y_a4 = beckmann_shadow_pbrtapprox(x, 1.6)
    plt.plot(x,y_1,label = "0.1")
    plt.plot(x, y_2,label = "0.6")
    plt.plot(x,y_3,label = "1")
    plt.plot(x, y_4,label = "1.6")
    plt.plot(x, y_a1, label="a0.1")
    plt.plot(x, y_a2, label="a0.6")
    plt.plot(x, y_a3, label="a1")
    plt.plot(x, y_a4, label="a1.6")
    plt.legend()
    plt.show()
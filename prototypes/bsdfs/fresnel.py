from math import *
import matplotlib.pyplot as plt
import numpy as np

def dielectric_fresnel(cos_theta,index_of_refraction_inner = 1.45,index_of_refraction_outer = 1):
    #See Microfacet Models for Refraction through Rough Surfaces Section 5.1

    c = cos_theta

    g = np.sqrt((index_of_refraction_inner/index_of_refraction_outer)**2 -1 + c**2)

    f = 1/2 * ((g-c) / (g+c) )**2 * (1 + ((c*(g+c)-1)/(c*(g-c)+1))**2)
    return f



if __name__ == '__main__':
    x = np.linspace(0, 1, 100)
    y_1 = dielectric_fresnel(x,1.33,1)
    y_2 = dielectric_fresnel(x, 1.45, 1)
    plt.plot(x,y_1)
    plt.plot(x, y_2)
    plt.show()
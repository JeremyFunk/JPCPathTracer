from math import *
import numpy as np
from typing import Tuple
from test_sampling import test_sampling

def uniform_sampling(random_point):
    #x = sin(theta) cos(phi) = cos(2*pi*rand_2) sqrt(1-rand_1**2)
    #y = sin(theta) sin(phi) = sin(2*pi*rand_2) sqrt(1-rand_1**2)
    #z = cos(theta) = rand_1
    x = cos(2*pi*random_point[1])*sqrt(1-random_point[0]**2)
    y = sin(2*pi*random_point[1])*sqrt(1-random_point[0]**2)
    z = random_point[0]
    return np.array([x, y, z])

if __name__ == '__main__':
    test_sampling(uniform_sampling,10)
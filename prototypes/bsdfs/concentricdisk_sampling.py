from math import *
import numpy as np
from typing import Tuple
from test_sampling import test_sampling

def concentricSampleDisk(random_point : Tuple[float,float]):
    rand_1 = random_point[0] *2 -1
    rand_2 = random_point[1] *2 -1

    if(rand_1==0 and rand_2 ==0):
        return np.array([0,0])
    if(abs(rand_1)>abs(rand_2)):
        radius = rand_1
        theta = pi/4*rand_2/rand_1
    else:
        radius = rand_2
        theta = pi/2 - pi/4*rand_1/rand_2
    return radius * np.array([cos(theta),sin(theta)])

if __name__ == '__main__':
    test_sampling(concentricSampleDisk,10)
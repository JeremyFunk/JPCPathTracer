#https://cg.informatik.uni-freiburg.de/course_notes/graphics2_08_renderingEquation.pdf
from math import *
import numpy as np
from typing import Tuple
from test_sampling import test_sampling
from concentricdisk_sampling import concentricSampleDisk



def sample_cosine_weighted_hemisphere(random_point : Tuple[float,float]):
    #see page 47
    #x = sin(theta)cos(phi) = cos(2 pi rand[0]) sqrt(rand[1])
    #y = sin(theta)sin(phi) = sin(2 pi rand[0]) sqrt(rand[1])
    #z = cos(theta) = sqrt(1-rand[1])
    x = cos(2*pi*random_point[1])*sqrt(random_point[0])
    y = sin(2*pi*random_point[1])*sqrt(random_point[0])
    z = sqrt(1-random_point[0])
    return np.array([x,y,z])

def sample_cosine_weighted_hemisphere(random_point : Tuple[float,float]):
    d = concentricSampleDisk(random_point)
    z = sqrt(max(0,1-d[0]**2-d[1]**2))
    return np.array([d[0],d[1],z])

if __name__ == '__main__':
    test_sampling(sample_cosine_weighted_hemisphere,10)
#Reference: Microfacet Models for Refraction through Rough Surfaces
from math import *
import numpy as np

from test_sampling import test_sampling


def beckmann_sampling(random_point,alpha):
    #im microfacet space
    log_sample = log(random_point[0])
    if(isinf(log_sample)):
        log_sample = 0
    tan2theta = -alpha**2*log_sample
    phi = random_point[1]*2*pi

    cos_theta = 1 / sqrt(1+tan2theta)
    sin_theta = sqrt(max(0,1-cos_theta**2))
    # x = sin(theta)cos(phi)
    # y = sin(theta)sin(phi)
    # z = cos(theta)
    x = sin_theta*cos(phi)
    y = sin_theta*sin(phi)
    z = cos_theta
    return np.array([x, y, z])

if __name__ == '__main__':
    f = lambda x : beckmann_sampling(x,1.6)
    test_sampling(f,10)
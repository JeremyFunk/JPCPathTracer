import numpy as np

def char_rgb(r,g,b):
    return np.array([r,g,b])/255.0

# https://www.image-engineering.de/library/technotes/958-how-to-convert-between-srgb-and-ciexyz
def s_to_l(s):
    if(s<=0.04045):
        return s/12.92
    else:
        return ((s+0.055)/1.055)**2.4

def l_to_s(l):
    if(l<=0.0031308):
        return l * 12.92
    else:
        return 1.055*l**(1./2.4) -0.055

def srgb_to_lrgb(srgb):
    #gamma correction:
    return np.array([
        s_to_l(srgb[0]),
        s_to_l(srgb[1]),
        s_to_l(srgb[2])
    ])

def lrgb_to_srgb(lrgb):
    return np.array([
        l_to_s(lrgb[0]),
        l_to_s(lrgb[1]),
        l_to_s(lrgb[2])
    ])

# https://de.wikipedia.org/wiki/RGB-Farbraum#Umrechnung_zwischen_verschiedenen_RGB-Farbr%C3%A4umen
# https://mina86.com/2019/srgb-xyz-conversion/
# https://en.wikipedia.org/wiki/SRGB
M = np.array([
    [0.4124108464885388,   0.3575845678529519,  0.18045380393360833],
	[0.21264934272065283,  0.7151691357059038,  0.07218152157344333],
	[0.019331758429150258, 0.11919485595098397, 0.9503900340503373]
])
M_inv = np.array([
    [ 3.240812398895283,    -1.5373084456298136,  -0.4985865229069666],
	[-0.9692430170086407,    1.8759663029085742,   0.04155503085668564],
	[ 0.055638398436112804, -0.20400746093241362,  1.0571295702861434]
])

def lrgb_to_xyz(lrgb):
    print("lrgb: ",lrgb)
    r = M.dot(lrgb)
    print("r:",r)
    return r

def xyz_to_lrgb(xyz):
    print("xyz: ",xyz)
    r = M_inv.dot(xyz)
    print("r: ",r)
    return r


def xyz_to_srgb(xyz):
    return lrgb_to_srgb(xyz_to_lrgb(xyz))

def srgb_to_xyz(srgb):
    return lrgb_to_xyz(srgb_to_lrgb(srgb))

def tester(srgb,exp):
    ac  = srgb_to_xyz(srgb)
    print("sRgb: ", srgb)
    print("Expected xyz: ",exp)
    print("Acctual xyz: ",ac)
    print("Converted Back: ", xyz_to_srgb(ac))

#red
rgb = [1,0,0]
exp_xyz = [0.64,0.33,0.03]
tester(rgb,exp_xyz)

#green
rgb = [0,1,0]
exp_xyz = [0.30,0.60,0.10]
tester(rgb,exp_xyz)

#red
rgb = [0,0,1]
exp_xyz = [0.15,0.06,0.79]
tester(rgb,exp_xyz)

#white
rgb = [1,1,1]
exp_xyz = [1,1,1]
tester(rgb,exp_xyz)

#white
rgb = [2,3,2]
exp_xyz = [1,1,1]
tester(rgb,exp_xyz)
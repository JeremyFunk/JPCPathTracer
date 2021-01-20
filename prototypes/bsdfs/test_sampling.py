import matplotlib.pyplot as plt
import pandas as pd
def test_sampling(sampling_func,n):
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')

    s_x = []
    s_y = []
    v_x = []
    v_y = []
    v_z = []

    for y in range(0,n):
        for x in range(0, n):
            s1 = x/n+1/(2*n)
            s2 = y/n+1/(2*n)
            x1,x2,x3 = sampling_func([s1,s2])
            ax.scatter(x1,x2,x3)
            s_x.append(s1)
            s_y.append(s2)
            v_x.append(x1)
            v_y.append(x2)
            v_z.append(x3)

    ds = pd.DataFrame()
    ds["s_x"] = s_x
    ds["s_y"] = s_y
    ds["v_x"] = v_x
    ds["v_y"] = v_y
    ds["v_z"] = v_z

    ds.to_csv("bsdfs_samples.csv")
    ax.set_xlabel('X Label')
    ax.set_ylabel('Y Label')
    ax.set_zlabel('Z Label')
    plt.show()


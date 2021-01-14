import matplotlib.pyplot as plt
def test_sampling(sampling_func,n):
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    for y in range(0,n):
        for x in range(0, n):
            ax.scatter(*sampling_func([x/n+1/(2*n),y/n+1/(2*n)]))

    ax.set_xlabel('X Label')
    ax.set_ylabel('Y Label')
    ax.set_zlabel('Z Label')
    plt.show()


import os
import sys
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

def check_path(path: str):
    if not path == '':
        return os.path.exists(path)
        

dir_path = os.path.dirname(os.path.abspath(__file__))

data_path = os.path.join(dir_path, '..', '..', 'docs', 'benchmarks')

if not check_path(data_path):
    print('Benchmarks not found')

indices = pd.read_csv(os.path.join(data_path, 'index.csv'))

print(indices)

for index in indices['names']:
    current_data = pd.read_csv(os.path.join(data_path, index + '.csv'))

    if (current_data['time_unit'] == current_data['time_unit'][0]).all():
        print('Time units not consistent')

    fig, ax = plt.subplots()

    ax.plot(current_data['real_time'], label='Real time')
    ax.plot(current_data['cpu_time'], label='CPU time')

    ax.set_title(index)
    ax.set_xlabel('Benchmark Number')
    ax.set_ylabel('time in ' + current_data['time_unit'][0])
    ax.legend()
    

plt.tight_layout()
plt.show()
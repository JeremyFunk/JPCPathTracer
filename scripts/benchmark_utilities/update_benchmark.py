import os
import subprocess
import json
import sys
import pandas as pd
import numpy as np
from datetime import datetime

def check_path(path: str):
    if not path == '':
        return os.path.exists(path)
            

dir_path = os.path.dirname(os.path.abspath(__file__))

executable_path = os.path.join(dir_path, '..', '..', 'build', 'src', 'benchmarks', 'jpctracer_benchmarks.exe')

storage_path = os.path.join(dir_path, '..', '..', 'docs', 'benchmarks')

if not check_path(executable_path):
    sys.exit("No benchmark executable in build")

if not check_path(storage_path):
    os.makedirs(storage_path)

print("Running benchmarks")

output = subprocess.run([executable_path, '--benchmark_format=json'], capture_output=True)

if(output.returncode == 0):
    print("Benchmarks finished")

    json_out = output.stdout.decode('utf8')
    json_out = json.loads(json_out)

    benchmarks = json_out['benchmarks']

    columns=['run_name', 'run_type', 'repetitions', 'repetition_index', 'threads', 'iterations', 'real_time', 'cpu_time', 'time_unit', 'year', 'month', 'day', 'hour']

    index_file_path = os.path.join(storage_path, 'index.csv')

    if check_path(index_file_path):
        index_file = pd.read_csv(index_file_path)
    else:
        index_file = pd.DataFrame(columns=['names'])

    today = datetime.now()

    for benchmark in benchmarks:
        name = benchmark['name']
        name = name.partition('/')[0]

        print("current benchmark: ", name)

        if name not in index_file['names'].unique():
            index_file = index_file.append({'names': name}, ignore_index=True)

        current_storage_path = os.path.join(storage_path, name+'.csv')
        
        if check_path(current_storage_path):
            previous_data = pd.read_csv(current_storage_path)
        else:
            previous_data = pd.DataFrame(columns=columns)

        new_data = np.array([benchmark[key] for key in columns[:-4]]).reshape(1,9)
        new_data = np.append(new_data, [today.year, today.month, today.day, today.hour]).reshape(1,13)

        new_data = pd.DataFrame(new_data, columns=columns)

        previous_data = pd.concat([previous_data, new_data], ignore_index=False)

        previous_data.to_csv(current_storage_path, index=False)

index_file.to_csv(index_file_path, index=False)


print("All saved")
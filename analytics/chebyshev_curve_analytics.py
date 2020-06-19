#!/usr/bin/env python
# coding: utf-8

# In[1]:


import os
from subprocess import run, PIPE
import matplotlib.pyplot as plt


# In[2]:


os.chdir('../')
os.getcwd()


# In[3]:


def run_robust_sizing(binary, pv_cost, battery_cost, pv_max, cell_max,
                      metric, epsilon, conf, n_days, load_file, pv_file):
    
    cmd = f'{binary} {pv_cost} {battery_cost} {pv_max} {cell_max} {metric} {epsilon} {conf} {n_days} {load_file} {pv_file}'
    p = run(cmd, stdout=PIPE, stderr=PIPE, shell=True)
    
    return p.stdout.decode(), p.returncode


# In[4]:


def get_chebys(output):
    
    output_lines = output.split('\n')

    l = 0

    while output_lines[l] != 'DEBUG: cheby_on_C':
        l += 1

    l += 1
    cheby_on_C = []
    cheby_on_C_len = int(output_lines[l])

    for i in range(cheby_on_C_len):
        l += 1
        line = output_lines[l].split()
        cell, pv = (float(line[0]), float(line[1]))
        cheby_on_C.append((cell, pv))

    l += 1
    assert output_lines[l] == 'DEBUG: cheby_on_B'

    l += 1
    cheby_on_B = []
    cheby_on_B_len = int(output_lines[l])

    for i in range(cheby_on_B_len):
        l += 1
        line = output_lines[l].split()
        cell, pv = (float(line[0]), float(line[1]))
        cheby_on_B.append((cell, pv))

    l += 1
    result_line = tuple(map(float, output_lines[l].split()))

    return cheby_on_C, cheby_on_B, result_line


# In[39]:


def run_plot(constant_labels, input_labels, rel_dir):
    
    stdout, returncode = run_robust_sizing(**constant_labels, **input_labels)

    print(f'{input_labels} -> {returncode}')

    if not returncode:
        cheby_on_C, cheby_on_B, result_line = get_chebys(stdout)
        print(f'  result={result_line}')

        fig, (ax_on_B, ax_on_C) = plt.subplots(1, 2, figsize=(15, 10))
        fig.suptitle(f'input={input_labels}, result={result_line}')

        ax_on_B.set_title(f'cheby_on_B, n={len(cheby_on_B)}')
        ax_on_B.set(xlabel='cells (kWh)', ylabel='pv (kW)')
        if cheby_on_B:
            cheby_on_B_cells, cheby_on_B_pv = zip(*cheby_on_B)
            ax_on_B.scatter(x=cheby_on_B_cells, y=cheby_on_B_pv, s=1)

        ax_on_C.set_title(f'cheby_on_C, n={len(cheby_on_C)}')
        ax_on_C.set(xlabel='cells (kWh)', ylabel='pv (kW)')
        if cheby_on_C:
            cheby_on_C_cells, cheby_on_C_pv = zip(*cheby_on_C)
            ax_on_C.scatter(x=cheby_on_C_cells, y=cheby_on_C_pv, s=1)

        fig.show()
        fig.savefig(f'{rel_dir}/{input_labels["pv_max"]}_{input_labels["cell_max"]}_{input_labels["epsilon"]}.png')
    else:
        print(f'  robust sizing failed for {input_labels}.')


# In[40]:


constant_labels = {
    'binary': './bin/debug/sim',
    'pv_cost': 2000,
    'battery_cost': 500,
    'metric': 1,
    'conf': 0.95,
    'n_days': 100,
    'load_file': 'example_inputs/new_dheli_load.txt',
    'pv_file': 'example_inputs/new_dheli_pv.txt',
}

rel_dir = 'analytics/new_dheli_macbook'


for epsilon in [0.01, 0.05, 0.1, 0.25, 0.5, 0.75]:
    
    input_labels = {
        'pv_max': 70,
        'cell_max': 225,
        'epsilon': epsilon,
    }
    
    run_plot(constant_labels, input_labels, rel_dir)
    

for pv_max, cell_max in [(10, 20), (100, 300), (150, 300), (150, 500), (1000, 2000)]:
    
    input_labels = {
        'pv_max': pv_max,
        'cell_max': cell_max,
        'epsilon': 0.05,
    }
    
    run_plot(constant_labels, input_labels, rel_dir)


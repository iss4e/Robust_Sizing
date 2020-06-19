#!/usr/bin/env python
# coding: utf-8

# In[1]:


import os
import numpy as np
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

    chunks = []            # [cell, pv, cost]
    cheby_on_C = [[], []]  # cell, pv
    cheby_on_B = [[], []]  # cell, pv

    while output_lines[l] != 'DEBUG: sizing_curves':
        l += 1 # skip

    l += 1 # skip 'DEBUG: sizing_curves'

    while output_lines[l] != 'DEBUG: sizing_curves_end':
        l += 1 # skip 'chunk_n' line

        chunk = [[], [], []] # cell, pv, cost

        while 'chunk' not in output_lines[l] and output_lines[l] != 'DEBUG: sizing_curves_end':
            cell, pv, cost = map(float, output_lines[l].split())
            chunk[0].append(cell)
            chunk[1].append(pv)
            chunk[2].append(cost)
            l += 1

        chunks.append(chunk)

    while output_lines[l] != 'DEBUG: cheby_on_C':
        l += 1

    l += 1 # skip 'cheby_on_C' line

    cheby_on_C_len = int(output_lines[l])
    l += 1 # skip len line

    for _ in range(cheby_on_C_len):
        cell, pv = map(float, output_lines[l].split())
        cheby_on_C[0].append(cell)
        cheby_on_C[1].append(pv)
        l += 1

    while output_lines[l] != 'DEBUG: cheby_on_B':
        l += 1

    l += 1 # skip 'cheby_on_B' line

    cheby_on_B_len = int(output_lines[l])
    l += 1 # skip len line

    for _ in range(cheby_on_B_len):
        cell, pv = map(float, output_lines[l].split())
        cheby_on_B[0].append(cell)
        cheby_on_B[1].append(pv)
        l += 1

    result_line = tuple(map(float, output_lines[l].split()))

    return chunks, cheby_on_B, cheby_on_C, result_line


# In[35]:


def draw(chunks, cheby_on_B, cheby_on_C, title):
    
    fig = plt.figure(figsize=(15, 10))

    plt.title(title)

    plt.xlabel('cells (kWh)')
    plt.ylabel('pv (kW)')

    plt.scatter(x=cheby_on_B[0], y=cheby_on_B[1], c='y', label='cheby_on_B', s=4)
    plt.scatter(x=cheby_on_C[0], y=cheby_on_C[1], c='g', label='cheby_on_C', s=4)

    plt.plot([], [], 'b:', label='chunks')
    for chunk in chunks:
        plt.scatter(x=chunk[0], y=chunk[1], s=1)
        
    plt.legend()
        
    return fig

        
def run_plot(constant_labels, input_labels, save_dir=None):
    
    stdout, returncode = run_robust_sizing(**constant_labels, **input_labels)

    if not returncode:
        chunks, cheby_on_B, cheby_on_C, result_line = get_chebys(stdout)
        
        title = f'input={input_labels}, result={result_line}, len(cheby_on_B)={len(cheby_on_B[0])}, len(cheby_on_C)={len(cheby_on_C[0])}'
        print(title)
        
        fig = draw(chunks, cheby_on_B, cheby_on_C, title)
        
        if save_dir:
            fig.savefig(f'{save_dir}/{input_labels["pv_max"]}_{input_labels["cell_max"]}_{input_labels["epsilon"]}.png')
        else:
            fig.show()
    else:
        print(f'input={input_labels}, FAILED, returncode={returncode}')


# In[36]:


constant_labels = {
    'binary': './bin/debug/sim',
    'pv_cost': 2000,
    'battery_cost': 500,
    'metric': 1,
    'conf': 0.95,
    'n_days': 100,
    'load_file': 'example_inputs/load.txt',
    'pv_file': 'example_inputs/pv.txt',
    # 'load_file': 'example_inputs/new_dheli_load.txt',
    # 'pv_file': 'example_inputs/new_dheli_pv.txt',
    # 'load_file': 'example_inputs/seattle_load.txt',
    # 'pv_file': 'example_inputs/seattle_pv.txt',
}

save_dir = 'analytics/macbook'
# save_dir = 'analytics/new_dheli_macbook'
# save_dir = 'analytics/seattle_macbook'

for epsilon in [0.01, 0.05, 0.1, 0.25, 0.5, 0.75]:
    input_labels = {
        'pv_max': 70,
        'cell_max': 225,
        'epsilon': epsilon,
    }
    run_plot(constant_labels, input_labels, save_dir)
    

for pv_max, cell_max in [(10, 20), (100, 300), (150, 300), (150, 500), (1000, 2000)]:
    input_labels = {
        'pv_max': pv_max,
        'cell_max': cell_max,
        'epsilon': 0.05,
    }
    run_plot(constant_labels, input_labels, save_dir)


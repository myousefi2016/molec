#!usr/bin/env python3
#                   _
#   _ __ ___   ___ | | ___  ___
#  | '_ ` _ \ / _ \| |/ _ \/ __|
#  | | | | | | (_) | |  __/ (__
#  |_| |_| |_|\___/|_|\___|\___| - Molecular Dynamics Framework
#
#  Copyright (C) 2016  Carlo Del Don  (deldonc@student.ethz.ch)
#                      Michel Breyer  (mbreyer@student.ethz.ch)
#                      Florian Frei   (flofrei@student.ethz.ch)
#                      Fabian Thuring (thfabian@student.ethz.ch)
#
#  This file is distributed under the MIT Open Source License.
#  See LICENSE.txt for details.

from pymolec import *

import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import os.path

# seaborn formatting
sns.set_context("notebook", font_scale=1.1)
sns.set_style("darkgrid")
sns.set_palette('deep')
deep = ["#4C72B0", "#55A868", "#C44E52", "#8172B2", "#CCB974", "#64B5CD"]

def measure_performance():

    forces = ['cell_v2'];
    
    N     = np.array([10000, 20000])#, 50000, 100000, 200000, 500000, 1000000]).astype(np.int32)
    steps = np.array([  20,    20])#,    15,     10,     10,     6,      4])
    rhos  = np.array([0.5, 1., 1.5])#, 2., 2.5, 3., 3.5, 4., 4.5, 5., 5.5, 6.])

    rc  = 2.5
    
    if os.path.isfile("performances-grid-forces-density.npy"):
        print("Loading data from <performances-grid-forces-density.npy")
        performances = np.load("performances-grid-forces-density.npy")
        return performances, N, rhos
    else:
        
        performances = np.zeros((len(rhos), len(N)))

        for force in forces:
            for rho_idx, rho in enumerate(rhos):
                flops =  N * rc**3 * rho * (18 * np.pi + 283.5)
                
                p = pymolec(N=N, rho=rho, force=force, steps=steps, integrator='lf', periodic='c4')
                times = p.run()

                perf = flops / times[0,:]
                performances[len(rhos)-1-rho_idx, :] = perf
        
        print("Saving performance data to <performances-grid-forces-density.npy>")
        np.save("performances-grid-forces-density", performances)
    
    return performances, N, rhos
    
def plot_performance(performances, N, rhos):
    fig = plt.figure()
    ax = fig.add_subplot(1,1,1);
    
    # Generate a custom diverging colormap
    cmap = sns.diverging_palette(10, 133, n = 256, as_cmap=True)
    
    ax = sns.heatmap(performances, linewidths=.5,
                      yticklabels=rhos[::-1], xticklabels=N,
                      vmax=1.75, vmin=1.1, cmap=cmap)
    

    rho_labels_short = ['%.2f' % a for a in rhos]
    ax.set_yticklabels(rho_labels_short)
    
    ax.set_xlabel('Number of particles')
    ax.set_ylabel('Particle density',
                    rotation=0, horizontalalignment = 'left')
    ax.yaxis.set_label_coords(0., 1.01)
    plt.yticks(rotation=0) 
    
    filename = 'forces-grid.pdf'
    print("saving '%s'" % filename )
    plt.savefig(filename)


if __name__ == '__main__':
    perf, N, rhos = measure_performance()
    plot_performance(perf, N, rhos)
    
    

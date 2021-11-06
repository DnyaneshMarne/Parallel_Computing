# -*- coding: utf-8 -*-
"""
Created on Wed Oct 20 21:38:41 2021

@author: Dnyanesh
"""


import numpy as np 
from mpi4py import MPI

comm = MPI.COMM_WORLD
rank = comm.Get_rank(); size = comm.Get_size(); N = 64
status = MPI.Status()
rawNum = np.zeros(N, dtype="int")

sortNum = np.zeros(N, dtype="int")
proc_count = np.zeros(size, dtype="int")
if rank == 0:
    rawNum = np.random.randint(0,N,N)
comm.Bcast(rawNum)

counter = 0;
local_min = rank * (N/size);
local_max = (rank + 1) * (N/size); 
print("For rank ", rank, ": min is ", local_min,", max is " ,local_max)
local_bucket = rawNum[np.logical_and(rawNum >= local_min, 
                                       rawNum < local_max)]
#print(local_bucket)
local_bucket.sort()
counter = local_bucket.shape[0]
print("For rank",rank ,"there are", counter," elements in unsorted bucket that falls within max and min")
print("For rank",rank," Local sorted bucket", local_bucket)
countArr = np.array([counter],dtype="int")
comm.Gather(countArr, proc_count, root=0)
disp=np.zeros(size, dtype="int")
if rank == 0:
    disp[0] = 0
    for i in range(1,size):
        disp[i] = disp[i-1] + proc_count[i-1] 
comm.Gatherv(local_bucket,[sortNum,tuple(proc_count),tuple(disp),MPI.INT]) 

if rank == 0:
    print()
    print("Before sort: \n")
    print(rawNum)
    print("\nAfter sort: \n")
    print(sortNum)


#!/usr/bin/python3

import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

# read all the data from a csv file 
# mpi = pd.read_csv("secondary_result.csv")
bScatter = pd.read_csv("B_scatter.csv")
nbScatter = pd.read_csv("NB_scatter.csv")
bSend = pd.read_csv("B_send_Recv.csv")
nbSend = pd.read_csv("NB_send_Recv.csv")
nbSendRecv = pd.read_csv("B_sendRecv.csv")
nbOneSided = pd.read_csv("NB_oneSided.csv")

# print(bScatter)

# extract each col in the data 
# Method 1
commMethod_bScatter = bScatter['Communication Method'].values
sendTime_bScatter = bScatter['Scatter/Send'].values
recvTime_bScatter = bScatter['Gather/Receive'].values
iterations_bScatter = bScatter['Iterations'].values
vecSize_bScatter = bScatter['Vector Size'].values
proc_bScatter = bScatter['Processes'].values
# Method 2
commMethod_nbScatter = nbScatter['Communication Method'].values
sendTime_nbScatter = nbScatter['Scatter/Send'].values
recvTime_nbScatter = nbScatter['Gather/Receive'].values
iterations_nbScatter = nbScatter['Iterations'].values
vecSize_nbScatter = nbScatter['Vector Size'].values
proc_nbScatter = nbScatter['Processes'].values
# Method 3
commMethod_bSend = bSend['Communication Method'].values
sendTime_bSend = bSend['Scatter/Send'].values
recvTime_bSend = bSend['Gather/Receive'].values
iterations_bSend = bSend['Iterations'].values
vecSize_bSend = bSend['Vector Size'].values
proc_bSend = bSend['Processes'].values
# Method 4
commMethod_nbSend = nbSend['Communication Method'].values
sendTime_nbSend = nbSend['Scatter/Send'].values
recvTime_nbSend = nbSend['Gather/Receive'].values
iterations_nbSend = nbSend['Iterations'].values
vecSize_nbSend = nbSend['Vector Size'].values
proc_nbSend = nbSend['Processes'].values
# Method 5
commMethod_nbSendRecv = nbSendRecv['Communication Method'].values
recvTime_nbSendRecv = nbSendRecv['Gather/Receive'].values
sendTime_nbSendRecv = nbSendRecv['Scatter/Send'].values
iterations_nbSendRecv = nbSendRecv['Iterations'].values
vecSize_nbSendRecv = nbSendRecv['Vector Size'].values
proc_nbSendRecv = nbSendRecv['Processes'].values
# Method 6
commMethod_nbOneSided = nbOneSided['Communication Method'].values
sendTime_nbOneSided = nbOneSided['Scatter/Send'].values
recvTime_nbOneSided = nbOneSided['Gather/Receive'].values
iterations_nbOneSided = nbOneSided['Iterations'].values
vecSize_nbOneSided = nbOneSided['Vector Size'].values
proc_nbOneSided = nbOneSided['Processes'].values


# init plot 
fig, (ax1 , ax2) = plt.subplots(1,2, sharey=True,sharex=True, figsize = (10,10), gridspec_kw={'width_ratios': [1,1]})
fig.suptitle("MPI Methods",fontsize=14)

# start the plotting here
# for i in range(commMethod.size):
#     ax1.plot(iterations[i],sendTime[i],  marker='o', label=commMethod[i])
#     ax2.plot(iterations[i],recvTime[i], marker='o', label=commMethod[i])

# ax1.scatter(iterations,sendTime, marker='o', label=commMethod) 
# ax2.scatter(iterations,recvTime, marker='o', label=commMethod)

# ####################################  iterations / send-recv ##########################


# ax1.plot(iterations_bScatter,sendTime_bScatter,marker='o', label="BLOCKING SCATTER")
# ax1.plot(iterations_nbScatter,sendTime_nbScatter,marker='o', label="NONBLOCKING SCATTER")
# ax1.plot(iterations_bSend,sendTime_bSend,marker='o', label="BLOCKING SEND/RECV")
# ax1.plot(iterations_nbSend,sendTime_nbSend,marker='o', label="NONBLOCKING SEND/RECV")
# ax1.plot(iterations_nbSendRecv,sendTime_nbSendRecv,marker='o', label="NONBLOCKING SENDRECV")
# ax1.plot(iterations_nbOneSided,sendTime_nbOneSided,marker='o', label="ONE SIDED")

# ax2.plot(iterations_bScatter,recvTime_bScatter,marker='o', label="BLOCKING SCATTER")
# ax2.plot(iterations_nbScatter,recvTime_nbScatter,marker='o', label="NONBLOCKING SCATTER")
# ax2.plot(iterations_bSend,recvTime_bSend,marker='o', label="BLOCKING SEND/RECV")
# ax2.plot(iterations_nbSend,recvTime_nbSend,marker='o', label="NONBLOCKING SEND/RECV")
# ax2.plot(iterations_nbSendRecv,recvTime_nbSendRecv,marker='o', label="NONBLOCKING SENDRECV")
# ax2.plot(iterations_nbOneSided,recvTime_nbOneSided,marker='o', label="ONE SIDED")

# ax1.set_xlabel('Iterations')
# ax1.set_ylabel('Send time')

# ax2.set_xlabel('Iterations')
# ax2.set_ylabel('Recv time')

# ax1.set_xscale("log", base=2)
# ax1.set_yscale("log", base=2)

# ax2.set_xscale("log", base=2)
# ax2.set_yscale("log", base=2)

# #################################### end of iterations / send-recv ##########################


# #################################### procs / send-recv ##########################

ax1.plot(proc_bScatter,sendTime_bScatter,marker='o', label="BLOCKING SCATTER")
ax1.plot(proc_nbScatter,sendTime_nbScatter,marker='o', label="NONBLOCKING SCATTER")
ax1.plot(proc_bSend,sendTime_bSend,marker='o', label="BLOCKING SEND/RECV")
ax1.plot(proc_nbSend,sendTime_nbSend,marker='o', label="NONBLOCKING SEND/RECV")
ax1.plot(proc_nbSendRecv,sendTime_nbSendRecv,marker='o', label="NONBLOCKING SENDRECV")
ax1.plot(proc_nbOneSided,sendTime_nbOneSided,marker='o', label="ONE SIDED")

ax2.plot(proc_bScatter,recvTime_bScatter,marker='o', label="BLOCKING SCATTER")
ax2.plot(proc_nbScatter,recvTime_nbScatter,marker='o', label="NONBLOCKING SCATTER")
ax2.plot(proc_bSend,recvTime_bSend,marker='o', label="BLOCKING SEND/RECV")
ax2.plot(proc_nbSend,recvTime_nbSend,marker='o', label="NONBLOCKING SEND/RECV")
ax2.plot(proc_nbSendRecv,recvTime_nbSendRecv,marker='o', label="NONBLOCKING SENDRECV")
ax2.plot(proc_nbOneSided,recvTime_nbOneSided,marker='o', label="ONE SIDED")

# labels 
ax1.set_xlabel('Processes')
ax1.set_ylabel('Send time')

ax2.set_xlabel('Processes')
ax2.set_ylabel('Recv time')

# ax1.set_xscale("log", base=10)
ax1.set_yscale("log", base=2)
# ax2.set_xscale("log", base=10)
ax2.set_yscale("log", base=2)

# #################################### end of procs / send-recv ##########################



# #################################### vec size / send-recv ##########################
# ax1.plot(vecSize_bScatter,sendTime_bScatter,marker='o', label="BLOCKING SCATTER")
# ax1.plot(vecSize_nbScatter,sendTime_nbScatter,marker='o', label="NONBLOCKING SCATTER")
# ax1.plot(vecSize_bSend,sendTime_bSend,marker='o', label="BLOCKING SEND/RECV")
# ax1.plot(vecSize_nbSend,sendTime_nbSend,marker='o', label="NONBLOCKING SEND/RECV")
# ax1.plot(vecSize_nbSendRecv,sendTime_nbSendRecv,marker='o', label="NONBLOCKING SENDRECV")
# ax1.plot(vecSize_nbOneSided,sendTime_nbOneSided,marker='o', label="ONE SIDED")

# ax2.plot(vecSize_bScatter,recvTime_bScatter,marker='o', label="BLOCKING SCATTER")
# ax2.plot(vecSize_nbScatter,recvTime_nbScatter,marker='o', label="NONBLOCKING SCATTER")
# ax2.plot(vecSize_bSend,recvTime_bSend,marker='o', label="BLOCKING SEND/RECV")
# ax2.plot(vecSize_nbSend,recvTime_nbSend,marker='o', label="NONBLOCKING SEND/RECV")
# ax2.plot(vecSize_nbSendRecv,recvTime_nbSendRecv,marker='o', label="NONBLOCKING SENDRECV")
# ax2.plot(vecSize_nbOneSided,recvTime_nbOneSided,marker='o', label="ONE SIDED")

# # labels 
# ax1.set_xlabel('Vector Size')
# ax1.set_ylabel('Send time')

# ax2.set_xlabel('Vector Size')
# ax2.set_ylabel('Recv time')


# ax1.set_xscale("log", base=10)
# ax1.set_yscale("log", base=2)

# ax2.set_xscale("log", base=10)
# ax2.set_yscale("log", base=2)

# #################################### end of vec size / send-recv ##########################


# key 
ax1.legend()
ax2.legend()

ax1.yaxis.set_tick_params(labelleft=True)   # shows lables on the 1st plot
ax2.yaxis.set_tick_params(labelleft=True)   # shows labels on the 2nd plot

plt.rcParams['keymap.quit'] = ['q'] # to quit the plot 
plt.show()  # showing the plot figure 





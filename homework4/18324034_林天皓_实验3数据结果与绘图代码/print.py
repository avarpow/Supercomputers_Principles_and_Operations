# -*- coding: utf-8 -*-
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
used_time= np.zeros(shape=(8,12))
used_time = pd.read_csv('usetime.csv').to_numpy()
eff = pd.read_csv('eff.csv').to_numpy()
speedup = pd.read_csv('speedup.csv').to_numpy()
avx = pd.read_csv('avx.csv').to_numpy()
com_num=[1000,10000,100000,1000000,2000000,4000000,1000000000,10000000000,20000000000,40000000000,80000000000,200000000000]
thread_num=[2,4,8,18,36,54,72]
# ####### show thread ~ time    ######### 
k=6
L=[]
L_TEXT=[]
# for i in range(0,6):
#     tem_l,=plt.plot(thread_num[0:k],used_time[0:k,i])
#     L.append(tem_l)
#     plt.xlabel('thread num')
#     plt.ylabel('elapsed  time')
#     L_TEXT.append('n='+str(com_num[i]))

######## show n ~ time    ######### 
# k=12
# L=[]
# L_TEXT=[]
# for i in range(0,7):
#     tem_l,=plt.plot(com_num[0:k],used_time[i,0:k])
#     print(used_time[i,0:k])
#     L.append(tem_l)
#     plt.xlabel('n')
#     plt.ylabel('elapsed  time')
#     L_TEXT.append('thread_num='+str(thread_num[i]))

####### show n ~ efficiently    ######### 
# k=12
# plt.axes(xscale = "log")
# L=[]
# L_TEXT=[]
# for i in range(0,7):
#     tem_l,=plt.plot(com_num[0:k],eff[i,0:k])
#     print(eff[i,0:k])
#     L.append(tem_l)
#     plt.xlabel('n')
#     plt.ylabel('efficiently')
#     L_TEXT.append('thread_num='+str(thread_num[i]))

# ####### show thread ~ efficiently    ######### 
# k=12
# L=[]
# L_TEXT=[]
# for i in range(0,6):
#     tem_l,=plt.plot(thread_num[0:k],eff[0:k,i])
#     L.append(tem_l)
#     plt.xlabel('thread numbers')
#     plt.ylabel('efficiently')
#     L_TEXT.append('n='+str(com_num[i]))

####### show n ~ speedup    ######### 
# k=12
# plt.axes(xscale = "log")
# L=[]
# L_TEXT=[]
# for i in range(0,7):
#     tem_l,=plt.plot(com_num[0:k],speedup[i,0:k])
#     print(speedup[i,0:k])
#     L.append(tem_l)
#     plt.xlabel('n')
#     plt.ylabel('speedup')
#     L_TEXT.append('thread_num='+str(thread_num[i]))

# ####### show thread ~ speedup    ######### 
# k=12
# L=[]
# L_TEXT=[]
# for i in range(0,6):
#     tem_l,=plt.plot(thread_num[0:k],speedup[0:k,i])
#     L.append(tem_l)
#     plt.xlabel('thread numbers')
#     plt.ylabel('speedup')
#     L_TEXT.append('n='+str(com_num[i]))

####### show thread ~ avx    ######### 
# k=12
# plt.axes(xscale = "log")
# L=[]
# L_TEXT=[]
for i in range(0,7):
    tem_l,=plt.plot(com_num[1:k],avx[i,1:k])
    print(avx[i,1:k])
    L.append(tem_l)
    plt.xlabel('n')
    plt.ylabel('avx')
    L_TEXT.append('thread_num='+str(thread_num[i]))

plt.legend(L,L_TEXT,loc='best',fontsize=8)
plt.show()

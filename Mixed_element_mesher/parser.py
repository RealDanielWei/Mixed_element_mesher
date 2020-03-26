# -*- coding: utf-8 -*-
"""
Created on Mon Feb 10 02:15:40 2020

@author: tesla
"""
import re
import matplotlib.pyplot as plt
from matplotlib.lines import Line2D 
import os
import shutil

def show_Poly(index_list):
    fig=plt.figure(figsize=(6,30));
    ax=fig.add_subplot(111)
    all_x=[]
    all_y=[]
    f=open("Poly.txt")
    lines=f.readlines()
    f.close()
    for i in index_list:
        line=lines[2*i+1]
        line=re.findall(r"-?\d+\.?\d*",line)
        x=[]
        y=[]
        for i in range(len(line)//2):
            x.append(float(line[2*i]))
            all_x.append(float(line[2*i]))
            y.append(float(line[2*i+1]))
            all_y.append(float(line[2*i+1]))
        x.append(x[0])
        y.append(y[0])
        ax.add_line(Line2D(x,y))
        
    ax.set_xlim(min(all_x)-0.1*abs(min(all_x)), max(all_x)+0.1*abs(max(all_x)))
    ax.set_ylim(min(all_y)-0.1*abs(min(all_y)), max(all_y)+0.1*abs(max(all_y)))
    plt.show();

def output_ith_poly(index,filename):
    inf=open("Poly.txt")
    lines=inf.readlines()
    inf.close()
    of=open(filename,"w")
    #get z range
    line=lines[2*index]
    line=re.findall(r"\s-?\d+\.?\d*",line)
    of.write(line[0])
    of.write(" ")
    of.write(line[1])
    of.write("\n")
    #get poly nodes
    line=lines[2*index+1]
    line=re.findall(r"-?\d+\.?\d*",line)
    for i in range(len(line)//2):
        of.write(line[2*i])
        of.write(" ")
        of.write(line[2*i+1])
        of.write("\n")
    of.close()
    
def output_polys(index_list,string):
    if(os.path.exists("./inputs")):
        shutil.rmtree("./inputs")
    os.mkdir("./inputs")
    of=open("./inputs/input_list.info","w")
    for index in index_list:
        output_ith_poly(index,"./inputs/"+string+str(index)+".polyrecord")
        of.write(string+str(index)+".polyrecord"+"\n")
    of.close()


show_Poly([0])
#output_ith_poly(2,"poly2.polyrecord")
#output_polys(range(175),"poly")

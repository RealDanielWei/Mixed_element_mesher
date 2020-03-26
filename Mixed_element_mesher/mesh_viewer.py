# -*- coding: utf-8 -*-
"""
Created on Wed Feb 26 18:13:05 2020

@author: tesla
"""
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.collections import LineCollection
from matplotlib.lines import Line2D 
from matplotlib.patches import Circle

def show_multiple_mesh_2d(meshfile_list,color_list):
    fig=plt.figure(figsize=(6,30));
    ax=fig.add_subplot(111)
    all_x=[]
    all_y=[]
    edge_length=[]
    for meshid in range(len(meshfile_list)):
        f=open(meshfile_list[meshid])
        lines=f.readlines()
        f.close()
        segs=[]
        for i in range(len(lines)):
            line=lines[i]
            line=line.split()
            x1=float(line[0])
            y1=float(line[1])
            x2=float(line[2])
            y2=float(line[3])
            edge_length.append(np.sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)))
            edge_length=[min(edge_length),max(edge_length)]
            seg=[[x1,y1],[x2,y2]]
            segs.append(seg)
            all_x.append(x1)
            all_x.append(x2)
            all_y.append(y1)
            all_y.append(y2)
            all_x=[min(all_x),max(all_x)]
            all_y=[min(all_y),max(all_y)]
        ax.add_collection(LineCollection(segs,colors=color_list[meshid]))
        print(meshfile_list[meshid]+" edges: max="+str(max(edge_length))+" min="+str(min(edge_length)))
    ax.set_xlim(min(all_x)-0.1*abs(min(all_x)), max(all_x)+0.1*abs(max(all_x)))
    ax.set_ylim(min(all_y)-0.1*abs(min(all_y)), max(all_y)+0.1*abs(max(all_y)))
    plt.show()

def show_records(record_list):
    fig=plt.figure(figsize=(6,30));
    ax=fig.add_subplot(111)
    x_global=[]
    y_global=[]
    for record in record_list:
        inf=open(record)
        lines=inf.readlines()
        inf.close()
        lines=lines[1:len(lines)]
        x=[]
        y=[]
        for i in range(len(lines)):
            line=lines[i]
            line=line.split()
            x.append(float(line[0]))
            y.append(float(line[1]))
        for xp in x:
            x_global.append(xp)
        for yp in y:
            y_global.append(yp)
        x.append(x[0])
        y.append(y[0])
        ax.add_line(Line2D(x,y))
        
    ax.set_xlim(min(x_global)-0.1*abs(min(x_global)), max(x_global)+0.1*abs(max(x_global)))
    ax.set_ylim(min(y_global)-0.1*abs(min(y_global)), max(y_global)+0.1*abs(max(y_global)))
    plt.show();

def show_metapoly_file(filename):
    inf=open(filename)
    filerecords=inf.readlines()
    inf.close()
    files=[]
    neg_points=[]
    for filerecord in filerecords:
        filerecord=filerecord.split()
        files.append(filerecord[0])
        if(len(filerecord)>1):
            neg_points.append([float(filerecord[1]),float(filerecord[2])])
        else:
            neg_points.append([])
    #start to plot
    fig=plt.figure(figsize=(6,30));
    ax=fig.add_subplot(111)
    x_global=[]
    y_global=[]
    for record in files:
        inf=open(record)
        lines=inf.readlines()
        inf.close()
        lines=lines[1:len(lines)]
        x=[]
        y=[]
        for i in range(len(lines)):
            line=lines[i]
            line=line.split()
            x.append(float(line[0]))
            y.append(float(line[1]))
        for xp in x:
            x_global.append(xp)
        for yp in y:
            y_global.append(yp)
        x.append(x[0])
        y.append(y[0])
        ax.add_line(Line2D(x,y))
    for point in neg_points:
        if(len(point)>0):
            ax.add_patch(Circle((point[0], point[1]),radius=2.5e-6, color='red'))
    ax.set_xlim(min(x_global)-0.1*abs(min(x_global)), max(x_global)+0.1*abs(max(x_global)))
    ax.set_ylim(min(y_global)-0.1*abs(min(y_global)), max(y_global)+0.1*abs(max(y_global)))
    plt.show();
    
#show_multiple_mesh_2d(["background.2dmesh","poly3.2dmesh","bufferzone3.2dmesh","poly4.2dmesh","bufferzone4.2dmesh","poly5.2dmesh","bufferzone5.2dmesh"],['g',"SaddleBrown",'g',"SaddleBrown",'g',"SaddleBrown",'g'])
show_multiple_mesh_2d(["./outputs/background.2dmesh","mixed_ele.2dmesh"],['g','b'])
#show_multiple_mesh_2d(["./outputs/background.2dmesh"],['g'])
#show_records(["gridpoly0.polyrecord","gridpoly1.polyrecord","gridpoly2.polyrecord","gridpoly3.polyrecord","gridpoly4.polyrecord","poly3.polyrecord","poly4.polyrecord","poly5.polyrecord"])
#show_metapoly_file("./outputs/polys.metapolyrecord")
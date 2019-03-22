

#ifndef CARS_SCHEDULER_H
#define CARS_SCHEDULER_H

/*
 * 　华为软件精英挑战赛
 * 　算法调度头文件
 *　 Author：Oldpan
 * 　Updated data：2019-3-16
 * */

#include <iostream>
#include "data_structure.h"

extern unsigned int global_time;

Road* get_optim_cross(Car& car, Cross& cross);
Status MakeCarIntoLane(Cross& cross, Car& car);
Status MakeCarToRoad(Cross& cross, map<int, Car*>& on_road);
Status MakeDijkstraGraph();

/*-图结构体
 * 包含了道路中的各个路口节点(顶点数) 以及道路(边数)
 * 这个图为有向加权图 也是强连通图
 * */
class Graph{

    Graph(){}

private:

    vector<Cross*> all_cross;
    vector<Road*> all_road;


};




















#endif //CARS_SCHEDULER_H



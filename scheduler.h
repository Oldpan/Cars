

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

extern const float COE_ROAD_WEIGHT;          // 每条道路的固有 权重系数
extern const float COE_CARS_WAIT;            // 子道路车辆数 权重系数
extern const float COE_CARS_EMPTY;
extern const float COE_ROAD_LENGTH_WEIGHT;   // 道路的长度权重(占固有权重的比例)
extern const float COE_ROAD_LANE_WEIGHT;
extern const int COE_TIME_BEFORE_CROSS;

extern int COE_CARS_CROSS_NUM;    // 每个时刻从路口等待库中出发的最大车辆
extern int COE_CARS_GARAGE_NUM;   // 每次从不同时刻车库中出发的最大车辆




extern string answer_path;



Road* get_optim_cross(Car& car, Cross& cross);
Status MakeCarIntoLane(Cross& cross, Car& car);
Status MakeCarToRoad(Cross& cross, map<int, Car*>& on_road);
Status Dijkstra(unordered_map<int, Cross*>& all_cross, int curr_cross_id,
        int banned_cross_id, unordered_map<int,float> &dis);
Status gen_route_table(Cross* cross, unordered_map<int, Cross*>& all_cross);
Status MakeDijkstraGraph(unordered_map<int, Cross*>& all_cross);






















#endif //CARS_SCHEDULER_H



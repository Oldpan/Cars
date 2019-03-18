

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
Status MakeCarToRoad(Cross& cross, unordered_map<int, Car*>& on_road);




















#endif //CARS_SCHEDULER_H



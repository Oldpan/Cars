//
// Created by prototype on 19-3-16.
//

#ifndef CARS_RUN_H
#define CARS_RUN_H

/*
 * 　华为软件精英挑战赛
 * 　模拟运行头文件
 * 　模拟运行代码参考于官方示例：https://bbs.huaweicloud.com/forum/thread-14832-1-1.html
 *　 Author：Oldpan
 * 　Updated data：2019-3-16
 * */

#include <ctime>
#include <iostream>
#include <unordered_map>
#include "data_structure.h"
#include "scheduler.h"

const long long MAX_TIME = 500000;




/* 初始化数据　将输入数据读取并创建相关数据结构*/




Status driveCarInGarage();    // 满足条件的车从车库开始行驶
Status run_car_on_cross();
Status run_car_on_road();
Status sche_in_cross(unordered_map<int, Car*>& cars_to_judge, Car* car);
Status MakeCarIntoLaneFromCross(unordered_map<int, Car*>& cars_to_judge, Road* road, Car* car);
Status run_car_on_this_lane(unordered_map<int, Car*>& cars_to_judge, Lane* lane);
Status com_next_dis(Car& car, Road* next_road);
bool check_has_stop_car();


void OwnInitData();
void run();
Status TestDataInit();











#endif //CARS_RUN_H

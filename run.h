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
#include "data_structure.h"

int global_time = 0;   // 上帝时间 从开始调度算起
vector<TGarage&> time_scheduler;  // 时刻表 每个时刻为车辆的出发时间 每个出发时间对应一个子车库
vector<Car&> on_road;   // 此时在道路上的车辆 按时间先后顺序排序

const long long MAX_TIME = 500000;


Status driveCarInGarage();    // 满足条件的车从车库开始行驶






void run();
Status TestDataInit();












#endif //CARS_RUN_H

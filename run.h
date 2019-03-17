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

const long long MAX_TIME = 500000;

extern unsigned int global_time;



/* 初始化数据　将输入数据读取并创建相关数据结构*/
Status initData();



Status driveCarInGarage();    // 满足条件的车从车库开始行驶






void run();
Status TestDataInit();












#endif //CARS_RUN_H

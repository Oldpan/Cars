/*
 * 　华为软件精英挑战赛
 * 　算法调度源文件
 *　 Author：Oldpan
 * 　Updated data：2019-3-16
 * */


#include "scheduler.h"



/* 决策函数 根据此刻车的位置 寻找最佳的下一个路口
 * 从而根据路返回通往该路口的道路
 * */
Road* get_optim_cross(Car& car, Cross& cross)
{
#ifdef DEBUG_MODE

    int road_id = car.first_road();
    // 根据道路id 在这个cross中找到 相应的道路
    auto id_and_road = cross.exist_roads.find(road_id);
    Road* road = id_and_road->second;

    return road;

#else


#endif


}

/*
 * 　华为软件精英挑战赛
 * 　算法调度源文件
 *　 Author：Oldpan
 * 　Updated data：2019-3-16
 * */


#include "scheduler.h"

unsigned int global_time = 0;       // 上帝时间 从开始调度算起


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


/* 决策函数
 * 此函数将此车放入这辆车下一步应该走的车道中*/
Status MakeCarIntoLane(Cross& cross, Car& car)
{
    auto road = get_optim_cross(car, cross);
    // 返回具有正确方向的子道路
    auto subroad_right_dir = road->getSubroad(car);

    // *以下部分可以封装为一个函数 放到scheduler中
    // 得到这个子道路的所有车道 按顺序排列 lanes是指针
    auto lanes = subroad_right_dir->getLane();
    // 从内车道依次遍历到外车道
    int length = road->get_length();

    for (auto &lane : *lanes){
        //　判断当前车道是否有空位可以放
        int i=0;
        for(; i < length; ++i)
        {
            // 如果这个位置有车辆
            if(!lane->is_carport_empty(i))
                break;
        }
        // 如果当前车道第一个位置有车 那么直接进行下一个车道
        if(0==i) continue;
        // 此时得到的是当前车道有车的位置信息

        int max_distance = min(car.get_max_speed(),road->get_limited_speed());  // (6,5)
        int position = min(max_distance-1, i-1);

        // 将这辆车放入指定位置
        Status status = lane->put_car_into(car, position);
        // 如果该车成功上路 则将该车从等待上车map中删除(只是指针而已)　删除后map还是顺序排列
        // 还有另一种实现方式　每次查询该车是否已经上路
        if(status.is_success())
        {
            cerr<<"Car("<<car.get_id()<<") go to road("<<road->get_id()<<")"<<endl;
            // 更新此时车的状态
            car.current_lane_ptr = lane;
            car.current_road_ptr = road;
            car.current_road = road->get_id();
            car.set_road_order(road->get_id());

            // 在map循环中貌似是安全的 需要进一步观察
            cross.remove_car_from_garage(car.get_id());
            break;
        }
    }

    return Status::success();
}


/* 决策函数
 * 这段函数将准备上路的车按照车辆的序号顺序　依次上路*/
Status MakeCarToRoad(Cross& cross, unordered_map<int, Car*>& on_road){

    auto cars = cross.cars_from_garage;

    // 首先遍历每一辆　在路口等待出发的车　此时遍历是有顺序的,按照id升序的方式
    for(auto it = cars.begin(); it != cars.end(); ++it)
    {
        //　查找并返回 当前车的第一个要走的道路id
        auto car = it->second;

        // 如果这辆车的实际出发时间还没到(这个时间是经过算法规划的)
        if(car->get_start_time() >  global_time)
        {
            cerr<<"Car's start time is not right!"<<endl;
            continue;
        }

        Status status = MakeCarIntoLane(cross, *car);
        // 如果车辆成功进入车道 则将此车标记为在道路中的车
        if(status.is_success())
        {
            on_road.insert(mapCar(car->get_id(), car));
            // 设定车的实际出发时间
            car->set_start_time(global_time);
        }
    }

    return Status::success();

}



/*
 * 　华为软件精英挑战赛
 * 　模拟运行头文件
 * 　模拟运行代码参考于官方示例：https://bbs.huaweicloud.com/forum/thread-14832-1-1.html
 *　 Author：Oldpan
 * 　Updated data：2019-3-16
 * */

#include "run.h"

using namespace std;

/*---模拟运行程序中需要的数据结构---*/


vector<TGarage*> time_scheduler;    // 时刻表 每个时刻为车辆的出发时间 每个出发时间对应一个子车库
unordered_map<int, Road*> all_roads;          // 所有的道路信息　这里保存所有道路的原始内容　其余都是引用或指针
unordered_map<int, Cross*> all_cross;         // 所有的路口汇总　这里保存路口的原始内容　其余都是引用或指针
unordered_map<int, Car*> on_road;             // 所有在路上的车辆(不包括在路口中等待的车辆)


/*-检查道路上的所有车是否进入了停止状态
 * 如果全部都是停止状态则返回 True
 * */
bool check_has_stop_car()
{
    if(on_road.empty())
        return false;
    for (auto &car : on_road) {
        if(!car.second->is_init() && !car.second->is_stop() )
            return false;
    }
    return true;
}

/*--车辆通过路口时根据情况计算在下一个路口行驶的距离
 * 将计算出来的信息储存到这辆车的信息中
 * */
Status com_next_dis(Car& car, Road* next_road)
{
    auto current_road = car.current_road_ptr;
    auto V = car.get_max_speed();
    // 当前道路最大行驶速度记为V1 　为当前道路单位时间最大行驶距离
    auto V1 = min(current_road->get_limited_speed(), V);
    auto max_dis_1 = V1;
    // 下一个道路最大行驶速度记为V2 也就是下一条道路单位时间最大行驶距离
    auto V2 = min(next_road->get_limited_speed(), V);
    auto max_dis_2 = V2;

    // 当前道路可以行驶的距离
    auto S1 = car.last_move_dis;

    // 如果在当前道路的行驶距离S1已经大于等于下一条道路的单位时间最大行驶距离V2
    // 则此车辆不能通过路口，只能行进至当前道路的最前方位置，
    if(S1 >= V2)
    {
        car.next_move_dis = 0;
        return Status::success();
    }

    // 如果此时车在上一个道路最前方位置　
    if(0==S1)
    {
        car.next_move_dis = V2;
        return Status::success();
    }

    // 进入下一条道路行驶的距离 最小为0
    auto S2 = max(S1+V2-V1, 0);
    // 条件判断
    if(S2 > (V2-S1 < 0 ? 0:(V2-S1)))
    {
        return MAKE_ERROR("Wrong Value !",ErrorCode::kINVALID_VALUE);
    }
    car.next_move_dis = S2;

    return Status::success();
}

/* 当有一辆车从路口(来自道路R的车道C)成功到达另一条道路时
 * 则对该道路R的车道C上所有车辆进行一次调度*/
Status run_car_on_this_lane(unordered_map<int, Car*>& cars_to_judge, Lane* lane)
{
    auto length = lane->get_length();
    for (int j = length-1; j >= 0; --j)
    {
        // 如果这个位置没有车 则直接跳过
        if(lane->is_carport_empty(j))
            continue;

        auto car_in_lane = lane->get_car(j);

        // 如果该车已经处于停止状态 则直接跳过
        if(car_in_lane->is_stop())
            continue;

        // 查看前面是否有阻挡的车辆
        int pos = j+1;  // pos为阻挡车辆的位置
        for(; pos< length; ++pos)
        {
            if(!lane->is_carport_empty(pos))
                break;
        }

        int max_distance = min(car_in_lane->get_max_speed(),lane->get_max_speed());
        int dis_before_car = pos - j - 1;     // 此时车与前面阻挡车辆的距离 如 (4) 5 6 (7)  7-4-1=2

        // 如果找到的位置和路长度一致 --> 没有阻挡车辆(也包括前面有车但是不会撞上)
        if(pos == length || dis_before_car>=max_distance)
        {
            if(pos == length){
                int dis_before_cross = length - j - 1;  // 此时车与出道路口(路口)的距离
                // 没有阻挡也不回出路口
                if(max_distance <= dis_before_cross)
                {
                    auto new_position = j+max_distance;
                    // 将j位置的车移动到new_position
                    lane->move_car(j,new_position);
                    // 将此车设置为终止状态
                    car_in_lane->set_state(CarStatus::kStop);
                }
                else // 没有阻挡有可能出路口
                {
                    // 从初始顺序中得到车辆的行进顺序
                    auto next_road_id = car_in_lane->get_order_path(car_in_lane->current_road_order+1);

                    // 如果返回下一个道路与这条相同　代表已经车已经行驶完毕 可以宣布这辆车走完了
                    if (next_road_id == car_in_lane->current_road)
                    {
                        // 此时车从道路中移除
                        car_in_lane->remove_from_self_lane();
                        // 这里不移除的原因是为了判断
                        on_road.erase(car_in_lane->get_id());
                        car_in_lane->print_road_track();
                        cerr<<"Car("<<car_in_lane->get_id()<<") is finished!"<<endl;
                        car_in_lane->set_state(CarStatus::kFinish);
                        return Status::success();
                    }

                    Road* next_road = all_roads[next_road_id];
                    com_next_dis(*car_in_lane, next_road);
                    // 如果出不了路口
                    if(car_in_lane->next_move_dis == 0){
                        // 将车从此车道移动至最前面的位置
                        lane->move_car(j, length-1);
                        car_in_lane->set_state(CarStatus::kStop);
                    }
                    else
                    {
//                      // 根据要走的下一条路 设定路口等待方向

                        // 将此车放入第一优先级队列
                        // 注意　这里放入都是当前可以进入路口的第一优先梯队
                        cars_to_judge.insert(mapCar(car_in_lane->get_id(), car_in_lane));
                        car_in_lane->set_wait_dir(next_road);

                        // 返回车道通向的路口
                        auto out_cross_id = lane->get_dir().second;
                        auto cross = all_cross[out_cross_id];

                        // *** 在这里更新了车的当前路口状态
                        car_in_lane->set_curr_cross(*cross);
                    }

                }
            }
            else //前面有车但是不会撞上
            {
                lane->move_car(j,j+dis_before_car);
                car_in_lane->set_state(CarStatus::kStop);
            }

        }
        else// 有车辆阻挡情况
        {
            // 得到前方阻挡的车辆
            auto ahead_car = lane->get_car(pos);
            // 如果阻挡车辆为停止状态
            if(ahead_car->is_stop())
            {
                int new_pos = min(dis_before_car, max_distance);
                lane->move_car(j, new_pos);
                car_in_lane->set_state(CarStatus::kStop);
            }
            // 如果阻挡车辆为等待状态
            if(ahead_car->is_waiting()){
                // 车位置不变 将状态设为等待状态
                car_in_lane->set_state(CarStatus::kWaiting);
            }
        }

    }
    return Status::success();
}


/* 将此车从路口放到下一条道路的正确位置
 *
 * */
Status MakeCarIntoLaneFromCross(unordered_map<int, Car*>& cars_to_judge, Road* road, Car* car)
{
    // 返回具有正确方向的子道路
    auto subroad_right_dir = road->getSubroad(*car);

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
        // 如果当前车道入口处第一个位置有车 那么直接进行下一个车道
        if(0==i) continue;
        // 此时得到的是当前车道有车的位置信息

        int max_distance = car->next_move_dis;
        int position = min(max_distance-1, i-1);

        // 该车临走前的车道　我们要在这辆车走后进行调度 因此这里保留下车道的位置
        auto last_lane = car->current_lane_ptr;
        car->remove_from_self_lane();
        // 将这辆车放入指定位置
        Status status = lane->put_car_into(*car, position);

        // 如果成功
        if(status.is_success())
        {
            cars_to_judge.erase(car->get_id());

            cerr<<"Car("<<car->get_id()<<") from Road("
            <<car->current_road_ptr->get_id()<<") go to Road("
            <<car->next_road_prt->get_id()<<")"<<endl;

            car->set_road_order(road->get_id());

            // 每个道路（道路R）一旦有一辆等待车辆（记为车A，所在车道记为C）
            // 通过路口而成为终止状态，则会该道路R的车道C上所有车辆进行一次调度(类似于第一步中的调度)
            run_car_on_this_lane(cars_to_judge, last_lane);

            return Status::success();
        }
    }

    // 说明车道堵满了车　从道路上不了车道 将状态重新改回等待 默认此车行驶距离为0
    // 之后会重新判断这个车的可行驶距离
    // 并且从当前第一优先级别中删除(可以重新进入)
    // !!! 这里考虑下是否该车的其他状态是否需要调整
    car->set_state(CarStatus::kWaiting);
    cars_to_judge.erase(car->get_id());

    return Status::success();

}



// 这里调度所有从路口中朝同一条道路前进的车辆　
Status sche_in_cross(unordered_map<int, Car*>& cars_to_judge, Car* car)
{
    auto next_road = car->next_road_prt;

    /*
     * 相同道路车辆，不同车道的车辆，车道序号小的优先通行，且必须通行后
     * 下一车道号的车辆才能通行。即使前面车辆是转弯
     * 后面车辆是直行
     * 也必须等待前面的转弯车辆通行后后面的直行车辆才可以通行。*/

    // 从车道小的先来
    for(int order=0; order<50; order++)
    {
        // 直行先行
        for(int i = 0; i < cars_to_judge.size(); ++i)
        {
            if(car->get_state() == CarStatus::kGoStraight && car->get_lane_order()==order)
            {
                MakeCarIntoLaneFromCross(cars_to_judge, next_road, car);
            }
        }

        for(int i = 0; i < cars_to_judge.size(); ++i)
        {
            if(car->get_state() == CarStatus::kGoLeft && car->get_lane_order()==order)
            {
                MakeCarIntoLaneFromCross(cars_to_judge, next_road, car);
            }
        }

        for(int i = 0; i < cars_to_judge.size(); ++i)
        {
            if(car->get_state() == CarStatus::kGoRight && car->get_lane_order()==order)
            {
                MakeCarIntoLaneFromCross(cars_to_judge, next_road, car);
            }
        }

        if(cars_to_judge.empty())
            break;
    }


    return Status::success();
}



/*--调度第一步
 * 该步骤处理所有道路的车辆的顺序，不影响其他道路上车辆的顺序
 * 因此先调度哪条道路无关紧要。*/
Status run_car_on_road()
{
    static map<int, Road*> done_roads;   //已经处理过的车道 标记下来防止处理第二次

    for (auto &it: on_road) {  // !!! 这里面存在on_road车在同一个道路的情况
        // 这个car只是用来确定这个道路有没有车
        auto car = it.second;
        // 得到存在车辆的道路
        auto road = car->current_road_ptr;

        //　如果这条道路已经处理过　则跳过　
        if(done_roads.count(road->get_id()))
            break;

        for(int i = 0; i < 2; ++i)   // ！！！应该考虑下单行道的情况
        {
            // 某一个方向的车道
            auto subroad = road->get_subroad(i);
            auto lanes = subroad->getLane();
            auto road_length = subroad->get_length();
            // 遍历每一排 从第一排开始遍历 注意出道路口为道路第一排
            for (int j = road_length-1; j >= 0; --j) {

                auto lane_num = subroad->get_lane_num();
                // 遍历每一排中的每一个车道 按从内侧开始遍历
                for(int orderOflane = 0; orderOflane < lane_num; ++orderOflane)
                {
                    auto lane = (*lanes)[orderOflane];

                    // 如果这个位置没有车 则直接跳过
                    if(lane->is_carport_empty(j))
                        continue;

                    auto car_in_lane = lane->get_car(j);

                    // 如果该车已经处于停止状态 则直接跳过
                    if(car_in_lane->is_stop())
                        continue;

                    // 查看前面是否有阻挡的车辆
                    int pos = j+1;  // pos为阻挡车辆的位置
                    for(; pos< road_length; ++pos)
                    {
                        if(!lane->is_carport_empty(pos))
                            break;
                    }

                    int max_distance = min(car_in_lane->get_max_speed(),road->get_limited_speed());
                    int dis_before_car = pos - j - 1;     // 此时车与前面阻挡车辆的距离 如 (4) 5 6 (7)  7-4-1=2

                    // 如果找到的位置和路长度一致 --> 没有阻挡车辆(也包括前面有车但是不会撞上)
                    if(pos == road_length || dis_before_car>=max_distance)
                    {
                        if(pos == road_length){
                            int dis_before_cross = road_length - j - 1;  // 此时车与 出道路口(路口) 的距离
                            // 没有阻挡也不回出路口
                            if(max_distance <= dis_before_cross)
                            {
                                auto new_position = j+max_distance;
                                // 将j位置的车移动到new_position
                                lane->move_car(j,new_position);
                                // 将此车设置为终止状态
                                car_in_lane->set_state(CarStatus::kStop);

                            }
                            else // 没有阻挡有可能出路口 暂时设定为等待状态
                            {
                                // 更新一下剩余的距离
                                car_in_lane->last_move_dis = dis_before_cross;
                                car_in_lane->set_state(CarStatus::kWaiting);
                            }
                        }
                        else //前面有车但是不会撞上
                        {
                            lane->move_car(j,j+dis_before_car);
                            car_in_lane->set_state(CarStatus::kStop);
                        }

                    }
                    else// 有车辆阻挡情况
                    {
                        // 得到前方阻挡的车辆
                        auto ahead_car = lane->get_car(pos);
                        // 如果阻挡车辆为停止状态
                        if(ahead_car->is_stop())
                        {
                            int new_pos = min(dis_before_car, max_distance);
                            lane->move_car(j, new_pos);
                            car_in_lane->set_state(CarStatus::kStop);
                        }
                        // 如果阻挡车辆为等待状态
                        if(ahead_car->is_waiting()){
                            // 车位置不变 将状态设为等待状态
                            car_in_lane->set_state(CarStatus::kWaiting);
                        }
                    }

                }

            }

        }

        done_roads.insert(mapRoad(road->get_id(),road));
    }

    done_roads.clear();

    return Status::success();
}

/*--调度第二步骤---*/
Status run_car_on_cross()
{
    // 这里面存放了 可以过路口　第一优先级的车辆
    // 优先级的车辆是可以动态变动的
    static unordered_map<int, Car*> cars_to_judge;

    // 这里默认路口id从１开始(一般来说路口id都是从１开始)
    for (int cross_id = 1; cross_id <= all_cross.size() ; ++cross_id) {
        auto cross = all_cross[cross_id];

        // 按照道路id升序的顺序取每条道路
        // 注意每条路的长度有可能不一样
        for(auto &id_road : cross->exist_roads){
            auto road = id_road.second;
            // 得到正确方向的子道路 这个道路的终点方向为此路口
            auto subroad = road->getSubroad(*cross);
            auto lanes = subroad->getLane();

            // 判断下返回的子道路方向是否正确
            if(subroad->get_dir().second != cross_id )
                return MAKE_ERROR("Retured subroad's dir is wrong!",
                        ErrorCode::kINTERNAL_ERROR);

            auto road_length = road->get_length();
            // 从第一排开始(出口路为第一排)
            for (int j = road_length-1; j >= 0; --j) {

                auto lane_num = subroad->get_lane_num();
                // 遍历每一排中的每一个车道 按从内侧开始遍历
                for(int orderOflane = 0; orderOflane < lane_num; ++orderOflane)
                {
                    auto lane = (*lanes)[orderOflane];

                    // 如果这个位置没有车 则直接跳过
                    if(lane->is_carport_empty(j))
                        continue;

                    auto car_in_lane = lane->get_car(j);

                    // 如果该车已经处于停止状态 则直接跳过
                    if(car_in_lane->is_stop())
                        continue;
                    // 如果这辆车在等待　首先判断前方是否有车辆
                    if(car_in_lane->is_waiting())
                    {
                        // 查看前面是否有阻挡的车辆
                        int pos = j+1;  // pos为阻挡车辆的位置
                        for(; pos< road_length; ++pos)
                        {
                            if(!lane->is_carport_empty(pos))
                                break;
                        }
                        // 前方有车辆
                        if(pos != road_length){

                            auto ahead_car = lane->get_car(pos);

                            // 如果前面阻挡的车在等待出路口(在出路口队列 但是路口队列还没有执行)　那么这辆车状态不变
                            if(ahead_car->get_state() == CarStatus::kWaiting )
                                break;

                            // 如果前面的车停止　说明前面的车过不了路口　在路口的最前面
                            if(ahead_car->get_state() == CarStatus::kStop){
                                int max_distance = min(car_in_lane->get_max_speed(),road->get_limited_speed());
                                int dis_before_car = pos - j - 1;     // 此时车与前面阻挡车辆的距离 如 (4) 5 6 (7)  7-4-1=2
                                // 将此车根据速度移动到指定位置
                                lane->move_car(j,j+dis_before_car);
                                car_in_lane->set_state(CarStatus::kStop);
                                continue;
                            }

                        }
                        else // 前方没车辆
                        {
                            auto next_road_id = car_in_lane->get_order_path(car_in_lane->current_road_order+1);

                            // 如果返回下一个道路与这条相同　代表已经车已经行驶完毕 可以宣布这辆车走完了
                            if (next_road_id == car_in_lane->current_road)
                            {
                                car_in_lane->remove_from_self_lane();
                                on_road.erase(car_in_lane->get_id());
                                car_in_lane->set_state(CarStatus::kFinish);
                                car_in_lane->print_road_track();
                                cerr<<"Car("<<car_in_lane->get_id()<<") is finished!"<<endl;

                                // 想想这个continue会去哪儿
                                continue;
                            }

                            Road* next_road = all_roads[next_road_id];
                            com_next_dis(*car_in_lane, next_road);
                            // 如果出不了路口
                            if(car_in_lane->next_move_dis == 0){
                                // 将车从此车道移动至最前面的位置
                                lane->move_car(j, road_length-1);
                                car_in_lane->set_state(CarStatus::kStop);
                            }
                            else
                            {
//                              // 根据要走的下一条路 设定路口等待方向
//                                car_in_lane->set_wait_dir(next_road);
                                // 将此车放入第一优先级队列
                                // 注意　这里放入都是当前可以进入路口的第一优先梯队
                                cars_to_judge.insert(mapCar(car_in_lane->get_id(), car_in_lane));
                                car_in_lane->set_state(CarStatus::kWaiting);

                                // 将即将通往不同方向的车辆 放到对应的cross链接着的道路中 目前还没有用到这个
                                if(cross->road_up == next_road)
                                    cross->waiting_cars_up.insert(mapCar(car_in_lane->get_id(), car_in_lane));
                                else if(cross->road_right == next_road)
                                    cross->waiting_cars_right.insert(mapCar(car_in_lane->get_id(), car_in_lane));
                                else if(cross->road_down == next_road)
                                    cross->waiting_cars_down.insert(mapCar(car_in_lane->get_id(), car_in_lane));
                                else
                                    cross->waiting_cars_left.insert(mapCar(car_in_lane->get_id(), car_in_lane));

                                // *** 在这里更新了车的当前路口状态
                                car_in_lane->set_curr_cross(*cross);

                            }
                        }
                    }
                }
            }
        }

        // 可以进入该道路的直行车辆、左转车辆、右转车辆的优先级只受直行、左转、右转优先级影响
        // 不受车辆所在位置前后的影响。
        // 继续调整这个路口
        // 确认所有在路口中等待调度的车的方向优先级
        for (auto &car_to_wait_id : cars_to_judge)
        {
            auto car_to_wait = car_to_wait_id.second;
            auto next_road_id = car_to_wait->get_order_path(car_to_wait->current_road_order+1);
            Road* next_road = all_roads[next_road_id];
            // 确认这个车的前进方向(直行　左拐　右拐)
            car_to_wait->set_wait_dir(next_road);
        }

        // 只要处于第一优先的队列不为空 就一直循环调度(第一优先的队列会随着有车辆成功通过路口而更新)
        while(!cars_to_judge.empty())
        {
            // 接下来利用已知道路中车辆的优先调度顺序　和 之前计算出来的信息 使车辆过路口　
            for(auto &id_road : cross->exist_roads){
                auto road = id_road.second;
                // 得到正确方向的子道路 这个道路的终点方向为此路口
                auto subroad = road->getSubroad(*cross);
                auto lanes = subroad->getLane();

                // 判断下返回的子道路方向是否正确
                if(subroad->get_dir().second != cross_id )
                    return MAKE_ERROR("Retured subroad's dir is wrong!",
                                      ErrorCode::kINTERNAL_ERROR);

                auto road_length = road->get_length();
                // 从第一排开始(出口路为第一排)
                for (int j = road_length-1; j >= 0; --j) {

                    auto lane_num = subroad->get_lane_num();
                    // 遍历每一排中的每一个车道 按从内侧开始遍历
                    for(int orderOflane = 0; orderOflane < lane_num; ++orderOflane)
                    {
                        auto lane = (*lanes)[orderOflane];

                        // 如果这个位置没有车 则直接跳过
                        if(lane->is_carport_empty(j))
                            continue;

                        auto car_in_lane = lane->get_car(j);

                        // 如果该车已经处于停止状态 则直接跳过
                        if(car_in_lane->is_stop())
                            continue;

                        // 此时应该没有等待的车辆了 如果还有 说明程序有问题
                        if(car_in_lane->is_waiting())
                            return MAKE_ERROR("This condition can not happen!",
                                              ErrorCode::kINTERNAL_ERROR);

                        // 如果该车已经可以过路口　并且已经在路口待出发库中
                        if(car_in_lane->is_in_cross())
                        {
                            sche_in_cross(cars_to_judge, car_in_lane);
                        }
                    }
                }
            }

        }

    }


}









/*--测试数组，读取文件按照这样的格式来表示*/
/*(id,始发地,目的地,最高速度,出发时间)*/
const vector<vector<int>> CAR = {{1000,2,4,8,3},{1001,1,2,6,1},{1002,4,3,7,2}};

/*(道路id，道路长度，最高限速，车道数目，起始点id，终点id，是否双向)*/
const vector<vector<int>> ROAD = {{500,15,6,2,1,2,1},{501,12,6,3,2,3,1},
                                  {502,10,6,2,3,4,1},{503,8,6,1,4,1,1}};

/*(结点id,道路id,道路id,道路id,道路id)*/
const vector<vector<int>> CROSS = {{1,-1,500,503,-1},{2,-1,-1,501,500},
                                   {3,501,-1,-1,502},{4,503,502,-1,-1}};

/*(车辆id，实际出发时间，行驶路线序列)*/


/*\  　简单测试使用的地图
 * 　　用于检查　逻辑是否正确
 *    1 ----500----2 ----504-----5
 *    -            -             -
 *    -            -             -
 *   503          501           506
 *    -            -             -
 *    -            -             -
 *    4 --- 502----3 ----505----6
 * */

/* 测试数据初始化函数
 * 仅仅用于测试 这里要注意　在子函数中使用new申请的空间在堆中而不是栈
 * 所以需要手动去释放已经申请的空间
 * */
Status TestDataInit()
{

    const vector<vector<int>> ANSWER = {{1000, 3, 500, 503},
                                        {1001, 2, 500     },
                                        {1002, 1, 502     },
                                        {1003, 1, 500, 501},
                                        {1004, 5, 503, 502},
                                        {1005, 1, 501, 502},
                                        {1006, 1, 500, 501},
                                        {1007, 2, 500, 504, 506},
                                        {1008, 2, 502, 505, 506},
                                        {1009, 2, 506, 504, 500}};

    Car* car_1 = new Car(1000,2,4,8,3);
    Car* car_2 = new Car(1001,1,2,6,2);
    Car* car_3 = new Car(1002,4,3,7,1);
    Car* car_4 = new Car(1003,1,3,7,1);
    Car* car_5= new Car(1004,1,3,5,5);
    Car* car_6= new Car(1005,3,1,5,1);
    Car* car_7= new Car(1006,1,3,5,1);

    Car* car_8= new Car(1007,1,6,5,2);
    Car* car_9= new Car(1008,4,5,5,3);
    Car* car_10= new Car(1009,6,1,5,5);

    car_1->setPathOrder(ANSWER[0]);
    car_2->setPathOrder(ANSWER[1]);
    car_3->setPathOrder(ANSWER[2]);
    car_4->setPathOrder(ANSWER[3]);
    car_5->setPathOrder(ANSWER[4]);
    car_6->setPathOrder(ANSWER[5]);
    car_7->setPathOrder(ANSWER[6]);
    car_8->setPathOrder(ANSWER[7]);
    car_9->setPathOrder(ANSWER[8]);
    car_10->setPathOrder(ANSWER[9]);


    Road* road_1 = new Road(500,15,6,2,1,2,1);
    Road* road_2 = new Road(501,12,6,3,2,3,1);
    Road* road_3 = new Road(502,10,6,2,3,4,1);
    Road* road_4 = new Road(503,8,6,2,4,1,1);

    Road* road_5 = new Road(504,10,6,2,2,5,1);
    Road* road_6 = new Road(505,8,6,2,3,6,1);
    Road* road_7 = new Road(506,10,6,5,5,6,1);

    all_roads.insert(mapRoad(road_1->get_id(), road_1));
    all_roads.insert(mapRoad(road_2->get_id(), road_2));
    all_roads.insert(mapRoad(road_3->get_id(), road_3));
    all_roads.insert(mapRoad(road_4->get_id(), road_4));

    all_roads.insert(mapRoad(road_5->get_id(), road_5));
    all_roads.insert(mapRoad(road_6->get_id(), road_6));
    all_roads.insert(mapRoad(road_7->get_id(), road_7));


    auto cross_1 = new Cross(1,-1,500,503,-1);
    auto cross_2 = new Cross(2,-1,-1,501,500);
    auto cross_3 = new Cross(3,501,-1,-1,502);
    auto cross_4 = new Cross(4,503,502,-1,-1);
    auto cross_5 = new Cross(5,-1,-1,506,504);
    auto cross_6 = new Cross(6,506,-1,-1,505);

    cross_1->initCross(all_roads);
    cross_2->initCross(all_roads);
    cross_3->initCross(all_roads);
    cross_4->initCross(all_roads);
    cross_5->initCross(all_roads);
    cross_6->initCross(all_roads);

    all_cross.insert(mapCross(cross_1->get_id(),cross_1));
    all_cross.insert(mapCross(cross_2->get_id(),cross_2));
    all_cross.insert(mapCross(cross_3->get_id(),cross_3));
    all_cross.insert(mapCross(cross_4->get_id(),cross_4));
    all_cross.insert(mapCross(cross_5->get_id(),cross_5));
    all_cross.insert(mapCross(cross_6->get_id(),cross_6));

    road_1->initRoad(all_cross);
    road_2->initRoad(all_cross);
    road_3->initRoad(all_cross);
    road_4->initRoad(all_cross);
    road_5->initRoad(all_cross);
    road_6->initRoad(all_cross);
    road_7->initRoad(all_cross);


    auto garage1 = new TGarage(1,4);         //　先进去的车先出发 此时车库中全是同一时间出发的车辆
    garage1->pushCar(*car_3);
    garage1->pushCar(*car_4);
    garage1->pushCar(*car_6);
    garage1->pushCar(*car_7);

    auto garage2 = new TGarage(2,1);
    garage2->pushCar(*car_2);
    garage2->pushCar(*car_8);

    auto garage3 = new TGarage(3,1);
    garage3->pushCar(*car_1);
    garage3->pushCar(*car_9);

    auto garage4 = new TGarage(5,1);
    garage4->pushCar(*car_5);
    garage4->pushCar(*car_10);

    time_scheduler.push_back(garage1);
    time_scheduler.push_back(garage2);
    time_scheduler.push_back(garage3);
    time_scheduler.push_back(garage4);

    return Status::success();
}


/* 这里将所有等待出发的车辆从子车库中提出来放到路口中　初始化
 * 子车库中的车都是按照id顺序升序排列
 * 路口中的待出发车辆也是按照id顺序 排列 但是可能会有id在后面的车辆实际出发时间提前
 *                             所以实际排列优先级为  出发时间time > 出发车辆id
 * 具体步骤:
 * 1,按照时间片取出这个 时间片的子车库 子车库中的车辆都在这个时间内上路行驶
 * 2,将这个时间片的子车库中的所有车 按照每个车的出发路口id放入每个路口的待出发车库
 * 3,遍历一遍所有的路口 如果路口的待出发车库中有车　那么就这些车上路
 * 4,
 * */
Status driveCarInGarage(unordered_map<int, Car*>& on_road)
{
    static int count;    // 这里记录随时间流逝　子车库的遍历情况　

    if(count == time_scheduler.size())
        return Status::success();

    if(global_time == time_scheduler[count]->time_to_go())
    {
        auto garage_to_go = time_scheduler[count];                      // 从计划车库中取出子车库
        Status status = garage_to_go->driveCarInCross(all_cross);       // 将子车库中的车辆放入各自的出发点路口
        //  这里的 it 遍历一遍路口　(*遍历代码有优化空间)
        for (auto &all_cros : all_cross) {
            //　取出map中的cross
            auto cross = all_cros.second;
           // 如果此时路口没有 等待上路的车辆
            if(cross->is_cfgara_empty())
                continue;

            Status status = MakeCarToRoad(*cross, on_road);
        }
        count += 1;
    }
    return Status::success();
}

void OwnInitData(){

    all_roads = all_roads_id;
    all_roads_id.clear();

    all_cross = all_cross_id;
    all_cross_id.clear();

    auto all_cars = all_car_id;

    // 使用规划好的答案
    if(!answers.empty())
    {
        for (int i=0; i<answers.size(); ++i)
        {
            all_car_f[0]->setPathOrder(answers[0]);
        }
    }

}



/* 运行模拟车辆行驶代码
 * 规则：
 * 1,系统调度先调度在路上行驶的车辆进行行驶，当道路上所有车辆全部不可再行驶后再调度等待上路行驶的车辆
 * 2,调度等待上路行驶的车辆，按等待 车辆ID升序 进行调度，进入道路车道依然按车道小优先进行进入
 * 3,为了简化系统设计，车辆速度设定均小于等于所有道路的最小长度，不考虑一个时间调度一个车辆可以越过一条道路的情况
 *
 *
 *
 * */

/* !!! 要注意是否及时更新所有应该更新的状态  */
void run()
{
    /*--系统先调度在路上行驶的车辆，随后再调度等待上路行驶的车辆*/
    for (global_time = 0; global_time < MAX_TIME; ++global_time){

        /*----第一步主要是调度道路中行驶且不会出路口的情况*/

        if(!on_road.empty())
            run_car_on_road();

        /*----第二步则调度路口中和因为其他原因等待的车辆*/

        run_car_on_cross();

        for (auto& car_id:on_road) {
            auto car = car_id.second;
            car->set_state(CarStatus::kWaiting);
        }

        /*--执行完上面的步骤后,所有在路上的车辆都为等待状态--没有停滞状态的车辆*/
        /*----所有在路上的车调度完毕后才命令车库中的车辆上路行驶*/
        driveCarInGarage(on_road);

    }
}


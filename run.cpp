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


vector<TGarage*> time_scheduler;              // 时刻表 每个时刻为车辆的出发时间 每个出发时间对应一个子车库
unordered_map<int, Road*> all_roads;          // 所有的道路信息　这里保存所有道路的原始内容　其余都是引用或指针
unordered_map<int, Cross*> all_cross;         // 所有的路口汇总　这里保存路口的原始内容　其余都是引用或指针
map<int, Car*> on_road;                       // 所有在路上的车辆(不包括在路口中等待的车辆)


/*-检查道路上的所有车是否进入了停止状态
 * 如果全部都是停止状态则返回 True
 * */
bool check_has_stop_car()
{
    if(on_road.empty())
        return true;
    for (auto &car : on_road) {
        if( !car.second->is_stop() )
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
//    auto S2 = max(S1+V2-V1, 0);    // 此公式有错误
    auto S2 = max(V2-S1, 0);

    // 条件判断
    if(S2 > (V2-S1 < 0 ? 0:(V2-S1)))
    {
        cerr<<"Wrong Value when compute next_move_dis"<<endl;
        return MAKE_ERROR("Wrong Value !",ErrorCode::kINVALID_VALUE);
    }
    car.next_move_dis = S2;

    return Status::success();
}

/* 当有一辆车从路口(来自道路R的车道C)成功到达另一条道路时
 * 则对该道路R的车道C上所有车辆进行一次调度*/
Status run_car_on_this_lane(vector<int>& id_cars, int& count, unordered_map<int, Car*>& cars_to_judge, Lane* lane)
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
                // 没有阻挡也不会出路口
                if(max_distance <= dis_before_cross)
                {
                    auto new_position = j+max_distance;
                    // 将j位置的车移动到new_position
                    lane->move_car(j,new_position);
                    // 将此车设置为终止状态
                    car_in_lane->set_state(CarStatus::kStop);
                }
                else // 没有阻挡有可能出路口 也可能是到终点
                {
                    // 如果该车可以到终点
                    if(car_in_lane->get_end_id() == lane->get_dir().second){

                        // 这里将这个车设为直行 代表这辆车即将结束并且需要参与路口优先级判断
                        car_in_lane->set_state(CarStatus::kGoStraight);
                        // 此车位更新
                        id_cars[count] = car_in_lane->get_id();
                        cars_to_judge.insert(mapCar(car_in_lane->get_id(), car_in_lane));
                        // 计数退回去 因为有新的车可以走了　这个位子还要来一遍
                        count -= 1;
                        // 更新这辆车的当前路口(也就是最终路口)
                        Cross* end_cross = all_cross[car_in_lane->get_end_id()];
                        car_in_lane->set_curr_cross(*end_cross);

                        continue;
                    }

                    // 如果不会到终点
#ifdef DEBUG_MODE
                    auto next_road_id = car_in_lane->get_order_path(car_in_lane->current_road_order+1);
#else
                    // 这个路口是当前车道的出方向
                    int cross_id = lane->get_dir().second;
                    auto curr_cross = all_cross[cross_id];
                    auto next_road = get_optim_cross(*car_in_lane, *curr_cross);
#endif

                    // 如果不是终点但可以过路口
                    car_in_lane->last_move_dis = dis_before_cross;
                    com_next_dis(*car_in_lane, next_road);

                    // 不管能不能出口 都会进行优先级队列进行判断
                    cars_to_judge.insert(mapCar(car_in_lane->get_id(), car_in_lane));
                    // 这里确定了等待的方向
                    car_in_lane->set_wait_dir(next_road);
                    id_cars[count] = car_in_lane->get_id();
                    count -= 1;
                    car_in_lane->set_curr_cross(*curr_cross);

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
                int new_pos = min(dis_before_car, max_distance) + j;
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
Status MakeCarIntoLaneFromCross(vector<int>& id_cars, unordered_map<int, Car*>& cars_to_judge, Road* road,
        Car* car, int& count)
{
    // 如果这辆车到达终点了
    if(car->get_cross_id() == car->get_end_id() && car->is_will_finish()){

        auto last_lane = car->current_lane_ptr;
        car->remove_from_self_lane();
        on_road.erase(car->get_id());
        car->print_road_track();
        cerr<<"Car("<<car->get_id()<<") is finished!"<<endl;

        // 从路口判断中去除
        cars_to_judge.erase(car->get_id());
        id_cars[count] = -1;

        // 每个道路一旦有一辆等待车辆通过路口而成为终止状态(或者到终点而处于结束状态)
        // 则会该道路的车道上所有车辆进行一次调度(类似于第一步中的调度)
        run_car_on_this_lane(id_cars, count, cars_to_judge, last_lane);

        return Status::success();
    }

    // 如果这辆车过不了路口
    if(car->next_move_dis == 0){

        auto curr_lane = car->current_lane_ptr;
        auto curr_lane_length = curr_lane->get_length();
        int j=curr_lane_length-1;
        for(; j>=0; j--)
        {
            if(curr_lane->get_car(j) == car)
                break;
        }

        // 这里的last_move_dis可能为-1？
        if(curr_lane->get_car(j) != car)
            cerr<<"Car find in lane is not the car!"<<endl;
        // 将车移动至最前端
        curr_lane->move_car(j,curr_lane_length-1);
        car->set_state(CarStatus::kStop);

        cars_to_judge.erase(car->get_id());
        id_cars[count] = -1;

        // 每个道路一旦有一辆等待车辆通过路口而成为终止状态(或者到终点而处于结束状态)
        // 则会该道路的车道上所有车辆进行一次调度(类似于第一步中的调度)
        run_car_on_this_lane(id_cars, count, cars_to_judge, curr_lane);

        return Status::success();

    }

    // 如果可以过路口
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

        // 说明下一条道路当前车道没有车
        if(i==length)
        {
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

                // 更新车的状态
                car->current_lane_ptr = lane;
                car->current_road_order += 1;
                car->current_road_ptr = road;
                car->set_road_order(road->get_id());
                car->set_state(CarStatus::kStop);

                id_cars[count] = -1;

                // 每个道路（道路R）一旦有一辆等待车辆（记为车A，所在车道记为C）
                // 通过路口而成为终止状态，则会该道路R的车道C上所有车辆进行一次调度(类似于第一步中的调度)
                run_car_on_this_lane(id_cars, count, cars_to_judge, last_lane);

                return Status::success();
            }
        }

        // 得到当前下一个路口当前车道 处在最前面的车
        Car* ahead_car = lane->get_car(i);
        // 如果这个车是停止状态
        if(ahead_car->is_stop())
        {
            // 而且这个前面挡着的车处于第一个位置 那直接换下一个车道
            if(0==i)
                continue;
            else{
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

                    // 更新车的状态
                    car->current_lane_ptr = lane;
                    car->current_road_order += 1;
                    car->current_road_ptr = road;
                    car->set_road_order(road->get_id());
                    car->set_state(CarStatus::kStop);

                    id_cars[count] = -1;

                    // 每个道路（道路R）一旦有一辆等待车辆（记为车A，所在车道记为C）
                    // 通过路口而成为终止状态，则会该道路R的车道C上所有车辆进行一次调度(类似于第一步中的调度)
                    run_car_on_this_lane(id_cars, count, cars_to_judge, last_lane);

                    return Status::success();
                }
            }
        }
        // 如果当前下一个道路车道的车是等待状态
        if(ahead_car->is_waiting())
        {
            // 那么这辆车的状态重新转化为等待状态
            car->set_state(CarStatus::kWaiting);
            // 已经判断结束 在下一次循环中再进行判断
            cars_to_judge.erase(car->get_id());
            id_cars[count] = -1;
            return Status::success();
        }
    }

    // 如果上不去另一条路,将车移到当前车道出口最前端 并设定为停止状态
    // 首先找这辆车的位置
    auto curr_lane = car->current_lane_ptr;
    auto curr_lane_length = curr_lane->get_length();
    int j=curr_lane_length-1;
    for(; j>=0; j--)
    {
        if(curr_lane->get_car(j) == car)
            break;
    }

    // 这里的last_move_dis可能为-1？
    if(curr_lane->get_car(j) != car)
        cerr<<"Car find in lane is not the car!"<<endl;

    curr_lane->move_car(j,curr_lane_length-1);
    car->set_state(CarStatus::kStop);

    cars_to_judge.erase(car->get_id());
    id_cars[count] = -1;

    // 每个道路一旦有一辆等待车辆通过路口而成为终止状态(或者到终点而处于结束状态)
    // 则会该道路的车道上所有车辆进行一次调度(类似于第一步中的调度)
    run_car_on_this_lane(id_cars, count, cars_to_judge, curr_lane);

    return Status::success();

}



/*--调度第一步
 * 该步骤处理所有道路的车辆的顺序，不影响其他道路上车辆的顺序
 * 因此先调度哪条道路无关紧要。*/
Status run_car_on_road()
{
    static map<int, Road*> done_roads;   //已经处理过的车道 标记下来防止处理第二次
    static vector<int> car_on_road;
    car_on_road.clear();

    // 这里将on_road车的 id 存入 car_on_road 中
    for(auto& car_and_id : on_road)
    {
        auto car = car_and_id.second;
        car_on_road.push_back(car->get_id());
    }

    for (int car_id : car_on_road) {  // !!! 这里面存在on_road车在同一个道路的情况

        // 如果车已经不在了就跳过
        if(!on_road.count(car_id))
            continue;

        auto car = on_road.find(car_id)->second;
        // 得到存在车辆的道路
        auto road = car->current_road_ptr;

        //　如果这条道路已经处理过　则跳过　
        if(done_roads.count(road->get_id()))
            continue;

        auto lane_num = 2;
        if (!road->is_duplex())
            lane_num = 1;

        for(int i = 0; i < lane_num; ++i)   //
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

                                //　如果前面就是终点 这里做修改:
                                // 即将到达终点的车在路口会参与优先级（直左右） 并且当做直行判断
                                if(car_in_lane->get_end_id() == lane->get_dir().second){

                                    // 这里将这个车设为真行 代表这辆车即将结束并且需要参与路口优先级判断
                                    car_in_lane->set_state(CarStatus::kGoStraight);
                                    // 更新这辆车的当前路口(也就是最终路口)
                                    Cross* end_cross = all_cross[car_in_lane->get_end_id()];
                                    car_in_lane->set_curr_cross(*end_cross);

                                    continue;
                                }

                                // 如果前面不是终点
                                // 更新一下剩余的距离
                                car_in_lane->last_move_dis = dis_before_cross;
                                car_in_lane->set_state(CarStatus::kWaiting);
                            }
                        }
                        else //前面有车但是不会撞上
                        {
                            lane->move_car(j,j+max_distance);
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
                            int new_pos = min(dis_before_car, max_distance) + j;
                            lane->move_car(j, new_pos);
                            car_in_lane->set_state(CarStatus::kStop);
                        } else
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
    cars_to_judge.clear();

    // 这里默认路口id从１开始(一般来说路口id都是从１开始)
    static int all_cross_size = all_cross.size();
    for (int cross_id = 1; cross_id <= all_cross_size ; cross_id++) {
        auto cross = all_cross[cross_id];

        // 按照道路id升序的顺序取每条道路
        // 注意每条路的长度有可能不一样
        for(auto &id_road : cross->exist_roads){
            auto road = id_road.second;
            // 得到正确方向的子道路 这个道路的终点方向为此路口
            auto subroad = road->get_InSubroad(*cross);

            if(subroad == nullptr)
                continue;

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
                                continue;

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
#ifdef DEBUG_MODE
                            auto next_road_id = car_in_lane->get_order_path(car_in_lane->current_road_order+1);
#else                       // 车辆没过下一个道路时，默认所在当前道路id为　所在这条道路的入门

                            // 这个路口是准备去的路口
                            auto next_road = get_optim_cross(*car_in_lane, *cross);
                            auto next_road_id = next_road->get_id();
#endif
                            //　存在一些能在这一时刻到终点但是前面有车挡着 会在之后进行调度

                            com_next_dis(*car_in_lane, next_road);

                            // 如果出不了路口
                            if(car_in_lane->next_move_dis == 0){

                                // 即便这辆车出不了路口 也要参与优先级判断
                                cars_to_judge.insert(mapCar(car_in_lane->get_id(), car_in_lane));
                                car_in_lane->next_road_prt = next_road;
                                car_in_lane->set_state(CarStatus::kWaiting);
                            }
                            else
                            {
                                // 将此车放入第一优先级队列
                                // 注意　这里放入都是当前可以进入路口的第一优先梯队
                                cars_to_judge.insert(mapCar(car_in_lane->get_id(), car_in_lane));
                                car_in_lane->next_road_prt = next_road;
                                car_in_lane->set_state(CarStatus::kWaiting);

                                // *** 在这里更新了车的当前路口状态
                                car_in_lane->set_curr_cross(*cross);

                            }
                        }
                    }

                    // 如果这辆车即将到达终点
                    if(car_in_lane->is_will_finish()){
                        // 将此车放入优先级别进行排序 因为此车也要参与优先级判断
                        cars_to_judge.insert(mapCar(car_in_lane->get_id(), car_in_lane));
                        car_in_lane->set_curr_cross(*cross);
                    }
                }
            }
        }

        static vector<int> car_id_in_judge;
        car_id_in_judge.clear();

        // 可以进入该道路的直行车辆、左转车辆、右转车辆的优先级只受直行、左转、右转优先级影响
        // 不受车辆所在位置前后的影响
        // 继续调整这个路口
        // 确认所有在路口中等待调度的车的方向优先级
        for (auto &car_to_wait_id : cars_to_judge)
        {
            Car* car_to_wait = car_to_wait_id.second;

            // 如果这辆车即将到达终点 则不为这辆车设置方向(已经设好)
            if(car_to_wait->is_will_finish()){
                car_id_in_judge.push_back(car_to_wait->get_id());
                continue;
            }

            Road* next_road = car_to_wait->next_road_prt;
            // 确认这个车的前进方向(直行　左拐　右拐)
            car_to_wait->set_wait_dir(next_road);
            // 存放所有存在的有逻辑顺序的车辆标号
            car_id_in_judge.push_back(car_to_wait->get_id());
        }

        if(!cars_to_judge.empty())
        {
            // 按照排数 从第一排开始 最多30排
            for(int row = 1; row < 30; ++row){
                // 按照车道顺序 从内侧开始算起 车道最多20个
                for (int order = 0; order < 20; ++order)
                {
                    static int count = 0;
                    // car_id_in_judge中的数量是固定的　
                    auto length = car_id_in_judge.size();

                    count = 0;
                    for (;;) {

                        auto car_id = car_id_in_judge[count];

                        if(car_id != -1){

                            auto car = cars_to_judge[car_id];
                            auto next_road = car->next_road_prt;

                            if(car->get_state() == CarStatus::kGoStraight && car->get_lane_order()==order
                                                                          && car->get_position_row() == row)

                                MakeCarIntoLaneFromCross(car_id_in_judge, cars_to_judge, next_road, car, count);

                            count += 1;
                            if(count == length) break;
                        } else{
                            count += 1;
                            if(count == length) break;
                        }
                    }

                    count = 0;
                    for (;;) {

                        auto car_id = car_id_in_judge[count];

                        if(car_id != -1){

                            auto car = cars_to_judge[car_id];
                            auto next_road = car->next_road_prt;

                            if(car->get_state() == CarStatus::kGoLeft && car->get_lane_order()==order
                                                                      && car->get_position_row() == row)

                                MakeCarIntoLaneFromCross(car_id_in_judge, cars_to_judge, next_road, car, count);

                            count += 1;
                            if(count == length) break;
                        } else{
                            count += 1;
                            if(count == length) break;
                        }

                    }

                    count = 0;
                    for (;;) {

                        auto car_id = car_id_in_judge[count];

                        if(car_id != -1){

                            auto car = cars_to_judge[car_id];
                            auto next_road = car->next_road_prt;

                            if(car->get_state() == CarStatus::kGoRight && car->get_lane_order()==order
                                                                       && car->get_position_row() == row)

                                MakeCarIntoLaneFromCross(car_id_in_judge, cars_to_judge, next_road, car, count);

                            count += 1;
                            if(count == length) break;
                        } else{
                            count += 1;
                            if(count == length) break;
                        }
                    }
                    // 只要判断的车辆空了立马退出
                    if (cars_to_judge.empty())
                        break;
                }
                // 只要判断的车辆空了立马退出
                if (cars_to_judge.empty())
                    break;
            }

        }

    }

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
Status driveCarInGarage(map<int, Car*>& on_road)
{
    static int count_subgarge;    // 这里记录随时间流逝　子车库的遍历情况　

    if(count_subgarge != time_scheduler.size())
    {
        // 不管车能不能行驶到路口，总之先把所有到点的车放到相应的路口中
        if(global_time >= time_scheduler[count_subgarge]->time_to_go())
        {
            auto garage_to_go = time_scheduler[count_subgarge];              // 从计划车库中取出子车库
            bool is_all_go = garage_to_go->driveCarInCross(all_cross);       // 将子车库中的车辆放入各自的出发点路口
            //  这里的 it 遍历一遍路口　(*遍历代码有优化空间)
            if(is_all_go)
                count_subgarge += 1;
        }

        for(int i=count_subgarge; i < 10; i++)
        {
            auto tgarage = time_scheduler[i];
            // 如果此时车辆出不去
            tgarage->set_all_car_time(global_time + tgarage->time_to_go());
        }
    }

    for (auto &id_cross : all_cross) {
        //　取出map中的cross
        auto cross = id_cross.second;
        // 如果此时路口没有 等待上路的车辆
        if(cross->is_cfgara_empty())
            continue;

        Status status = MakeCarToRoad(*cross, on_road);
    }

    return Status::success();
}

Status no_lock()
{
    if(on_road.empty())
        return Status::success();

//    for(auto& cross_and_id:all_cross)
//    {
//        Cross* cross = cross_and_id.second;
//        if(cross->is_lock())
//        {
//            cerr<<"Cross("<<cross->get_id()<<") Locked!"<<endl;
//            return MAKE_ERROR("Locked!",ErrorCode::kFAIL_CONDITION);
//        }
//    }

    for(auto& road_and_id:all_roads)
    {
        Road* road = road_and_id.second;
        if(road->is_lock())
        {
            cerr<<"Road("<<road->get_id()<<") Locked!"<<endl;
            return MAKE_ERROR("Locked!",ErrorCode::kFAIL_CONDITION);
        }
    }

//    for(auto& car_and_id :on_road)
//    {
//        auto car = car_and_id.second;
//        if(car->is_state_change())
//            return Status::success();
//    }

//    cerr<<"Locked!"<<endl;
//    return MAKE_ERROR("Locked!",ErrorCode::kFAIL_CONDITION);

    return Status::success();
}


// 比较两辆车的发车时间
bool time_comparsion(Car* car1, Car* car2){
    return car1->get_start_time() < car2->get_start_time();
}

// 初始化读文件的数据
void OwnInitData(){

    all_roads = all_roads_id;
    all_roads_id.clear();

    all_cross = all_cross_id;
    all_cross_id.clear();

//    if(!answer.empty())
//    {
//        for (int i = 0; i < all_car_f.size(); ++i)
//            all_car_f[i]->setPathOrder(answer[i]);
//    }

    // 按照预计发车时间顺序排好队
    sort(all_car_f.begin(),all_car_f.end(),time_comparsion);
    // 这个临时车库的内存需要回收(未做)
    auto garage = new TGarage(0);

    // 发现数据集中的车辆出发时间都是 10 以内
    for (auto &car : all_car_f) {

        static int curr_start_time = 0;
        auto start_time = car->get_start_time();

        // 当前标记出发时间和这一辆车的出发时间不同 则来个新的子车库
        if(curr_start_time != start_time)
        {
            garage = new TGarage(start_time);
            time_scheduler.push_back(garage);
            curr_start_time = start_time;
        }

        garage->pushCar(*car);
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

/* !!! 如果出现问题,请检查注意是否及时更新所有应该更新的状态 */
void run()
{

    /*--系统先调度在路上行驶的车辆，随后再调度等待上路行驶的车辆*/
    for (global_time = 1; global_time < MAX_TIME; ++global_time){

        cerr<<"TIME: "<<global_time<<endl;

        /*----第一步主要是调度道路中行驶且不会出路口的情况*/

        if(!on_road.empty())
            run_car_on_road();


        static int count;
        count=0;
        /*----第二步则调度路口中和因为其他原因等待的车辆*/
        while(!check_has_stop_car())
        {
            run_car_on_cross();
            count ++;
            // 判断是否死锁
            if(count > 5){
                Status status = no_lock();
                if(status.is_error())
                    exit(0);
            }
        }



        for (auto& car_id:on_road) {
            auto car = car_id.second;
            car->set_state(CarStatus::kWaiting);
        }

        /*--执行完上面的步骤后,所有在路上的车辆都为等待状态--没有停滞状态的车辆*/
        /*----所有在路上的车调度完毕后才命令车库中的车辆上路行驶*/
        driveCarInGarage(on_road);
    }

}


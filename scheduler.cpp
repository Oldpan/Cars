/*
 * 　华为软件精英挑战赛
 * 　算法调度源文件
 *　 Author：Oldpan
 * 　Updated data：2019-3-27
 * */


#include "scheduler.h"

unsigned int global_time = 0;       // 上帝时间 从开始调度算起




/* 参数信息 */
const float COE_ROAD_WEIGHT = 0.7;         //  每条道路的固有 权重系数
const float COE_CARS_WAIT = 0.2;           //  子道路等待车辆的权重系数
const float COE_CARS_EMPTY = 0.1;          //  子道路空车位车辆的权重系数
const int COE_TIME_BEFORE_CROSS = 100;    //  每多少个单位时间后 从路口中发的车辆永久减少


const float COE_ROAD_LENGTH_WEIGHT = 0.9;   // 道路的长度权重(占固有权重的比例)
const float COE_ROAD_LANE_WEIGHT = 0.1;     // 道路的车道数权重(占固有权重的比例)

// 总体车辆调度参数
int COE_CARS_CROSS_NUM = 1;        // 每个时刻从 每一路口等待库中 出发的最大车辆    5
int COE_CARS_GARAGE_NUM = 30;      // 每一时刻从子车库中 安排上路口的最大车辆       60
string answer_path = "";


/* 决策函数 根据此刻车的位置 寻找最佳的下一个路口
 * 从而根据路返回通往该路口的道路
 * 所有的权重的范围都为0-1
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

    // 权重信息应该有两部分 一部分是已经定好的固定权重 另一部分是车在行驶过程中的动态权重
    // 如果车辆的当前id和终点id一致 说明已经走完
    if(cross.get_id() == car.get_end_id())
        return car.current_road_ptr;

    auto target_cross_id = car.get_end_id();
    // 道路的权重不会超过100
    static float weight;
    weight = 100;
    Road* optim_road = nullptr;
    for (auto& road_and_id : cross.exist_roads)
    {
        auto road = road_and_id.second;
        // 如果此条道路是单方向
        if(!road->is_duplex() && (road->get_start_id() != cross.get_id()))
            continue;
        // 如果此条道路与当前车所在道路id相同则跳过
        if(road == car.current_road_ptr)
            continue;
        auto route_table = cross.get_route_table(road->get_id());
        auto id_and_weight = route_table->find(target_cross_id);
        auto this_weight = id_and_weight->second;
        // 返回从此路口出该道路的子道路
        auto sub_road = road->get_OutSubroad(cross);

        float empty_weight = sub_road->empty_pos_weight();
        float wait_weight = sub_road->waiting_cars_weight();

        this_weight = COE_ROAD_WEIGHT*this_weight
                    + COE_CARS_EMPTY*empty_weight
                    + COE_CARS_WAIT*wait_weight;

        // 选择最小权重
        if(weight > this_weight)
        {
            weight = this_weight;
            optim_road = road;
        }
    }

    return optim_road;

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
//            cerr<<"Car("<<car.get_id()<<") go to road("<<road->get_id()<<")"<<endl;
            // 更新此时车的状态
            car.current_lane_ptr = lane;
            car.current_road_ptr = road;
            car.set_road_order(road->get_id());      // 这里添一句更新当前车辆的路口id

            // 在map循环中貌似是安全的 需要进一步观察
            cross.remove_car_from_garage(car.get_id());
            return Status::success();
        }
    }

//    cerr<<"Can't put car:"<<car.get_id()<< " from garage in lane this time!"<<endl;
    return MAKE_ERROR("Can't put car from garage in lane!",
                      ErrorCode::kFAIL_CONDITION);
}


/* 决策函数
 * 这段函数将准备上路的车按照车辆的序号顺序　依次上路*/
Status MakeCarToRoad(Cross& cross, map<int, Car*>& on_road){

    auto cars = cross.cars_from_garage;

    // 这个时间点此车库已经上路的车辆数目
    static int count_car_to_road;
    count_car_to_road = 0;

    // 存放当前在路口待发车库的车辆id 是有顺序的
    static vector<int> cars_from_garage_id;
    cars_from_garage_id.clear();
    // 每次进行这个函数都会更新一次 因为每次路口待发车库中的车会动态变化的
    for(auto &it : cars)
    {
        auto car = it.second;
        cars_from_garage_id.push_back(car->get_id());
    }

    // 首先遍历每一辆 在路口等待出发的车 此时遍历是有顺序的 按照id升序的方式
    for (auto& car_id : cars_from_garage_id) {

        auto car = cars.find(car_id)->second;

        if(count_car_to_road < COE_CARS_CROSS_NUM){

            Status status = MakeCarIntoLane(cross, *car);
            // 如果车辆成功进入车道 则将此车标记为在道路中的车
            if(status.is_success())
            {
                on_road.insert(mapCar(car->get_id(), car));
                // 设定车的实际出发时间
                car->set_start_time(global_time);
                count_car_to_road ++;
            } else{
                // 让这辆车下一时刻上道
                car->set_start_time(global_time+1);
            }
        } else{
            // 让这辆车下一时刻上道
            car->set_start_time(global_time+1);
        }
    }
    return Status::success();
}

// 对每个路口节点使用算法计算当前最短路径
Status MakeDijkstraGraph(unordered_map<int, Cross*>& all_cross){

    for (auto &cross : all_cross_f)
    {
        gen_route_table(cross, all_cross);
    }

    return Status::success();

}

// 输入原路口的下一个路口,返回去除原路口后到所有路口的最短路
// dis(id,distance)
Status Dijkstra(unordered_map<int, Cross*>& all_cross, int curr_cross_id, int banned_cross_id, unordered_map<int,float> &dis)
{
    priority_queue < pair<float, int>, vector<pair<float, int>>, greater<pair<float, int>>>que;
    dis[curr_cross_id] = 0;
    que.push({ 0,curr_cross_id });
    while (!que.empty()) {
        pair<int, int>p = que.top(); que.pop();
        // v为路口ID
        int v = p.second;
        if (dis.find(v) != dis.end() && dis[v] < p.first)
            continue;
        auto cross_id = all_cross.find(v);
        auto cross = cross_id->second;
        auto roads = cross->exist_roads;

        for (auto &it : roads) {
            auto road = it.second;
            // 权重 到时候可以更改
            auto weight = road->get_weight();
            auto next_cross = road->get_next_cross(cross);
            if (next_cross == nullptr)
                continue;

            auto next_cross_id = next_cross->get_id();
            if(next_cross_id == banned_cross_id)
                continue;
            if (dis.find(next_cross_id) == dis.end() || dis[next_cross_id] > dis[v] + weight) {

//                dis[next_cross_id] = (dis[v] + weight) / static_cast<float>(dis.size());
                dis[next_cross_id] = (dis[v] + weight);
                que.push({ dis[next_cross_id],next_cross_id });
            }
        }
    }
    return Status::success();
}

/*创建路由表*/
Status gen_route_table(Cross* cross, unordered_map<int, Cross*>& all_cross)
{
    auto roads = cross->exist_roads;
    for (auto &it : roads)
    {
        auto road = it.second;
        auto road_id = road->get_id();

        auto weight = road->get_weight();
        auto next_cross = road->get_next_cross(cross);
        if(next_cross == nullptr)
            continue;
        pair<int, int> curr_route = {road_id,weight};
        cross->set_route_table(road_id, curr_route);

        auto curr_road_routes = cross->get_route_table(road_id);
        Dijkstra(all_cross, next_cross->get_id(), cross->get_id(), *curr_road_routes);
        bool have_old = false;
        for (auto old_route_table : *curr_road_routes){
            if(old_route_table.first == road_id){
                have_old = true;
                cross->delete_route_table(road_id, old_route_table);
                cross->set_route_table(road_id, curr_route);
            }
        }
        if(!have_old)
            cross->set_route_table(road_id, curr_route);

        auto curr_route_table = cross->get_route_table(road_id);
        curr_route_table->erase(road_id);
        for (auto& road_route : *curr_route_table)
        {
            road_route.second += weight;
        }
    }
    return Status::success();
}
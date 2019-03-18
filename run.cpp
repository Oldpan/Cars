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
static unordered_map<int, Road*> all_roads;          // 所有的道路信息　这里保存所有道路的原始内容　其余都是引用或指针
static unordered_map<int, Cross*> all_cross;         // 所有的路口汇总　这里保存路口的原始内容　其余都是引用或指针
static unordered_map<int, Car*> on_road;             // 所有在路上的车辆(不包括在路口中等待的车辆)


/*-检查道路上的所有车是否进入了停止状态
 * 如果全部都是停止状态则返回 True
 * */
bool check_has_stop_car()
{
    if(on_road.empty())
        return false;
    for(auto it = on_road.begin(); it != on_road.end(); ++ it)
    {
        if(!(*it).second->is_stop())
            return false;
    }
    return true;
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
void run()
{
    /*--系统先调度在路上行驶的车辆，随后再调度等待上路行驶的车辆*/
    for (global_time = 0; global_time < MAX_TIME; ++global_time){

        /*----第一步主要是调度道路中行驶且不会出路口的情况*/
        while(check_has_stop_car())
        {


        }

        /*----第二步则调度路口中和因为其他原因等待的车辆*/
        while(check_has_stop_car())
        {


        }



        /*--执行完上面的步骤后,所有在路上的车辆都为等待状态--没有停滞状态的车辆*/
        /*----所有在路上的车调度完毕后才命令车库中的车辆上路行驶*/
        driveCarInGarage();
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
const vector<vector<int>> ANSWER = {{1000, 3, 501, 502},{1001, 1, 501, 502},{1002, 1, 501, 502}};


/*
 * 初始化数据，读取文件内的数据并储存到数据结构体中
 * */
Status initData()
{

    // 在读取数据的时候，默认按车辆编号的顺序排序车辆数据　以保证在之后也可以按顺序读取


}



/*\  　简单测试使用的地图
 * 　　用于检查　逻辑是否正确
 *    1 ----500----2
 *    -            -
 *    -            -
 *   503          501
 *    -            -
 *    -            -
 *    4 --- 502----3
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
                                        {1004, 5, 503, 502}};

    Car* car_1 = new Car(1000,2,4,8,3);
    Car* car_2 = new Car(1001,1,2,6,2);
    Car* car_3 = new Car(1002,4,3,7,1);
    Car* car_4 = new Car(1003,1,3,7,1);
    Car* car_5= new Car(1004,1,3,5,5);

    car_1->setPathOrder(ANSWER[0]);
    car_2->setPathOrder(ANSWER[1]);
    car_3->setPathOrder(ANSWER[2]);
    car_4->setPathOrder(ANSWER[3]);
    car_5->setPathOrder(ANSWER[4]);

    Road* road_1 = new Road(500,15,6,2,1,2,1);
    Road* road_2 = new Road(501,12,6,3,2,3,1);
    Road* road_3 = new Road(502,10,6,2,3,4,1);
    Road* road_4 = new Road(503,8,6,1,4,1,1);

    all_roads.insert(mapRoad(road_1->get_id(), road_1));
    all_roads.insert(mapRoad(road_2->get_id(), road_2));
    all_roads.insert(mapRoad(road_3->get_id(), road_3));
    all_roads.insert(mapRoad(road_4->get_id(), road_4));

    auto cross_1 = new Cross(1,-1,500,503,-1);
    auto cross_2 = new Cross(2,-1,-1,501,500);
    auto cross_3 = new Cross(3,501,-1,-1,502);
    auto cross_4 = new Cross(4,503,502,-1,-1);

    cross_1->initCross(all_roads);
    cross_2->initCross(all_roads);
    cross_3->initCross(all_roads);
    cross_4->initCross(all_roads);

    all_cross.insert(mapCross(cross_1->get_id(),cross_1));
    all_cross.insert(mapCross(cross_2->get_id(),cross_2));
    all_cross.insert(mapCross(cross_3->get_id(),cross_3));
    all_cross.insert(mapCross(cross_4->get_id(),cross_4));

    road_1->initRoad(all_cross);
    road_2->initRoad(all_cross);
    road_3->initRoad(all_cross);
    road_4->initRoad(all_cross);

    auto garage1 = new TGarage(1,1);         //　先进去的车先出发 此时车库中全是同一时间出发的车辆
    garage1->pushCar(*car_3);
    garage1->pushCar(*car_4);

    auto garage2 = new TGarage(2,1);
    garage2->pushCar(*car_2);

    auto garage3 = new TGarage(3,1);
    garage3->pushCar(*car_1);

    auto garage4 = new TGarage(5,1);
    garage3->pushCar(*car_5);

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
Status driveCarInGarage()
{
    static int count;    // 这里记录随时间流逝　子车库的遍历情况　　
    if(0 != count)       // 将此条件判断提前可节省部分时间
    {

    }else{               // 如果刚开始

        if(global_time == time_scheduler[count]->time_to_go())
        {
            auto garage_to_go = time_scheduler[count];                      // 从计划车库中取出子车库
            Status status = garage_to_go->driveCarInCross(all_cross);       // 将子车库中的车辆放入各自的出发点路口
            //  这里的 it 遍历一遍路口　(*遍历代码有优化空间)
            for(auto it = all_cross.begin(); it != all_cross.end(); ++it)
            {
                //　取出map中的cross
                auto cross = (*it).second;
               // 如果此时路口没有 等待上路的车辆
                if(cross->is_cfgara_empty())
                    continue;

                Status status = MakeCarToRoad(*cross);
            }
            count += 1;
        }
    }

}
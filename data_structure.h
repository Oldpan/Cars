#include <utility>

/*
 * 　华为软件精英挑战赛
 * 　数据结构头文件
 *　 Author：Oldpan
 * 　Updated data：2019-3-16
 * */

#ifndef CARS_DATA_STRUCTURE_H
#define CARS_DATA_STRUCTURE_H

#define DEBUG_MODE    // 如果是debug模式,那么使用预先设定好的车辆路径,而不是利用算法决策

#include <iostream>
#include <queue>
#include <map>
#include <unordered_map>
#include "utils.h"
#include "scheduler.h"

using namespace std;

/*--- 这里展示在这个文件中定义好的类，仅仅是声明　---*/
class Cross;
class SubRoad;
class Road;
class TGarage;
class Car;










/*---辅助代码,简化代码编写量----*/
using mapCar = pair<int, Car*>;
using mapRoad = pair<int, Road*>;
using mapCross = pair<int, Cross*>;









/*
 * 记录信息类
 * 用于运行时输出信息，同时记录需要输出的信息数据
 *　
 * */

template <typename T>
class MyLogger:public Logger{
public:
    explicit MyLogger(Severity severity = Severity::kWARNING)
    : car_severity(severity){}

    void log(Severity severity, const char* msg) override
    {
        // suppress messages with severity enum value greater than the reportable
        if (severity > car_severity)
            return;

        switch (severity)
        {
            case Severity::kINTERNAL_ERROR: std::cerr << "INTERNAL_ERROR: "; break;  // cerr意为无缓冲快速地输出信息
            case Severity::kERROR: std::cerr << "ERROR: "; break;
            case Severity::kWARNING: std::cerr << "WARNING: "; break;
            case Severity::kINFO: std::cerr << "INFO: "; break;
            default: std::cerr << "UNKNOWN: "; break;
        }
        std::cerr << msg << std::endl;
    }

    Severity car_severity;
};


enum class CarStatus{
    kInit = 0,         //　车辆刚刚初始化　在车库中等待出发
    kGoStraight = 1,   // 　此时这辆车在路口等待时　原本的行驶方式是直行
    kGoLeft = 2,
    kGoRight = 3,
    kWaiting = 4,      //  此刻车辆正在等待　一次调度能使所有车辆均到达各车辆的行驶速度行驶，保证不能出现各车辆循环等待的情况，否则该次调度就会锁死
    kStop = 5,         //　此刻车辆标记为终止状态　说明这辆车已经走过了　
};


/*　车辆类　
 *
 *
 * */

class Car{
public:
    explicit Car(int id, int start_id,
                 int end_id, int max_speed,
                 int start_time)
        : _id(id), _start_id(start_id),
          _end_id(end_id), _max_speed(max_speed),
          _start_time(start_time) {}


public:

    Status goIntoCross(Cross &);       // 车辆进入出发点路口 进入等待状态
    Status setPathOrder(const vector<int>& car_answer);

    bool is_stop();
    bool is_waiting();
    bool is_init();
    int first_road() const;        // 返回车辆上路的第一条路的ID


    int current_road = -1;         //　当前所在道路　如果为-1则不在任何道路
    int current_lane = -1;
    int current_speed;

    int next_road;             // 车辆下一时间点要走的路口　
    int next_lane;             // 车辆下一时间点要走的路口
    int last_move_dis;         // 在上一个道路行驶的距离 也可以理解为在当前道路行驶的距离
    int next_move_dis;         // 到了下一个路口要行使的距离(在上一个路口已经行驶过了一段距离)

    int get_id() const;
    int get_start_id() const;
    int get_end_id() const;
    int get_max_speed() const;
    int get_start_time() const;
    int get_cross_id() const;    // 返回当前所在路口id
    CarStatus get_state() const;

private:
    int _id;
    int _start_id;
    int _end_id;
    int _max_speed;
    int _start_time;
    int _current_corss_id;
    vector<int> _path_order;                        // 记录车辆行驶的顺序　记录道路的id
    CarStatus _current_state=CarStatus::kInit;      // 当前车辆的行驶状态

};

/* 车道类　车道的长度和所在道路一致
 * */
class Lane{
public:
    explicit Lane(int length, pair<int,int> dir)
        :  _length(length), _current_dir(std::move(dir)) {}

    Status initLane();           // 初始化当前的车道 将车道中填满虚假的车辆 不知道是否存在空间优化
    pair<int, int> get_dir();    //　得到当前车道的方向

private:

    int _length;                     //　当前车道的长度
    pair<int, int>  _current_dir;    //  当前车道的方向
    map<int, Car*> _cars;

};



/*　子道路类　
 *  对于双行道的道路，拥有两个子道路，其中子道路的方向在初始化的时候是固定的
 *　
 * */

class SubRoad{
public:
    explicit SubRoad(int num, int length, int from, int to){  //　注意这里转递过来的是引用
        _current_dir = make_pair(from, to);
        _num = num;
        _length = length;
    }

    Status initSubRoad();
    Lane* getLane();


private:    //　为了测试将私有隐掉　
    pair<int, int>  _current_dir;    // 当前这个道路的方向　cross.id -> cross.id
    int _num;                        // 有多少车道
    int _length;                     //　子道路有多长
    vector<Lane*> _lanes;            //　当前这个子道路有几个车道 按车道升序方式排列

};


/*　道路类　
 *  根据题目中定义设计的道路类
 *
 * */

class Road{
public:
    explicit Road(int id, int length,
                  int limited_speed,
                  int lane_num,
                  int start_id, int end_id,
                  bool is_duplex)
         :  _id(id), _length(length),
            _limited_speed(limited_speed),
            _lane_num(lane_num),
            _start_id(start_id), _end_id(end_id),
            _is_duplex(is_duplex) {}

    Status initRoad(unordered_map<int, Cross*> all_cross);
    Cross* left_corss = nullptr;     // 这里定义left_cross为start_id
    Cross* right_cross = nullptr;    // 这里定义right_cross为end_id

    int get_id() const;
    int get_length() const;
    int get_limited_speed() const;
    int get_start_id() const;
    int get_end_id() const;
    bool is_duplex() const;
    SubRoad* getSubroad(Car& car);      // 返回正确方向的子道路

//private:
    int _id;
    int _length;
    int _limited_speed;
    int _lane_num;
    int _start_id;
    int _end_id;
    bool _is_duplex;
    // 对于下面两个指针 后期可以加上常量指针返回函数　(指针指向的地址不变)
    SubRoad* _subroad_1 = nullptr;
    SubRoad* _subroad_2 = nullptr;

};


/* 路口类　
 * 出路口的车辆会集中在路口参与路口的优先级排序
 * 优先运行已经在道路上运行的车辆　在运行等待上路的行驶的车辆
 * */
class Cross{
public:
    explicit Cross(int id,
                   int road_up, int road_right,
                   int road_down, int road_left)
         :  _id(id),
            _road_up(road_up),
            _road_right(road_right),
            _road_down(road_down),
            _road_left(road_left) {}


    Road* road_up = nullptr;
    Road* road_right = nullptr;
    Road* road_down = nullptr;
    Road* road_left = nullptr;
    // 以下三个成员变量都是需要顺序的 因此使用了map
    map<int, Road*> exist_roads;      // 该道路连接的所有路口　按照id升序进行排序 map中find的时间复杂度为logn
    map<int, Car*> waiting_cars;      // 定义从上个道路过来经过这个路口的车辆
    map<int, Car*> cars_from_garage;  // 从车库中进入路口等待上道的车辆汇总

public:

    int get_id() const;
    bool is_cfgara_empty();                         // 检查刚上路的预备车辆是否为空
    bool is_wait_empty();
    Status initCross(unordered_map<int, Road*>& all_roads);   // 初始化路口参数
    Status pushCar(Car& car);                       //　将车辆输入到路口中
    Status pCar_to_road();


//private:
    int _id;
    int _road_up;
    int _road_right;
    int _road_down;
    int _road_left;
};


/* 特定时间命令车出发的子车库
 * */
class TGarage{

public:
    explicit TGarage(int sTime, int numOfcar)
        : _time_to_go(sTime), num_of_cars(numOfcar) {}

    Status pushCar(Car& car);       // 将车装入车库
    Status driveCarInCross(unordered_map<int, Cross*>& all_cross);

    int time_to_go() const;

private:

    int _time_to_go = 0;
    int num_of_cars = 0;
    vector<Car*> cars;      // 存放这一时间点要走的车辆信息 读取信息创建对象　存在到这里
                            // 注意，这里是第一次实际存放车的位置 之后访问车辆都从这里访问
};







#endif //CARS_DATA_STRUCTURE_H

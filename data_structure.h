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
#include <cmath>
#include "utils.h"


using namespace std;


/*--- 这里展示在这个文件中定义好的类，仅仅是声明　---*/
class Cross;
class Lane;
class SubRoad;
class Road;
class TGarage;
class Car;



extern unordered_map<int, Road*> all_roads_id;       // 所有的道路信息　这里保存所有道路的原始内容　其余都是引用或指针
extern unordered_map<int, Cross*> all_cross_id;      // 所有的路口汇总　这里保存路口的原始内容　其余都是引用或指针
extern unordered_map<int, Car*> all_car_id;          // 所有在路上的车辆(不包括在路口中等待的车辆)

extern vector<Road*> all_roads_f;       // 所有的道路信息　这里保存所有道路的原始内容　其余都是引用或指针
extern vector<Cross*> all_cross_f;      // 所有的路口汇总　这里保存路口的原始内容　其余都是引用或指针
extern vector<Car*> all_car_f;          // 所有在路上的车辆(不包括在路口中等待的车辆)
extern vector<vector<int>> answer;     // 每辆车的答案　按照id顺序排列


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
    kFinish = 6        // 　表示车辆已经到达目的地
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

    explicit Car(vector<int> init);


public:

    Status goIntoCross(Cross &);       // 车辆进入出发点路口 进入等待状态
    Status setPathOrder(const vector<int>& car_answer);

    bool is_stop();
    bool is_waiting();
    bool is_init();
    bool is_finish();
    bool is_in_cross();
    int first_road() const;             // 返回车辆上路的第一条路的ID

    int current_road = -1;              // 当前所在道路　如果为-1则不在任何道路
    Lane* current_lane_ptr = nullptr;
    Road* current_road_ptr = nullptr;   // 准备要进去的下一条道路
    int current_road_order=0;           //　当前在path_order中所处的位置
    int current_speed;

    Road* next_road_prt = nullptr;        // 车辆下一时间点要走的路口　
    Lane* next_lane_prt = nullptr;        // 车辆下一时间点要走的车道
    int last_move_dis = -1;               // 在上一个道路行驶的距离 也可以理解为在当前可道路行驶的距离
    int next_move_dis = -1;               // 到了下一个路口要行使的距离 为0则不通过路口


    int get_id() const;
    int get_start_id() const;
    int get_end_id() const;
    int get_max_speed() const;
    int get_start_time() const;           // 得到出发时间
    int get_cross_id() const;             // 返回当前所在路口id
    int get_order_path(int order) const;  // 得到规划好的路径中的一个位置
    int get_lane_order() const;

    CarStatus get_state() const;
    Status set_start_time(int time);       // 设置新的出发时间
    Status set_wait_dir(Road* next_road);    // 根据下一个道路决定车的转向
    Status set_curr_cross(Cross& cross);
    Status remove_from_self_lane();         // 将车从自身所在的当前车道删除
    void set_state(CarStatus status);

    vector<int>* get_road_order();
    bool set_road_order(int road_id);
    void print_road_track();

private:
    int _id;
    int _start_id;
    int _end_id;
    int _max_speed;
    int _start_time;
    int _current_corss_id;
    vector<int> _path_order;                        // 记录车辆默认的顺序　记录道路的id
    vector<int> _output_road_order;                 // 动态得出的顺序
    CarStatus _current_state=CarStatus::kInit;      // 当前车辆的行驶状态

};

/* 车道类　车道的长度和所在道路一致
 * */
class Lane{
public:
    explicit Lane(int length, pair<int,int> dir, int max_speed)
        :  _length(length), _current_dir(std::move(dir)),
           _max_speed(max_speed) {}

    Status initLane();           // 初始化当前的车道 将车道中填满虚假的车辆 不知道是否存在空间优化
    bool is_carport_empty(int position);
    int get_length() const;
    int get_max_speed() const;
    int get_order() const;
    bool set_order(int order);

    Status put_car_into(Car& car, int position);
    Car* get_car(int position);
    pair<int, int> get_dir() const;

    Status set_road_id(int id);
    Status move_car(int last_pos, int new_pos);
    Status remove_car(int position);


private:

    int _length;                     //　当前车道的长度
    pair<int, int>  _current_dir;    //  当前车道的方向
    map<int, Car*> _cars;
//    vector<Car*> _cars;              // 车道是有顺序的　
    int _max_speed;
    int _road_id;                    // 当前车道所在的道路id
    int _order;                      // 当前车道的序号

};



/*　子道路类　
 *  对于双行道的道路，拥有两个子道路，其中子道路的方向在初始化的时候是固定的
 *　
 * */

class SubRoad{
public:
    explicit SubRoad(int num, int length, int from, int to, int max_speed){  //　注意这里转递过来的是引用
        _current_dir = make_pair(from, to);
        _num = num;
        _length = length;
        _max_speed = max_speed;
    }

    bool initSubRoad();
    vector<Lane*>* getLane();
    int get_lane_num() const;
    pair<int, int> get_dir() const;
    Status set_road_id(int id);
    int get_length() const;


private:    //　为了测试将私有隐掉　
    pair<int, int>  _current_dir;    // 当前这个道路的方向　cross.id -> cross.id
    int _num;                        // 有多少车道
    int _length;                     //　子道路有多长
    vector<Lane*> _lanes;            //　当前这个子道路有几个车道 按车道升序方式排列
    int _road_id;                    // 当前子车道所在的道路id
    int _max_speed;

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

    Road(vector<int> init);

    Status initRoad(unordered_map<int, Cross*>& all_cross);
    Cross* left_cross = nullptr;     // 这里定义left_cross为start_id
    Cross* right_cross = nullptr;    // 这里定义right_cross为end_id

    int get_id() const;
    int get_length() const;
    int get_limited_speed() const;
    int get_start_id() const;
    int get_end_id() const;
    SubRoad* get_subroad(int subroad_id);
    bool is_duplex() const;
    bool has_car() const;
    SubRoad* getSubroad(Car& car);      // 返回根据车辆位置以该位置为出发点的道路
    SubRoad* getSubroad(Cross& cross);  // 根据路口返回出路口方向的子道路

private:
    int _id;
    int _length;
    int _limited_speed;
    int _lane_num;
    int _start_id;
    int _end_id;
    bool _is_duplex;
    int _has_car=0;    // 当前在这个道路中的车的数量 不为0即有
    // 对于下面两个指针 后期可以加上常量指针返回函数　(指针指向的地址不变)
    SubRoad* _subroad_1 = nullptr;
    SubRoad* _subroad_2 = nullptr;

};


/* 路口类　
 * 出路口的车辆会集中在路口参与路口的优先级排序
 * 优先运行已经在道路上运行的车辆　然后再运行等待上路的行驶的车辆
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

    explicit Cross(vector<int> init);

    // 以下四个指针可以用于判断车的行走方向
    Road* road_up = nullptr;
    Road* road_right = nullptr;
    Road* road_down = nullptr;
    Road* road_left = nullptr;
    // 以下三个成员变量都是需要顺序的 因此使用了map
    map<int, Road*> exist_roads;      // 该道路连接的所有路口　按照id升序进行排序 map中find的时间复杂度为logn
    map<int, Car*> waiting_cars;      // 定义从上个道路过来经过这个路口的车辆 会不断变化 为第一优先级
    map<int, Car*> cars_from_garage;  // 从车库中进入路口等待上道的车辆汇总

public:

    int get_id() const;
    bool is_cfgara_empty();                                   // 检查刚上路的预备车辆是否为空
    bool is_wait_empty();
    vector<Cross*>* get_connected_cross();
    Status remove_car_from_garage(int car_id);
    Status initCross(unordered_map<int, Road*>& all_roads);   // 初始化路口参数
    Status pushCar(Car& car);                                 //　将车辆输入到路口中

private:
    int _id;
    int _road_up;
    int _road_right;
    int _road_down;
    int _road_left;
    vector<Cross*> _connected_cross;
};

/*!! 因为初始化需要设定一个析构函数(未做)*/
/* 特定时间命令车出发的子车库
 * */
class TGarage{

public:
    explicit TGarage(int sTime)
        : _time_to_go(sTime) {}

    Status pushCar(Car& car);       // 将车装入车库
    Status driveCarInCross(unordered_map<int, Cross*>& all_cross);

    int time_to_go() const;

private:

    int _time_to_go = 0;
    vector<Car*> cars;      // 存放这一时间点要走的车辆信息 读取信息创建对象　存在到这里
                            // 注意，这里是第一次实际存放车的位置 之后访问车辆都从这里访问
};


class DataLoader{

public:
    DataLoader(string path_of_car_txt,
               string path_of_road_txt,
               string path_of_cross_txt,
               string path_of_answer_txt = nullptr):
            s_in_car(std::move(path_of_car_txt)),
            s_in_road(std::move(path_of_road_txt)),
            s_in_cross(std::move(path_of_cross_txt)),
            s_in_answer(path_of_answer_txt){}

    ~DataLoader() = default;
    void init();

private:

    string s_in_car;
    string s_in_road;
    string s_in_cross;
    string s_in_answer;

};


#endif //CARS_DATA_STRUCTURE_H

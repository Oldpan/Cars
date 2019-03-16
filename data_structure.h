/*
 * 　华为软件精英挑战赛
 * 　数据结构头文件
 *　 Author：Oldpan
 * 　Updated data：2019-3-16
 * */

#ifndef CARS_DATA_STRUCTURE_H
#define CARS_DATA_STRUCTURE_H



#include <iostream>
#include <queue>
#include <map>
#include "utils.h"

using namespace std;


/*--- 这里展示在这个文件中定义好的类，仅仅是声明　---*/
class Cross;

class SubRoad;
class Road;

class Car;



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
    kRunning = 0,      //　此刻车辆正在前进 每次车辆调度，该车辆要么不走，要么行驶其可行驶的最大车速
    kGoStraight = 1,
    kGoLeft = 2,
    kGoRight = 3,
    kWaiting = 4,      //  此刻车辆正在等待　一次调度能使所有车辆均到达各车辆的行驶速度行驶，保证不能出现各车辆循环等待的情况，否则该次调度就会锁死
    kStop = 5,         //　此刻车辆被终止　
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
    CarStatus current_state;      // 当前车辆的行驶状态
    int current_road;
    int current_lane;
    int current_speed;

    /*-- 暂时不用 但以后可能会用到 --*/
    int next_road;
    int next_lane;

//private:
    int _id;
    int _start_id;
    int _end_id;
    int _max_speed;
    int _start_time;

};

/*　子道路类　
 *  对于双行道的道路，拥有两个子道路，其中子道路的方向在初始化的时候是固定的
 *　
 * */

class SubRoad{
public:
    explicit SubRoad(int num, int from, int to){  //　注意这里转递过来的是引用
        _current_dir = make_pair(from,to);
        _lane = new map<int, Car&>[num];
    }

//private:    //　为了测试将私有隐掉　
    pair<int, int>  _current_dir;    // 当前这个道路的方向　cross.id -> cross.id
    map<int, Car&>* _lane;   //　当前这个子道路有几个车道

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

    Status initRoad();
    Cross* left_corss = nullptr;
    Cross* right_cross = nullptr;

//private:
    int _id;
    int _length;
    int _limited_speed;
    int _lane_num;
    int _start_id;
    int _end_id;
    bool _is_duplex;
    SubRoad* _subroad_1 = nullptr;
    SubRoad* _subroad_2 = nullptr;

};


/* 路口类　
 *
 *
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

//private:
    int _id;
    int _road_up;
    int _road_right;
    int _road_down;
    int _road_left;
};




#endif //CARS_DATA_STRUCTURE_H

/*
 * 　华为软件精英挑战赛
 * 　数据结构yuanwenjian
 *　 Author：Oldpan
 * 　Updated data：2019-3-16
 * */

#include "data_structure.h"

/*-   注意的地方
 * 在所有的数据结构中,如果没有特别说明,返回的一般都是指针,请不要随意修改原指针的内容!
 *
 *
 * */





/*--------------------------------Car类方法----------------------------------*/

int Car::get_id() const {
    return _id;
}

int Car::get_start_id() const{
    return _start_id;
}

bool Car::is_stop(){
    return (CarStatus::kStop == _current_state);
}

bool Car::is_waiting(){
    return (CarStatus::kWaiting == _current_state);
}

bool Car::is_init(){
    return (CarStatus::kInit == _current_state);
}

int Car::first_road() const {
    return _path_order[0];
}

/*
 * 车辆从车库　进入出发点路口　进入等待状态
 * */
Status Car::goIntoCross(Cross& cross)
{
    cross.pushCar(*this);
    return Status::success();
}

Status Car::setPathOrder(const vector<int>& car_answer)
{
    if(car_answer[0] != _id){
        cerr<<"Set Path car ID is different from source car ID"<<endl;
        return MAKE_ERROR("Set Path car ID is different from source car ID",
                          ErrorCode::kINVALID_VALUE);
    }
    for (auto it = car_answer.begin()+2; it != car_answer.end(); ++it) {
        _path_order.push_back(*it);
    }
    return Status::success();
}


/*-------------------------------SubLane类方法--------------------------------*/

/*--车道类　车道的长度和所在道路一致--*/
Status Lane::initLane()
{
    for (int i = 0; i < _length; ++i)
    {
        // 这里将车道中 首先充满了　空的车　先占上车位
        _cars.insert(mapCar(i, nullptr));
    }

    return Status::success();
}


/*---得到当前道路的方向---*/
pair<int, int> Lane::get_dir()
{
    return _current_dir;
}



/*-------------------------------SubRoad类方法--------------------------------*/

Status SubRoad::initSubRoad()
{
    for (int i = 0; i < _num; ++i) {
        auto lane = new Lane(_length, _current_dir);
        lane->initLane();
        _lanes.push_back(lane);
    }

    return Status::success();
}


/*---------------------------------Road类方法---------------------------------*/

/*初始化道路　与子道路绑定*/
Status Road::initRoad()
{
    if(is_duplex()){
        // 如果是双行道路
        //　则默认第一条子道路是从起始点方向到终点方向
        _subroad_1 = new SubRoad(_lane_num, _length,_start_id, _end_id);
        _subroad_1->initSubRoad();
        _subroad_2 = new SubRoad(_lane_num, _length, _end_id, _start_id);
        _subroad_2->initSubRoad();
    } else{
        _subroad_1 = new SubRoad(_lane_num, _length, _start_id, _end_id);
        _subroad_1->initSubRoad();
    }

//    cerr<<"Message"<<endl;

    return Status::success();
}

int Road::get_id() const{
    return _id;
}

bool Road::is_duplex() const{
    return _is_duplex;
}


/*这个函数返回当前道路->有特定方向的子道路*/
SubRoad* get_subroad(Car& car){


}



/*---------------------------------Cross类所有方法-------------------------------*/
int Cross::get_id() const{
    return _id;
}

bool Cross::is_cfgara_empty(){
    return cars_from_garage.empty();
}

bool Cross::is_wait_empty(){
    return waiting_cars.empty();
}

/*
 * ---初始化与路口相连的道路信息-----*/
Status Cross::initCross(map<int, Road*>& all_roads){

    if(_road_up != -1){
        auto id_road = all_roads.find(_road_up);
        road_up = id_road->second;
        exist_roads.insert(mapRoad(_road_up,road_up));
    }
    if(_road_right != -1){
        auto id_road = all_roads.find(_road_right);
        road_right = id_road->second;
        exist_roads.insert(mapRoad(_road_right,road_right));
    }
    if(_road_down != -1){
        auto id_road = all_roads.find(_road_down);
        road_down = id_road->second;
        exist_roads.insert(mapRoad(_road_down,road_down));
    }
    if(_road_left != -1){
        auto id_road = all_roads.find(_road_left);
        road_left = id_road->second;
        exist_roads.insert(mapRoad(_road_left,road_left));
    }

    return Status::success();
}

/*--这段函数将准备上路的车按照车辆的序号顺序　依次上路*/
Status Cross::pCar_to_road(){
    // 首先遍历每一辆　在路口等待出发的车
    for (auto it = cars_from_garage.begin(); it != cars_from_garage.end(); ++it) {

        //　查找并返回 当前车的第一个走的道路id
        auto id_and_car = (*it).second;
        int road_id = id_and_car->first_road();

        // 根据道路id 在这个cross中找到 相应的道路
        auto id_and_road = exist_roads.find(road_id);
        Road* road = id_and_road->second;



    }

}




Status Cross::pushCar(Car& car){
    // 对于到达时间准备出发的车辆　首先将其放到出发地路口
    if(car.is_init())
        cars_from_garage.insert(mapCar(car.get_id(), &car));
    // 对于可以通过路口的车辆(已经进行了计算)
    // 我们将其放入待更新路口　
    if(car.is_waiting())
        waiting_cars.insert(mapCar(car.get_id(), &car));

    return Status::success();

}








/*--------------------------------TGarage类方法--------------------------------*/

int TGarage::time_to_go() const{
    return _time_to_go;
}


Status TGarage::driveCarInCross(map<int, Cross*>& all_cross)
{
    //  首先将所有此时刻的车辆全部送入　相应出发路口进行等待
    for (auto it = cars.begin(); it != cars.end()  ; it++) {

        int cross_id =  (*it)->get_start_id();
        auto cross = all_cross.find(cross_id);    //　从所有的路口中找到需要的路口
        Cross* to_go_cross = cross->second;       //  取出路口
        (*it)->goIntoCross(*to_go_cross);
    }
}

Status TGarage::pushCar(Car& car)
{
    cars.push_back(&car);
    return Status::success();
}




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

int Car::get_end_id() const{
    return _end_id;
}

int Car::get_max_speed() const{
    return _max_speed;
}

int Car::get_start_time() const{
    return _start_time;
}

int Car::get_cross_id() const{
    return _current_corss_id;
}

CarStatus Car::get_state() const{
    return _current_state;
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

/* 车辆在刚从车库出发 或者从道路中出来
 * 进入某一路口　进入等待状态
 * */
Status Car::goIntoCross(Cross& cross)
{
    cross.pushCar(*this);
    _current_corss_id = cross.get_id();
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

/*----返回正确方向的车道----*/
Lane* getLane(){

}


/*---------------------------------Road类方法---------------------------------*/

/*初始化道路　与子道路绑定*/
Status Road::initRoad(unordered_map<int, Cross*> all_cross)
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

    auto id_and_road_left = all_cross.find(_start_id);
    left_corss = id_and_road_left->second;

    auto id_and_road_right = all_cross.find(_end_id);
    left_corss = id_and_road_right->second;

//    cerr<<"Message"<<endl;

    return Status::success();
}

int Road::get_id() const{
    return _id;
}

int Road::get_length() const{
    return _length;
}

int Road::get_limited_speed() const{
    return _limited_speed;
}

int Road::get_start_id() const{
    return _start_id;
}

int Road::get_end_id() const{
    return _end_id;
}

bool Road::is_duplex() const{
    return _is_duplex;
}


/* 这个函数根据输入车辆的信息
 * 返回当前道路->有特定方向的子道路
 * 根据车此时的位置,判断方式也是不同：
 * 1,
 * 2,如果此时车在路口中,则直接返回下一个道路的有方向的子道路
 * */
SubRoad* Road::getSubroad(Car& car){

    // 说明此时车刚从车库中出来 在路口处等待出发
    if(car.get_state() == CarStatus::kInit)
    {

        if(!_is_duplex)
            return _subroad_1;
        else{
            // 如果当前车所在的路口id和这条路的起始点id一致
            if(car.get_cross_id() == _start_id)
            // 因为默认第一条子道路为从起止点到重点
                return _subroad_1;
            else
                return _subroad_2;
        }




    }




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
Status Cross::initCross(unordered_map<int, Road*>& all_roads){

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
    // 首先遍历每一辆　在路口等待出发的车　此时遍历是有顺序的,按照id升序的方式
    for (auto &it : cars_from_garage) {

        //　查找并返回 当前车的第一个走的道路id
        auto car = it.second;

//        int road_id = car->first_road();
//        // 根据道路id 在这个cross中找到 相应的道路
//        auto id_and_road = exist_roads.find(road_id);
//        Road* road = id_and_road->second;

        auto road = get_optim_cross(*car, this);
        // 返回具有正确方向的子道路
        auto subroad_right_dir = road->getSubroad(*car);



    }


    return Status::success();

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


Status TGarage::driveCarInCross(unordered_map<int, Cross*>& all_cross)
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




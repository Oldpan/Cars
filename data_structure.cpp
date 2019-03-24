/*
 * 　华为软件精英挑战赛
 * 　数据结构yuanwenjian
 *　 Author：Oldpan
 * 　Updated data：2019-3-16
 * */

#include "data_structure.h"
#include "scheduler.h"

/*-   注意的地方
 * 在所有的数据结构中,如果没有特别说明,返回的一般都是指针,请不要随意修改原指针的内容!
 *
 *
 * */

unordered_map<int, Road*> all_roads_id;       // 所有的道路信息　这里保存所有道路的原始内容　其余都是引用或指针
unordered_map<int, Cross*> all_cross_id;      // 所有的路口汇总　这里保存路口的原始内容　其余都是引用或指针
unordered_map<int, Car*> all_car_id;

vector<Road*> all_roads_f;       // 所有的道路信息　这里保存所有道路的原始内容　其余都是引用或指针
vector<Cross*> all_cross_f;      // 所有的路口汇总　这里保存路口的原始内容　其余都是引用或指针
vector<Car*> all_car_f;
vector<vector<int>> answer;     // 每辆车的答案　按照id顺序排列


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

int Car::get_order_path(int order) const{
    // 如果下一条路已经没有了 则返回最后一条道
    if(order == _path_order.size())
        return _path_order[order-1];
    else
        return _path_order[order];
}

int Car::get_lane_order() const{

    auto lane = current_lane_ptr;
    return lane->get_order();

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

bool Car::is_finish(){
    return (CarStatus::kFinish == _current_state);
}

bool Car::is_in_cross(){
    return (CarStatus::kGoStraight == _current_state ||
            CarStatus::kGoRight    == _current_state ||
            CarStatus::kGoLeft     == _current_state);
}

// 处于等待状态的车辆的状态是否发生改变
bool Car::is_state_change(){

    return ((_last_state == CarStatus::kWaiting || _last_state == CarStatus::kInit)
            && _last_state != _current_state);
}


int Car::first_road() const {

    return _path_order[0];
}

Status Car::set_start_time(int time){

//    if(time > _start_time)
//        cerr<<"new start time ahead of initial start time"<<endl;
    _start_time = time;
    return Status::success();

}

// 确定过路口车的方向
Status Car::set_wait_dir(Road* next_road){

    // 更新此车的下一个道路
    next_road_prt = next_road;

    // 首先寻找两个道路的交叉点也就是路口
    auto curr_road = current_road_ptr;
    Cross* cross;
    if(next_road->left_cross == curr_road->left_cross ||
       next_road->left_cross == curr_road->right_cross)

        cross = next_road->left_cross;

    else

        cross = next_road->right_cross;

    /*     up
     * left  right
     *    down
     * */
    if(cross->road_up == curr_road && cross->road_down == next_road)
    {
        _current_state = CarStatus::kGoStraight;
        return Status::success();
    }
    if(cross->road_up == curr_road && cross->road_left == next_road)
    {
        _current_state = CarStatus::kGoRight;
        return Status::success();
    }
    if(cross->road_up == curr_road && cross->road_right == next_road)
    {
        _current_state = CarStatus::kGoLeft;
        return Status::success();
    }


    if(cross->road_right == curr_road && cross->road_up == next_road)
    {
        _current_state = CarStatus::kGoRight;
        return Status::success();
    }
    if(cross->road_right == curr_road && cross->road_down == next_road)
    {
        _current_state = CarStatus::kGoLeft;
        return Status::success();
    }
    if(cross->road_right == curr_road && cross->road_left == next_road)
    {
        _current_state = CarStatus::kGoStraight;
        return Status::success();
    }


    if(cross->road_down == curr_road && cross->road_up == next_road)
    {
        _current_state = CarStatus::kGoStraight;
        return Status::success();
    }
    if(cross->road_down == curr_road && cross->road_right == next_road)
    {
        _current_state = CarStatus::kGoRight;
        return Status::success();
    }
    if(cross->road_down == curr_road && cross->road_left == next_road)
    {
        _current_state = CarStatus::kGoLeft;
        return Status::success();
    }


    if(cross->road_left == curr_road && cross->road_up == next_road)
    {
        _current_state = CarStatus::kGoLeft;
        return Status::success();
    }
    if(cross->road_left == curr_road && cross->road_right == next_road)
    {
        _current_state = CarStatus::kGoStraight;
        return Status::success();
    }
    // !有可能存在所有情况都到不了的？
    if(cross->road_left == curr_road && cross->road_down == next_road)
    {
        _current_state = CarStatus::kGoRight;
        return Status::success();
    }

    return Status::success();


//    if(next_road->left_cross == curr_road->left_cross)
//        cross = next_road->left_cross;
//    else
//        if(next_road->left_cross == curr_road->right_cross)
//            cross = next_road->left_cross;
//        else
//            if(next_road->right_cross == curr_road->right_cross)
//                cross = next_road->right_cross;
//            else
//                if(next_road->right_cross == curr_road->left_cross)
//                    cross = next_road->right_cross;
}

Status Car::set_curr_cross(Cross& cross){

    _current_corss_id = cross.get_id();
//    cerr<<"Car:"<<_id<<" change cross id to:"<<_current_corss_id<<endl;
    return Status::success();

}

/*将车从当前所在的车道中移除*/
Status Car::remove_from_self_lane(){

    auto lane = current_lane_ptr;
    for (int i = 0; i < lane->get_length(); ++i) {
        auto lane_car = lane->get_car(i);
        if(lane_car == this)
        {
            lane->remove_car(i);
            break;
        }
    }
    return Status::success();
}

void Car::set_state(CarStatus status){
    _last_state = _current_state;
    _current_state = status;
}

vector<int>* Car::get_road_order(){

    return &_output_road_order;
}

bool Car::set_road_order(int road_id){

   _output_road_order.push_back(road_id);
    return true;
}

void Car::print_road_track(){
    cerr<<"Car("<<_id<<") Road Track:"<<" (";
    auto size = _output_road_order.size();
    for (int i = 0; i < size; ++i)
    {
        if(i==size-1)
        {
            cerr<<_output_road_order[i]<<")"<<endl;
            break;
        }
        cerr<<_output_road_order[i]<<",";
    }
}

Car::Car(vector<int> init){

    _id = init[0];
    _start_id = init[1];
    _end_id = init[2];
    _max_speed = init[3];
    _start_time = init[4];

}




/* 车辆在刚从车库出发 或者从道路中出来
 * 进入某一路口　进入等待状态
 * */
Status Car::goIntoCross(Cross& cross)
{
    cross.pushCar(*this);
    // 此时更新车的状态
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
    _start_time = car_answer[1];
    return Status::success();
}


/*--------------------------------Lane类方法---------------------------------*/
/*--车道类　车道的长度和所在道路一致--*/
Status Lane::initLane()
{
    // 注意这里虽然使用了 map 但是id是按照车位顺序来的而不是车的id
    for (int i = 0; i < _length; ++i)
    {
        // 这里将车道中 首先充满了　空的车　先占上车位
        _cars.insert(mapCar(i, nullptr));
    }

    return Status::success();
}



/* 查看当前车道中的某一位置是否为空
 * 下标从0开始*/
bool Lane::is_carport_empty(int position){
    return _cars[position] == nullptr;
}

int Lane::get_length() const{
    return _length;
}

int Lane::get_max_speed() const{
    return _max_speed;
}

int Lane::get_order() const{
    return _order;
}

bool Lane::set_order(int order){
    _order = order;
    return true;
}

Status Lane::set_road_id(int id){
    _road_id = id;
    return Status::success();
}

/*--移动当前车道中车的 位置--*/
Status Lane::move_car(int last_pos, int new_pos)
{
    auto car = _cars[last_pos];
    if(last_pos > new_pos)
        cerr<<"Car:"<<car->get_id()<<" last_pos > new_pos! Wrong!"<<endl;
    _cars.find(last_pos)->second = nullptr;
    _cars.find(new_pos)->second = car;

    return Status::success();

}

Status Lane::remove_car(int position){

    _cars[position] = nullptr;
    return Status::success();
}

Status Lane::put_car_into(Car& car, int position){

    if(_cars[position] != nullptr)
    {
        cerr<<"The position in lane already has a car in it!"<<endl;
        return MAKE_ERROR("The position in lane already has a car in it!",
                          ErrorCode::kINVALID_VALUE);
    }

    _cars[position] = &car;
    return Status::success();
}

Car* Lane::get_car(int position){
    return _cars[position];
}

pair<int, int> Lane::get_dir() const{
    return _current_dir;
}

/*-------------------------------SubRoad类方法--------------------------------*/

bool SubRoad::initSubRoad()
{
    for (int i = 0; i < _num; ++i) {
        auto lane = new Lane(_length, _current_dir, _max_speed);
        lane->set_order(i);
        lane->initLane();
        lane->set_road_id(_road_id);
        _lanes.push_back(lane);
    }
    return true;
}

/*----返回正确方向的车道----*/
vector<Lane*>* SubRoad::getLane(){
    return &_lanes;
}

int SubRoad::get_lane_num() const{
    return _num;
}

pair<int, int> SubRoad::get_dir() const{
    return _current_dir;
}

Status SubRoad::set_road_id(int id){
    _road_id = id;
    return Status::success();
}

int SubRoad::get_length() const{
    return _length;
}

const int SubRoad::get_cars(){

    int num = 0;
    for (auto& lane : _lanes){

        for(int i = 0; i < _length; ++i)
        {
            if(!lane->is_carport_empty(i))
                num += 1;
        }
    }
    return num;

}

/*---------------------------------Road类方法---------------------------------*/
Road::Road(vector<int> init){

    _id = init[0];
    _length = init[1];
    _limited_speed = init[2];
    _lane_num = init[3];
    _start_id = init[4];
    _end_id = init[5];
    _is_duplex = (bool)init[6];

}


/*初始化道路　与子道路绑定*/
Status Road::initRoad(unordered_map<int, Cross*>& all_cross)
{
    if(is_duplex()){
        // 如果是双行道路
        //　则默认第一条子道路是从起始点方向到终点方向
        _subroad_1 = new SubRoad(_lane_num, _length,_start_id, _end_id, _limited_speed);
        _subroad_1->set_road_id(_id);
        _subroad_1->initSubRoad();
        _subroad_2 = new SubRoad(_lane_num, _length, _end_id, _start_id, _limited_speed);
        _subroad_2->set_road_id(_id);
        _subroad_2->initSubRoad();
    } else{
        _subroad_1 = new SubRoad(_lane_num, _length, _start_id, _end_id, _limited_speed);
        _subroad_1->set_road_id(_id);
        _subroad_1->initSubRoad();
    }

    auto id_and_road_left = all_cross.find(_start_id);
    left_cross = id_and_road_left->second;

    auto id_and_road_right = all_cross.find(_end_id);
    right_cross = id_and_road_right->second;

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

SubRoad* Road::get_subroad(int subroad_id){
    if(!is_duplex())
        return _subroad_1;
    else{
        if(0==subroad_id)
            return _subroad_1;
        else
            return _subroad_2;
    }
}

bool Road::is_duplex() const{
    return _is_duplex;
}

int Road::get_weight() const{

    // 这里返回的权重为道路长度和限速的比值
    return int(_length/_limited_speed);
}






/* 这个函数根据输入车辆的信息
 * 返回当前道路->有特定方向的子道路
 * 根据车此时的位置,判断方式也是不同：
 * ! 这个方式仅仅用与刚从路口出发时候进行判断
 * 如果此时车在路口中,则直接返回下一个道路的有方向的子道路
 * */
SubRoad* Road::getSubroad(Car& car){

//    // 说明此时车刚从车库中出来 在路口处等待出发
//    if(car.get_state() == CarStatus::kInit)
//    {

    //　根据当前车所处的id和子道路的起点id确定该返回哪个子道路
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

//    }

}


// 根据路口 返回出路口方向的子道路
SubRoad* Road::get_OutSubroad(Cross& cross){

    auto cross_id = cross.get_id();
    if (_is_duplex)
    {
        if(cross_id == _start_id)
            return _subroad_1;
        else
            return _subroad_2;
    } else{
        if (cross_id == _start_id)
            return _subroad_1;
        else{
//            cerr<<"The subroad in this road does't exist! Wrong!"<<endl;
            return nullptr;
        }
    }
}

//  根据路口 返回入路口方向的子道路
SubRoad* Road::get_InSubroad(Cross& cross){

    auto cross_id = cross.get_id();
    if (_is_duplex)
    {
        if(cross_id == _start_id)
            return _subroad_2;
        else
            return _subroad_1;
    } else{
        if (cross_id != _start_id)
            return _subroad_1;
        else{
//            cerr<<"The subroad in this road does't exist! Wrong!"<<endl;
            return nullptr;
        }
    }
}


/*根据原路口返回当前道路另一端的路口*/
/*这里只返回当前路口可以通过该路到达的另一个路口
 * 如果无法到达则返回空指针*/
Cross* Road::get_next_cross(Cross* curr_cross){

    if (_is_duplex)
    {
        if(_start_id == curr_cross->get_id())
            return right_cross;
        else
            return left_cross;
    } else{
        if (_start_id == curr_cross->get_id())
            return right_cross;
        else
            return nullptr;
    }
}


/*---------------------------------Cross类所有方法-------------------------------*/
Cross::Cross(vector<int> init){

    _id = init[0];
    _road_up = init[1];
    _road_right = init[2];
    _road_down = init[3];
    _road_left = init[4];

}

int Cross::get_id() const{
    return _id;
}

bool Cross::is_cfgara_empty(){
    return cars_from_garage.empty();
}

bool Cross::is_wait_empty(){
    return waiting_cars.empty();
}

// 返回与这个路口相连的所有路口指针
// 注意这些路口只包含 可以从该路口经过道路到达的这些路口
// 对于该路口和另一路口只有一条道路链接 但是从该路口过不了另一路口(因为道路方向由另一路口指向当前路口)
vector<Cross*>* Cross::get_connected_cross(){

    return &_connected_cross;

}


Status Cross::remove_car_from_garage(int car_id)
{
    if(cars_from_garage.count(car_id))
        cars_from_garage.erase(car_id);
    else
        return MAKE_ERROR("Try to remove a car doesn't exist!",
                ErrorCode::kINVALID_VALUE);

    return Status::success();
}

/*
 * ---初始化与路口相连的道路信息-----*/
Status Cross::initCross(unordered_map<int, Road*>& all_roads)
{

    if(_road_up != -1){
        auto id_road = all_roads.find(_road_up);
        road_up = id_road->second;
        exist_roads.insert(mapRoad(_road_up,road_up));

        if(road_up->get_start_id() == _id)
            _connected_cross.push_back(road_up->right_cross);
        else{
            if (road_up->is_duplex())
                _connected_cross.push_back(road_up->left_cross);
        }
    }

    if(_road_right != -1){
        auto id_road = all_roads.find(_road_right);
        road_right = id_road->second;
        exist_roads.insert(mapRoad(_road_right,road_right));

        if(road_right->get_start_id() == _id)
            _connected_cross.push_back(road_right->right_cross);
        else{
            if (road_right->is_duplex())
                _connected_cross.push_back(road_right->left_cross);
        }
    }

    if(_road_down != -1){
        auto id_road = all_roads.find(_road_down);
        road_down = id_road->second;
        exist_roads.insert(mapRoad(_road_down,road_down));

        if(road_down->get_start_id() == _id)
            _connected_cross.push_back(road_down->right_cross);
        else{
            if (road_down->is_duplex())
                _connected_cross.push_back(road_down->left_cross);
        }
    }

    if(_road_left != -1) {
        auto id_road = all_roads.find(_road_left);
        road_left = id_road->second;
        exist_roads.insert(mapRoad(_road_left, road_left));

        if (road_left->get_start_id() == _id)
            _connected_cross.push_back(road_left->right_cross);
        else {
            if (road_left->is_duplex())
                _connected_cross.push_back(road_left->left_cross);
        }
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

Status Cross::set_route_table(int road_id, pair<int, int> route){

    _route_table[road_id].insert(route);
    return Status::success();
}

Status Cross::delete_route_table(int road_id, pair<int, int> route){

    _route_table[road_id].erase(route.first);
    return Status::success();

}

unordered_map<int, int>* Cross::get_route_table(int road_id){

    return &_route_table[road_id];
}

/*--------------------------------TGarage类方法--------------------------------*/

int TGarage::time_to_go() const{
    return _time_to_go;
}

void TGarage::set_time(int time){
    _time_to_go = time;
}

void TGarage::set_all_car_time(int time){
    for(auto& car:cars)
        car->set_start_time(time);
}

bool TGarage::driveCarInCross(unordered_map<int, Cross*>& all_cross)
{
    static int car_count;
    car_count = 0;
    // 首先将所有此时刻的车辆全部送入 相应出发路口进行等待
    for (auto &car : cars) {

        // 因为所有的车都在车库中不会消失 所以用出发时间判断
        if (car->get_start_time() < global_time)
            continue;

        if(car_count < COE_CARS_GARAGE_NUM)
        {
            int cross_id = car->get_start_id();
            auto cross = all_cross.find(cross_id);    //　从所有的路口中找到需要的路口
            Cross* to_go_cross = cross->second;       //  取出路口
            car->goIntoCross(*to_go_cross);
            // 将此车移除总车库
            all_car_id.erase(car->get_id());
            car_count ++;
        } else{

            car->set_start_time(global_time + 1);
        }
    }

    for (auto &car : cars){
        // 只要有车出发时间比全局时间大 说明已经进行了改变 也就是没有出车库
        if(car->get_start_time() > global_time)
            return false;
    }

    // 如果时刻车库没发完车则返回false
    return true;

}

Status TGarage::pushCar(Car& car)
{
    cars.push_back(&car);
    return Status::success();
}

void DataLoader::init() {
    ifstream in_car, in_road, in_cross, in_answer;
    regex num("-?[0-9]+");
    in_car.open(s_in_car, ios::in);
    string stroneline;

    vector<int>car_cross_input(5, 0);
    vector<int>road_input(7, 0);
    vector<int>answer_input;

    while (getline(in_car, stroneline)) {
        int i = 0;
        for (sregex_iterator it(stroneline.begin(), stroneline.end(), num), end_it; it != end_it; ++it) {
            car_cross_input[i++] = stoi(it->str());
        }
        if (i != 0) {
            Car *new_car = new Car(car_cross_input);
            all_car_id[new_car->get_id()] = new_car;
            all_car_f.push_back(new_car);
        }
    }

    in_road.open(s_in_road, ios::in);
    while (getline(in_road, stroneline)) {
        int i = 0;
        for (sregex_iterator it(stroneline.begin(), stroneline.end(), num), end_it; it != end_it; ++it) {
            road_input[i++] = stoi(it->str());
        }
        if (i != 0) {
            Road *new_road = new Road(road_input);
            all_roads_id[new_road->get_id()] = new_road;
            all_roads_f.push_back(new_road);
        }
    }

    in_cross.open(s_in_cross, ios::in);
    while (getline(in_cross, stroneline)) {
        int i = 0;
        for (sregex_iterator it(stroneline.begin(), stroneline.end(), num), end_it; it != end_it; ++it) {
            car_cross_input[i++] = stoi(it->str());
        }
        if (i != 0) {
            Cross *new_cross = new Cross(car_cross_input);
            all_cross_id[new_cross->get_id()] = new_cross;
            all_cross_f.push_back(new_cross);
        }
    }

    if (s_in_answer != " ")
    {
        //答案第一行不能为文字
        in_answer.open(s_in_answer, ios::in);
        while (getline(in_answer, stroneline)){
            for (sregex_iterator it(stroneline.begin(), stroneline.end(), num), end_it; it != end_it; ++it) {
                answer_input.push_back(stoi(it->str()));
            }
            answer.push_back(answer_input);
            answer_input.clear();
        }
    }
    // 先初始化道路 再初始化路口
    for (auto &road : all_roads_f){
        road->initRoad(all_cross_id);
    }

    for (auto &cross : all_cross_f) {
        cross->initCross(all_roads_id);
    }


}


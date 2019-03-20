//
// Created by prototype on 19-3-16.
//

#include "utils.h"


map<int, Road*> all_roads_id;       // 所有的道路信息　这里保存所有道路的原始内容　其余都是引用或指针
map<int, Cross*> all_cross_id;      // 所有的路口汇总　这里保存路口的原始内容　其余都是引用或指针
map<int, Car*> all_car_id;          // 所有在路上的车辆(不包括在路口中等待的车辆)

vector<Road*> all_roads_f;       // 所有的道路信息　这里保存所有道路的原始内容　其余都是引用或指针
vector<Cross*> all_cross_f;      // 所有的路口汇总　这里保存路口的原始内容　其余都是引用或指针
vector<Car*> all_car_f;          // 所有在路上的车辆(不包括在路口中等待的车辆)


void DataLoader::init() {
    ifstream in_car, in_road, in_cross;
    regex num("-?[0-9]+");
    in_car.open(s_in_car, ios::in);
    string stroneline;

    vector<int>car_cross_input(5, 0);
    vector<int>road_input(7, 0);

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
}

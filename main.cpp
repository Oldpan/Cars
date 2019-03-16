/*
 * 　华为软件精英挑战赛
 * 　主函数部分
 *　 Author：Oldpan
 * 　Updated data：2019-3-16
 * */

#include <iostream>
#include <ctime>
#include <utility>
#include "data_structure.h"

using namespace std;


int main()
{



/*-- test code --*/

    Car* test_car = new Car(10000,18,50,8,3);

    Road* test = new Road(5000,15,6,2,1,2,1);
    test->initRoad();

    pair<int, Car> tt(5, *test_car);

    test->_subroad_1->_lane[3];




    return 0;
}
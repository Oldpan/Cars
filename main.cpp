/*
 * 　华为软件精英挑战赛
 * 　主函数部分
 *　 Author：Oldpan
 * 　Updated data：2019-3-16
 * */

/*--其他方式提速方法:
 *  1,可否充分利用编译时间(60s)--将部分计算部分移到编译部分(模板元编程)  (*)
 *  2,
 * */


#include <iostream>
#include <ctime>
#include <utility>
#include "run.h"

using namespace std;


int main()
{

/*-- test code --*/

//    Car* test_car = new Car(10000,18,50,8,3);
//    Road* test = new Road(5000,15,6,2,1,2,1);
//    test->initRoad();
//
//    pair<int, Car> tt(5, *test_car);
//    cout << tt.second._id << endl;

//    test->_subroad_1->_lane[0].insert(tt);
//
    TestDataInit();
    run();

    return 0;
}
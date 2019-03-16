//
// Created by prototype on 19-3-16.
//

/*
 * 　华为软件精英挑战赛
 * 　模拟运行头文件
 * 　模拟运行代码参考于官方示例：https://bbs.huaweicloud.com/forum/thread-14832-1-1.html
 *　 Author：Oldpan
 * 　Updated data：2019-3-16
 * */

#include "run.h"

using namespace std;

void run()
{
    time_t result = std::time(0);
    cout << std::asctime(std::localtime(&result))
         << result << endl;




}


/*\  　测试使用的地图
 * 　　用于检查　逻辑是否正确
 *    1 ---5000----2
 *    -            -
 *    -            -
 *  5003         5001
 *    -            -
 *    -            -
 *    4 --- 5002---3
 * */


/* 测试数据初始化函数
 *　仅仅用于测试
 * */

Status TestDataInit()
{

    Car* car_1 = new Car(10000,2,4,8,3);
    Car* car_2 = new Car(10001,1,2,6,1);
    Car* car_3 = new Car(10002,4,3,7,2);

    Road* road_1 = new Road(5000,15,6,2,1,2,1);
    Road* road_2 = new Road(5001,12,6,3,2,3,1);
    Road* road_3 = new Road(5002,10,6,2,3,4,1);
    Road* road_4 = new Road(5003,8,6,1,4,1,1);

    Cross* cross_1 = new Cross(1,-1,5000,5003,-1);
    Cross* cross_2 = new Cross(2,-1,-1,5001,5000);
    Cross* cross_3 = new Cross(3,5001,-1,-1,5002);
    Cross* cross_4 = new Cross(4,5003,5002,-1,-1);

    cross_1->road_down = road_4;
    cross_1->road_right = road_1;

    cross_2->road_down = road_2;
    cross_2->road_left = road_1;

    cross_3->road_up = road_2;
    cross_3->road_left = road_3;

    cross_4->road_up = road_4;
    cross_4->road_right = road_3;

    return Status::success();

}

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


// 注意调整current_road 和 current_road_ptr 变动一致
int main(int argc, const char* argv[])
{

//    string car_path = argv[1];
//    string road_path = argv[2];
//    string cross_path = argv[3];
//    string answer_path = argv[4];
//
//
//    auto dataloader = new DataLoader(car_path, road_path,
//                                    cross_path, answer_path);
//    dataloader->init();
//
//    OwnInitData();

    TestDataInit();
    run();

    return 0;
}
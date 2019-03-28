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


// 代码中一定要存在对每个变量值的异常检测
int main(int argc, const char* argv[])
{

    string car_path = argv[1];
    string road_path = argv[2];
    string cross_path = argv[3];
    string answer_path_s;

    if(argv[4] == nullptr)
        answer_path_s = " ";
    else
        answer_path_s = argv[4];


    auto dataloader = new DataLoader(car_path, road_path,
                                    cross_path, answer_path_s);
    dataloader->init();
    answer_path = dataloader->get_answer_path();

    OwnInitData();

    MakeDijkstraGraph(all_cross);

//    for(auto& cross_id : all_cross)
//    {
//        auto cross = cross_id.second;
//        auto roads = cross->exist_roads;
//        cout<<"cross:"<<cross->get_id()<<endl;
//        for(auto& road_and_id : roads){
//            auto road = road_and_id.second;
//            auto road_id = road->get_id();
//            cout<<"road:"<<road_id<<endl;
//            auto route_map = cross->get_route_table(road_id);
//            for (auto& route : *route_map) {
//
//                cout<<route.first<<" "<<route.second<<endl;
//
//            }
//        }
//    }

    run();

    return 0;
}

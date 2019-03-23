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

    string car_path = argv[1];
    string road_path = argv[2];
    string cross_path = argv[3];
    string answer_path;

    if(argv[4] == nullptr)
        answer_path = " ";
    else
        answer_path = argv[4];


    auto dataloader = new DataLoader(car_path, road_path,
                                    cross_path, answer_path);
    dataloader->init();

    OwnInitData();

    MakeDijkstraGraph(all_cross);

    for(auto& cross_id : all_cross)
    {
        auto cross = cross_id.second;
        auto roads = cross->exist_roads;
        cout<<"cross:"<<cross->get_id()<<endl;
        for(auto& road_and_id : roads){
            auto road = road_and_id.second;
            auto road_id = road->get_id();
            cout<<"road:"<<road_id<<endl;
            auto route_map = cross->get_route_table(road_id);
            for (auto& route : *route_map) {

                cout<<route.first<<" "<<route.second<<endl;

            }
        }
    }



//    TestDataInit();
//    run();

    return 0;
}


//oid Data_Loader::form_route_table(Cross* cur_cross) {
//    for (int dir = 0; dir < 4; dir++) {
//        int id_of_next_cross = cross_id_to_crosss_id(cur_cross->get_id(), dir);
//        if (id_of_next_cross == -1)
//            continue;
//        int self_length = get_min_length(cross_id_to_road(cur_cross->get_id(), dir));
//        Route_table* route_table = new Route_table(dir, self_length);
//        dijkstra(id_of_next_cross, cur_cross->get_id(), route_table->min_dis_table_by_length);
//        bool have_old = false;
//        for (auto old_route_table : cur_cross->m_route_table) {
//            if (old_route_table->m_dir == dir) {
//                have_old = true;
//                delete(old_route_table);
//                old_route_table = route_table;
//            }
//        }
//        if(!have_old)
//            cur_cross->m_route_table.push_back(route_table);
//    }
//}
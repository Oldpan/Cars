/*
 * 　华为软件精英挑战赛
 * 　数据结构yuanwenjian
 *　 Author：Oldpan
 * 　Updated data：2019-3-16
 * */

#include "data_structure.h"


Status Road::initRoad()
{
    if(_is_duplex){   // 如果是双行道路
        _subroad_1 = new SubRoad(_lane_num, _start_id, _end_id);
        _subroad_2 = new SubRoad(_lane_num, _end_id, _start_id);
    } else
        _subroad_1 = new SubRoad(_lane_num, _start_id, _end_id);

    cerr<<"Message"<<endl;

    return Status::success();
}






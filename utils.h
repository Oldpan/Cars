#include <utility>

/*
 * 　华为软件精英挑战赛
 * 　辅助类 负责记录，输入输出需要的信息
 *　 Author：Oldpan
 * 　Updated data：2019-3-16
 * */

#ifndef CARS_UTILS_H
#define CARS_UTILS_H


#include <fstream>
#include <string>
#include <vector>
#include <regex>


using namespace std;


/*
 * 记录信息类
 * 必须被继承
 * 只是接口
 * */

enum class ErrorCode : int
{
    kSUCCESS = 0,
    kFAIL_CONDITION = 1,
    kINTERNAL_ERROR = 2,
    kINVALID_VALUE = 3,
};


class Status{
    ErrorCode _code;
    std::string _desc;

public:
    static Status success() { return Status(ErrorCode::kSUCCESS); }
    Status() {}

    explicit Status(ErrorCode code, std::string desc="")
            : _code(code), _desc(std::move(desc)){}
    const char* desc() const  { return _desc.c_str(); }
    ErrorCode  code() const  { return _code; }
    bool is_error()   const { return _code != ErrorCode::kSUCCESS; }
    bool is_success() const { return _code == ErrorCode::kSUCCESS; }
};


#define MAKE_ERROR(desc, code) \
  Status((code), (desc))

// 判断 condition 是否正确 不正确则生成错误代码 error_code 的Status 直接返回错误代码
#define ASSERT(condition, error_code) do { \
    if( !(condition) ) { \
      return MAKE_ERROR("Assertion failed: " #condition, (error_code)); \
    } \
  } while(0)












#endif

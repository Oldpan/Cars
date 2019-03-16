/*
 * 　华为软件精英挑战赛
 * 　辅助类 负责记录，输入输出需要的信息
 *　 Author：Oldpan
 * 　Updated data：2019-3-16
 * */

#ifndef CARS_UTILS_H
#define CARS_UTILS_H


#include <fstream>
#include <map>


/*
 * 记录信息类
 * 必须被继承
 * 只是接口
 * */


class Logger
{
public:

    enum class Severity
    {
        kINTERNAL_ERROR = 0, //  输出　程序代码错误　
        kERROR = 1,          //  输出　车辆行驶错误　这种错误不能出现
        kWARNING = 2,        //  输出　有可能会影响车辆正常行驶的消息　
        kINFO = 3            //  输出　一些基本的信息
    };

    virtual void log(Severity severity, const char* msg) = 0;

    virtual ~Logger() {}
};


enum class ErrorCode : int
{
    kSUCCESS = 0,
    kINTERNAL_ERROR = 1,
    kMEM_ALLOC_FAILED = 2,
    kINVALID_VALUE = 3,
    kINVALID_GRAPH = 4,
};


class Status{
    ErrorCode _code;

public:
    static Status success() { return Status(ErrorCode::kSUCCESS); }
    Status() {}

    explicit Status(ErrorCode code)
            : _code(code) {}
    ErrorCode  code() const  { return _code; }
    bool is_error()   const { return _code != ErrorCode::kSUCCESS; }
    bool is_success() const { return _code == ErrorCode::kSUCCESS; }
};


#define MAKE_ERROR(desc, code) \
  Status((code), (desc), __FILE__, __LINE__, __func__)

// 判断 condition 是否正确 不正确则生成错误代码 error_code 的Status 直接返回错误代码
#define ASSERT(condition, error_code) do { \
    if( !(condition) ) { \
      return MAKE_ERROR("Assertion failed: " #condition, (error_code)); \
    } \
  } while(0)


//
//class Garage{
//public:
//
//
//private:
//
//};


#endif

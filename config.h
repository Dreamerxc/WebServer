//
// Created by Dreamer.c on 2021/7/4.
//

#ifndef WEBSERVER_CONFIG_H
#define WEBSERVER_CONFIG_H
#include "webserver.h"

using namespace std;

class Config
{
public:
    Config();
    ~Config(){};

    void parse_arg(int argc, char* argv[]);

    int PORT;        // 端口号
    int TRIGMode;     // 触发组合模式
    int LISTENTrigmode;    // listenfd触发模式
    int CONNTrigmode;      // connfd触发模式
    int OPT_LINGER;        // 优雅关闭连接
    int sql_num;           // 数据库连接池数量
    int thread_num;        // 线程池数量
    int actor_model;       // 并发模型选择
};

#endif //WEBSERVER_CONFIG_H

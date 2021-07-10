#include "config.h"
#include <string>
using namespace std;
int main(int argc, char *argv[]) {
    //需要修改的数据库信息,登录名,密码,库名
    string user = "debian-sys-maint";
    string passwd = "h6jjML1x0ANZkKVo";
    string databasename = "db";

    //命令行解析
    Config config;
    config.parse_arg(argc, argv);

    WebServer server;

    //初始化
    server.init(config.PORT, user, passwd, databasename, config.OPT_LINGER,
                config.TRIGMode, config.sql_num, config.thread_num, config.actor_model);


    //数据库
    server.sql_pool();

    //线程池
    server.thread_pool();

    //触发模式
    server.trig_mode();

    //监听
    server.eventListen();

    //运行
    server.eventLoop();

    return 0;
}

#include "config.h"

Config::Config()
{
    PORT = 9006;   // 默认端口号

    TRIGMode = 0;  // 触发组合模式，默认listenfd LT + connfd LT

    LISTENTrigmode = 0; // listenfd触发方式，默认LT

    CONNTrigmode = 0;   // connfd触发方式，默认LT

    OPT_LINGER = 0;    // 默认不使用优雅关闭连接

    sql_num = 8;       // 数据库连接池默认8

    thread_num = 8;    // 数据库连接池默认8

    actor_model = 0;   //  并发模型，默认为proactor
}

void Config::parse_arg(int argc, char **argv) {
    int opt;
    const char *str = "p:m:o:s:t:a:";
    while ((opt = getopt(argc, argv, str)) != -1)
    {
        switch (opt)
        {
            case 'p':
            {
                PORT = atoi(optarg);
                break;
            }
            case 'm':
            {
                TRIGMode = atoi(optarg);
                break;
            }
            case 'o':
            {
                OPT_LINGER = atoi(optarg);
                break;
            }
            case 's':
            {
                sql_num = atoi(optarg);
                break;
            }
            case 't':
            {
                thread_num = atoi(optarg);
                break;
            }
            case 'a':
            {
                actor_model = atoi(optarg);
                break;
            }
            default:
                break;
        }
    }
}

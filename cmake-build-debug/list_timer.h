//
// Created by dreamer on 2020/12/6.
//

#ifndef WEBSERVER_LIST_TIMER_H
#define WEBSERVER_LIST_TIMER_H
#include<unistd.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/epoll.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<assert.h>
#include<sys/stat.h>
#include<cstring>
#include<pthread.h>
#include<cstdio>
#include<stdlib.h>
#include<sys/mman.h>
#include<stdarg.h>
#include<errno.h>
#include<sys/wait.h>
#include<sys/uio.h>
#include<time.h>

class util_timer;

struct client_data{
    sockaddr_in address;
    int sockfd;
    util_timer *timer;
};

class util_timer{
public:
    util_timer():prev(NULL),next(NULL){};

public:
    time_t expires;
    void (* cb_func)(client_data*);
    client_data *user_data;
    util_timer *prev;
    util_timer *next;
};

class sort_timer_list{
public:
    sort_timer_list();
    ~sort_timer_list();

    void add_timer(util_timer *timer);
    void adjust_timer(util_timer *timer);
    void del_timer(util_timer *timer);
    void tick();

private:
    void add_timer(util_timer *timer,util_timer *list_head);
     util_timer *head;
     util_timer *tail;
};

class utils{
public:
    utils(){};
    ~utils(){};

    void init(int timeslot);
    int setnonblocking(int fd);
    void addfd(int epollfd,int fd,bool one_shot,int mode);
    static void sig_handler(int sig);
    void addsig(int sig,void(handler)(int),bool restart = true);
    void timer_handler();
    void show_error(int connfd,const char *info);

public:
    static int *u_pipefd;
    sort_timer_list m_timer_list;
    static int u_epollfd;
    int m_timeslot;
};

void cb_func(client_data *user_data);
#endif //WEBSERVER_LIST_TIMER_H

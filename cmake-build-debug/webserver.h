//
// Created by dreamer on 2020/12/8.
//
#ifndef WEBSERVER_WEBSERVER_H
#define WEBSERVER_WEBSERVER_H

#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<unistd.h>
#include<errno.h>
#include<fcntl.h>
#include<stdlib.h>
#include<cassert>
#include<sys/epoll.h>

#include "list_timer.h"
#include"threadpool.h"
#include"http_conn.h"

const int MAX_FD = 65536;  // the size of socket fd
const int MAX_EVENT_NUMBER = 10000;
const int TIMESLOT = 5;

class Webserver{
public:
    Webserver();
    ~Webserver();

    void init(int port,string user,string password,string databasename,
              int log_write,int opt_linger,int trigmode,int sql_num,
              int thread_num,int close_log,int actor_model);

    void thread_pool();
    void sql_pool();
    void log_write();
    void trig_mode();
    void eventListen();
    void eventLoop();
    void timer(int connfd,struct sockaddr_in client_address);
    void adjust_timer(util_timer *timer);
    void deal_timer(util_timer *timer,int sockfd);
    bool dealclientdata();
    bool dealwithsignal(bool& timerout,bool& stop_server);
    void dealwithread(int sockfd);
    void dealwithwrite(int sockfd);

public:
    int m_port;
    char *m_root;
    int m_log_write;
    int m_close_log;
    int m_actormodel;

    int m_pipefd[2];
    int m_epollfd;
    http_conn *users;

    // sql ,,,
    connection_pool *m_connpool;
    string m_user;
    string m_password;
    string m_databasename;
    int m_sql_num;

    threadpool<http_conn>* m_pool;
    int m_thread_num;


    epoll_event events[MAX_EVENT_NUMBER];

    int m_listenfd;
    int m_linger;
    int m_mode;
    int m_listenmode;
    int m_conntmode;

    client_data *users_timer;
    utils util;
};

#endif //WEBSERVER_WEBSERVER_H

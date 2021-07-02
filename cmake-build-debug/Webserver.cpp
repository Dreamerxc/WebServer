//
// Created by dreamer on 2020/12/8.
//

#include "webserver.h"

Webserver::Webserver() {
    users = new http_conn[MAX_FD];

    char server_path[200];
    getcwd(server_path,200);
    char root[6] = "/root";
    m_root = (char *)malloc(strlen(server_path)+strlen(root)+1);
    strcpy(m_root,server_path);
    strcat(m_root,root);

    users_timer = new client_data[MAX_FD];
}

Webserver::~Webserver() {
    close(m_epollfd);
    close(m_listenfd);
    close(m_pipefd[1]);
    close(m_pipefd[0]);
    delete[] users;
    delete[] users_timer;
    delete m_pool;
}

void Webserver::init(int port, int user, int password, int databasename, int log_write, int opt_linger, int trigmode,
                     int sql_num, int thread_num, int close_log, int actor_model) {
    m_port = port;
    m_user = user;
    m_password = password;
    m_databasename = databasename;
    m_sql_num = sql_num;
    m_thread_num = thread_num;
    m_log_write = log_write;
    m_linger = opt_linger;
    m_mode = trigmode;
    m_close_log = close_log;
    m_actormodel = actor_model;
}

void Webserver::trig_mode() {
    if(0==m_mode){
        m_listenmode = 0;
        m_conntmode = 0;
    }
    else if(1==m_mode){
        m_listenmode = 1;
        m_conntmode = 0;
    }
    else if(2==m_mode){
        m_listenmode = 1;
        m_conntmode = 0;
    }
    else if(3==m_mode){
        m_listenmode = 1;
        m_conntmode = 1;
    }
}

void Webserver::log_write() {

}

void Webserver::sql_pool() {

}

void Webserver::thread_pool() {
    m_pool = new threadpool<http_conn>(m_actormodel,m_connpool,m_thread_num);
}

void Webserver::eventListen() {
    m_listenfd = socket(PF_INET,SOCK_STREAM,0);
    assert(m_listenfd>=0);

    if(0==m_linger){
        struct linger tmp = {0,1};
        setsockopt(m_listenfd,SOL_SOCKET,SO_LINGER,&tmp,sizeof(tmp));
    }
    else if(1==m_linger){
        struct linger tmp = {1,1};
        setsockopt(m_listenfd,SOL_SOCKET,SO_LINGER,&tmp,sizeof(tmp));
    }

    int ret = 0;
    struct sockaddr_in address;
    bzero(&address,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(m_port);

    int flag = 1;
    setsockopt(m_listenfd,SOL_SOCKET,SO_REUSEADDR,&flag,sizeof(flag));
    ret = bind(m_listenfd,(struct sockaddr*)&address,sizeof(address));
    aasert(ret >= 0);
    ret = listen(m_listenfd,5);
    assert(ret >= 0);
    util.init(TIMESLOT);

    epoll_event events[MAX_EVENT_NUMBER];
    m_epollfd = epoll_create(5);
    assert(m_epollfd!=-1);

    util.addfd(m_epollfd,m_listenfd,false,m_listenmode);
    http_conn::m_epolled = m_epollfd;

    ret = socketpair(PF_UNIX,SOCK_STREAM,0,m_pipefd);
    assert(ret!=-1);
    util.setnonblocking(m_pipefd[1]);
    util.addfd(m_epollfd,m_pipefd[0],false,0);

    util.addsig(SIGPIPE,SIG_IGN);
    util.addsig(SIGALRM,util.sig_handler,false);
    util.addsig(SIGTERM,util.sig_handler,false);

    alarm(TIMESLOT);

    utils::u_pipefd = m_pipefd;
    utils::u_epollfd = m_epollfd;
}

void Webserver::timer(int connfd,struct sockaddr_in client_address){
    users[connfd].init(connfd,client_address,m_root,m_conntmode,m_close_log,
                       m_user,m_password,m_databasename);
    users_timer[connfd].address = client_address;
    users_timer[connfd].sockfd = connfd;
    util_timer *timer = new util_timer;
    timer->user_data = &users_timer[connfd];
    timer->cb_func = cb_func;
    time_t cur = time(NULL);
    timer->expires = cur + 3*TIMESLOT;
    users_timer[connfd].timer = timer;
    util.m_timer_list.add_timer(timer);
}

void Webserver::adjust_timer(util_timer *timer){
    time_t cur = time(NULL);
    timer->expires = cur + 3*TIMESLOT;
    util.m_timer_list.adjust_timer(timer);
}

void Webserver::deal_timer(util_timer *timer, int sockfd) {
    timer->cb_func(&users_timer[sockfd]);
    if(timer){
        util.m_timer_list.del_timer(timer);
    }
}

bool Webserver::dealclientdata() {
    struct sockaddr_in client_address;
    socklen_t client_addrlength = sizeof(client_address);
    if(m_listenmode == 0){
        int connfd = accept(m_listenfd,(struct sockaddr*)&client_address,&client_addrlength);
        if(connfd<0){
            return false;
        }
        if(http_conn::m_user_count >= MAX_FD){
            util.show_error(connfd,"Internet server busy");
            return false;
        }
        timer(connfd,client_address);
    }
    else{
        while(1){
            int connfd = accept(m_listenfd,(struct sockaddr*)&client_address,&client_addrlength);
            if(connfd<0){
                break;
            }
            if(http_conn::m_user_count >= MAX_FD){
                util.show_error(connfd,"Internet server busy");
                break;
            }
            timer(connfd,client_address);
        }
        return false;
    }
    return true;
}

bool Webserver::dealwithsignal(bool &timerout, bool &stop_server) {
    int ret = 0;
    int sig;
    char signals[1024];
    ret = recv(m_pipefd[0],signals,sizeof(signals),0);
    if(ret == -1){
        return false;
    }
    else if(ret == 0){
        return false;
    }
    else{
        for(int i = 0;i<ret;i++){
            switch(signals[i]){
                case SIGALRM:{
                    timeout = true;
                    break;
                }
                case SIGTERM:{
                    stop_server = true;
                    break;
                }
            }
        }
    }
    return true;
}

void Webserver::dealwithread(int sockfd) {
    util_timer *timer = users_timer[sockfd].timer;

    if(1 == m_actormodel){
        if(timer){
            adjust_timer(timer);
        }
        m_pool->append(users+scokfd,0);

        while(true){
            if(1 == users[sockfd].improv){
                if(1==users[sockfd].timer_flag){
                    deal_timer(timer,sockfd);
                    users[sockfd].timer_flag = 0;
                }
                users[sockfd].improv = 0;
                break;
            }
        }
    }

    else{
        if(users[sockfd].read_once()){
            m_pool->append(users + sockfd);
            if(timer){
                adjust_timer(timer);
            }
        }
        else{
            deal_timer(timer,sockfd);
        }
    }
}

void Webserver::dealwithwrite(int sockfd) {
    util_timer *timer = users_timer[sockfd].timer;

    if(1==m_actormodel){
        if(timer){
            adjust_timer(timer);
        }
        m_pool->append(users+sockfd,1);

        while(true){
            if(1==users[sockfd].improv){
                if(1==users[sockfd].timer_flag){
                    deal_timer(timer,sockfd);
                    users[sockfd].timer_flag = 0;
                }
                users[sockfd].improv = 0;
                break;
            }
        }
    }
    else{
        if(users[sockfd].write()){
            if(timer){
                adjust_timer(timer);
            }
        }
        else{
            deal_timer(timer,sockfd);
        }
    }
}

void Webserver::eventLoop(){
    bool timeout = false;
    bool stop_server = false;

    while(!stop_server){
        int number = epoll_wait(m_epollfd,events,MAX_EVENT_NUMBER,-1);
        if(number<0&&errno!=EINTR){
            break;
        }

        for(int i = 0;i<number;i++){
            int sockfd = events[i].data.fd;

            if(sockfd == m_listenfd){
                bool flag = dealclientdata();
                if(flag == false) continue;
            }

            else if(events[i].events&(EPOLLRDHUP|EPOLLHUP|EPOLLERR)){
                util_timer *timer = users_timer[sockfd].timer;
                deal_timer(timer,sockfd);
            }

            else if((sockfd==m_pipefd[0])&&(events[i].events&EPOLLIN)){
                bool flag = dealwithsignal(timeout,stop_server);
                if(false == flag){

                }
            }

            else if(events[i].events&EPOLLIN){
                dealwithread(sockfd);
            }
            else if(events[i].events&EPOLLOUT){
                dealwithwrite(sockfd);
            }
        }
        if(timeout){
            util.timer_handler();

            timeout = false;
        }
    }
}





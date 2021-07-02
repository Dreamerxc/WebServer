//
// Created by dreamer on 2020/12/6.
//
#include"list_timer.h"
sort_timer_list::sort_timer_list(){
    head = NULL;
    tail = NULL;
}

sort_timer_list::~sort_timer_list() {
    util_timer *tmp = head;
    while(tmp){
        head = tmp->next;
        delete tmp;
        tmp = head;
    }
}

void sort_timer_list::add_timer(util_timer *timer,util_timer *list_head) {
    util_timer *prev = list_head;
    util_timer *tmp = prev->next;
    while(tmp){
        if(timer->expires<tmp->expires){
           prev->next = timer;
           timer->prev = prev;
           timer->next = tmp;
           tmp->prev = timer;
           break;
        }
        prev = tmp;
        tmp = tmp->next;
        if(!tmp){
            prev->next = timer;
            timer->prev = prev;
            timer->next = NULL;
            tail = timer;
        }
    }
}

void sort_timer_list::add_timer(util_timer *timer) {
    if(!timer){
        return;
    }
    if(!head){
        head = timer;
        tail = timer;
        return;
    }
    if(timer->expires < head->expires){
        timer->next = head;
        head->prev = timer;
        head = timer;
        return;
    }
    add_timer(timer,head);
}

void sort_timer_list::adjust_timer(util_timer *timer){
    if(!timer){
        return;
    }
    util_timer *tmp = timer->next;
    if(!tmp||(timer->expires<tmp->expires)){
        return;
    }
    if(timer==head){
        head = head->next;
        head->prev = NULL;
        timer->next = NULL;
        add_timer(timer,head);
    }
    else{
        timer->prev->next = timer->next;
        timer->next->prev = timer->prev;
        add_timer(timer,timer->next);
    }
}

void sort_timer_list::del_timer(util_timer *timer) {
    if (!timer) {
        return;
    }
    if(timer==head&&timer==tail){
        delete timer;
        head = NULL;
        tail = NULL;
        return;
    }
    if(timer==head){
        head = head->next;
        head->prev = NULL;
        delete timer;
        return;
    }
    if(timer==tail){
        tail = tail->prev;
        tail->next = NULL;
        delete timer;
        return;
    }
    timer->prev->next = timer->next;
    timer->next->prev = timer->prev;
    delete timer;
}

void sort_timer_list::tick() {
    if(!head){
        return;
    }
    time_t cur = time(NULL);
    util_timer *tmp = head;
    while(tmp){
        if(cur<tmp->expires){
            break;
        }
        tmp->cb_func(tmp->user_data);
        head = head->next;
        if(head){
            head->prev = NULL;
        }
        delete tmp;
        tmp = head;
    }
}

void utils::init(int timeslot) {
    m_timeslot = timeslot;
}

int utils::setnonblocking(int fd) {
    int old_option = fcntl(fd,F_GETFL);
    int new_option = old_option|O_NONBLOCK;
    fcntl(fd,F_SETFL,new_option);
    return old_option;
}

void utils::addfd(int epollfd,int fd,bool one_shot,int mode){
    epoll_event event;
    event.data.fd = fd;
    if(mode == 1){
        event.events = EPOLLIN|EPOLLET|EPOLLRDHUP;
    }
    else{
        event.events = EPOLLIN|EPOLLRDHUP;
    }
    if(one_shot){
        event.events |= EPOLLONESHOT;
    }
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);
    setnonblocking(fd);
}

void utils::sig_handler(int sig) {
    int save_errno = errno;
    int msg = sig;
    send(u_pipefd[1],(char*)&msg,1,0);
    errno = save_errno;
}

void utils::addsig(int sig, void (handler)(int), bool restart) {
    struct sigaction sa;
    memset(&sa,'\0',sizeof(sa));
    sa.sa_handler = handler;
    if(restart){
        sa.sa_flags |= SA_RESTART;
    }
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig,&sa,NULL)!=-1);
}

void utils::timer_handler() {
    m_timer_list.tick();
    alarm(m_timeslot);
}

void utils::show_error(int connfd, const char *info) {
    send(connfd,info,strlen(info),0);
    close(connfd);
}

int *utils::u_pipefd = 0;
int utils::u_epollfd = 0;

void cb_func(client_data *user_data){
    epoll_ctl(utils::u_epollfd,EPOLL_CTL_DEL,user_data->sockfd,0);
    assert(user_data);
    close(user_data->sockfd);
}
















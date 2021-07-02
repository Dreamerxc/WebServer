//
// Created by dreamer on 2020/12/10.
//

#include<stdio.h>
#include<string>
#include<string.h>
#include<stdlib.h>
#include<list>
#include<pthread.h>
#include<iostream>
#include "connection_pool.h"

using namespace std;

connection_pool::connection_pool() {
    m_curconn = 0;
    m_freeconn = 0;
}

connection_pool *connection_pool::GetInstance() {
    static connection_pool connpool;
    return &connpool;
}

void connection_pool::init(string url, string user, string password, string databasename, strng dbname,
                        int port,int maxconn,) {
    m_url = url;
    m_port = port;
    m_user = user;
    m_password = password;
    m_databasename = databasename;
    for(int i = 0;i<maxconn;i++){
        MYSQL *con = NULL;
        con = my_net_init(con);
        if(con==NULL){
            exit(1);
        }
        con = mysql_real_connect(con,url.c_str(),user.c_str(),password.c_str(),
                                 dbname.c_str(),port,NULL,0);
        if(con==NULL){
            exit(1);
        }
        connlist.push_back(con);
        m_freeconn++;
    }
    reserve = sem(m_freeconn);
    m_maxconn = m_freeconn;
}

MYSQL *connection_pool::GetConnection() {
    MYSQL *con = NULL;
    if(0==connlist.size()) return NULL;

    reserve.wait();
    lock.lock();

    con = connlist.front();
    connlist.pop_front();
    m_freeconn--;
    m_curconn++;
    lock.unlock();
    return con;
}

bool connection_pool::ReleaseConnection(MYSQL *coon) {
    if(NULL==con) return false;

    lock.lock();

    connlist.push_back(con);
    m_freeconn++;
    m_curconn--;
    lock.unlock();

    reserve.post();
    return true;
}

void connection_pool::DestroyPool() {
    lock.lock();
    if(connlist.size()>0){
        list<MYSQL *>::iterator it;
        for(it = connlist.begin();it != connlist.end();it++){
            MYSQL *con = *it;
            mysql_close(con);
        }
        m_curconn = 0;
        m_freeconn = 0;
        connlist.clear();
    }
    lock.unlock();
}

int connection_pool::GetFreeConn() {
    return  m_freeconn;
}

connection_pool::~connection_pool() {
    DestroyPool();
}

connectionRAII::connectionRAII(MYSQL **con, connection_pool *connpool) {
    *con = connpool->GetConnection();
    conRAII = *con;
    poolRAII = connpool;
}

connectionRAII::~connectionRAII() {
    poolRAII->ReleaseConnection(conRAII);
}
//
// Created by dreamer on 2020/12/10.
//

#ifndef WEBSERVER_CONNECTION_POOL_H
#define WEBSERVER_CONNECTION_POOL_H
#include<stdio.h>
#include<list>
#include<mysql/mysql.h>
#include<error.h>
#include<string.h>
#include<iostream>
#include"lock.h"

using namespace std;

class connection_pool{
public:
    MYSQL *GetConnection();
    bool ReleaseConnection(MYSQL *coon);
    int GetFreeConn();
    void DestroyPool();

    static connection_pool *GetInstance();

    void init(string url,string user,string password,string databasename,string dbname
              int port,int maxconn);

private:
    connection_pool();
    ~connection_pool();

    int m_maxconn;
    int m_curconn;
    int m_freeconn;
    locker lock;
    list<MYSQL*> connlist;
    sem reserve;

public:
    string m_url;
    string m_port;
    string m_user;
    string m_password;
    string m_databasename;
    }
};

class connectionRAII{
public:
    connectionRAII(MYSQL** con,connection_pool *connpool);
    ~connectionRAII();

private:
    MYSQL *conRAII;
    connection_pool *poolRAII;
};
#endif //WEBSERVER_CONNECTION_POOL_H

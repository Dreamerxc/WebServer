//
// Created by dreamer on 2020/12/6.
//

#ifndef WEBSERVER_BLOCK_QUEUE_H
#define WEBSERVER_BLOCK_QUEUE_H
#include <iostream>
#include<stdlib.h>
#include<pthread.h>
#include"lock.h"
using namespace std;

template<class T>
class block_queue{
public:
    block_queue(int max_size==1000){
        if(max_size<0){
            exit(-1);
        }
        m_max_size = max_size;
        m_array = new T[m_max_size];
        m_front = -1;
        m_end = -1;
        m_size = 0;
    }

    ~block_queue(){
        m_mutex.lock();
        if(m_array!=nullptr)
            delete []m_array;
        m_mutex.unlock();
    }
    void clear(){
        m_mutex.lock();
        m_size = 0;
        m_front = -1;
        m_end = -1;
        m_mutex.unlock();
    }
    bool full(){
        m_mutex.lock();
        if(m_size>=m_max_size){
            m_mutex.unlock();
            return true;
        }
        m_mutex.unlock();
        return false;
    }
    bool empty(){
        m_mutex.lock();
        if(m_size==0){
            m_mutex.unlock();
            return true;
        }
        m_mutex.unlock();
        return false;
    }
    bool front(T& value){
        m_mutex.lock();
        if(m_size==0){
            m_mutex.unlock();
            return false;
        }
        value = m_array[m_front];
        m_mutex.unlock();
        return true;
    }
    bool back(T& value){
        m_mutex.lock();
        if(m_size==0){
            m_mutex.unlock();
            return false;
        }
        value = m_array[m_end];
        m_mutex.unlock();
        return true;
    }
    int size(){
        int temp = 0;
        m_mutex.lock();
        temp = m_size;
        m_mutex.unlock();
        return temp;
    }
    int max_size(){
        int temp = 0;
        m_mutex.lock();
        temp = m_max_size;
        m_mutex.unlock();
        return temp;
    }
    bool push(const T& item){
        m_mutex.lock();
        if(m_size>=m_max_size){
            m_cond.broadcast();
            m_mutex.unlock();
            return false;
        }
        m_end = (m_end+1)%m_max_size;
        m_array[m_end] = item;
        m_size++;
        m_cond.broadcast();
        m_mutex.unlock();
        return true;
    }
    bool pop(T& item){
        m_mutex.lock();
        while(m_size<=0) {
            if (!m_cond.wait(m_mutex.get())) {   // !!!!
                m_mutex.unlock();
                return false;
            }
        }
        m_front = (m_front + 1)%m_max_size;
        item = m_array[m_front];
        m_size--;
        m_mutex.unlock();
        return true;
    }
    bool pop(T& item,int ms_timeout){
        struct timespec t = {0,0};
        struct timeval now = {0,0};
        gettimeofday(&now,null);
        m_mutex.lock();
        if(m_size()<=0){
            t.tv_sec = now.tv_sec + ms_timeout/1000;
            t.tv_nsec = (ms_timeout % 1000)*1000;
            if(!m_cond.timewait(m_mutex.get(),t)){
                m_mutex.unlock();
                return false;
            }
        }
        if(m_size()<=0) {
            m_mutex.unlock();
            return false;
        }
        m_front = (front+1)%m_max_size;
        item = m_array[m_front];
        m_mutex.unlock();
        return true;
    }

private:
    locker m_mutex;
    cond m_cond;
    T* m_array;
    int m_size;
    int m_max_size;
    int m_front;
    int m_end;
};
#endif //WEBSERVER_BLOCK_QUEUE_H

#ifndef FIX_CLIENT_H_
#define FIX_CLIENT_H_
#ifdef __linux__
#include <sys/epoll.h>
#endif // __linux__
#include <pthread.h>
/* required for socket */
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <spdlog/spdlog.h>
#include "fix/fix_data_type.h"
#include "data_store.h"

#define MAX_EPOLL_EVENTS 64

class FixClient {
public:
    int Init(DataStore* data_store, OnRcvMsg recv_msg, char* host, short& port, bool cpu_affinity_enable, int cpu_on);
    void SendMsg(char* data, short len, int seq_num, char* msg_type);
    void RecvMsg();
private:
    char server_ip_addr_[16];
    short server_port_;
    OnRcvMsg on_rcv_msg_;
    short sock_fd_;
    string user_name_;
    string password_;
    bool is_conn_ = false;
    pthread_t recv_thread_;
    int epoll_fd_;
    bool is_ok_ = true;
    bool is_win_ = true;
    DataStore* data_store_;
    #if __linux__
    struct epoll_event event_, events_[MAX_EPOLL_EVENTS];
    #endif
};

#endif // FIX_CLIENT_H_
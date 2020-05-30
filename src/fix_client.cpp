#include "../include/fix_client.h"
#include "../include/utils.h"
int FixClient::Init(DataStore* data_store, OnRcvMsg recv_msg, char *host, short &port, bool cpu_affinity_enable, int cpu_on)
{
    on_rcv_msg_ = recv_msg;
    data_store_ = data_store;
    memset(server_ip_addr_, 0, sizeof(server_ip_addr_));
    memcpy(server_ip_addr_, host, strlen(host));
    server_port_ = port;
    if ((sock_fd_ = socket(AF_INET, SOCK_STREAM, 0)) == SOCK_ERR)
    {
        spdlog::info("## error in socket, errno = {}", errno);
        return -1;
    }
    spdlog::info("Socket fd = {}", sock_fd_);
#if __linux__
    is_win_ = false;
    epoll_fd_ = epoll_create(1);
    if (epoll_fd_ == -1)
    {
        spdlog::info("Failed to create epoll file descriptor");
        is_ok_ = false;
    }

    if (is_ok_)
    {
        event_.events = EPOLLIN;
        event_.data.fd = sock_fd_;

        if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, sock_fd_, &event_))
        {
            spdlog::info("Failed to add file descriptor to epoll");
            close(epoll_fd_);
            is_ok_ = false;
        }
    }

#endif // __linux__

    spdlog::info("Host = {}, port = {}", server_ip_addr_, server_port_);
    struct sockaddr_in addr = {};
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(server_ip_addr_);
    addr.sin_port = htons(server_port_);
    int conn_fd = connect(sock_fd_, (const sockaddr *)&addr, sizeof(addr));
    if (conn_fd == 0)
    {
        spdlog::info("Connect is OK!");
    }
    else
    {
        spdlog::info("Connection is Failed! reason is: {}, errno = {}", strerror(errno), errno);
        return -1;
    }
    //}
    is_conn_ = true;
    int flags = fcntl(sock_fd_, F_GETFL, 0);
    fcntl(sock_fd_, F_SETFL, flags | O_NONBLOCK);
    std::thread message_callback_thread(&FixClient::RecvMsg, this);
    if (cpu_affinity_enable)
    {
        Utils::cpuAffinity(message_callback_thread.native_handle(), cpu_on);
    }
    message_callback_thread.detach();
    spdlog::info("Neon Messsage Client[cpu_affinity_enable={}, cpu_on={}]", cpu_affinity_enable, cpu_on);
    return sock_fd_;
}

void FixClient::SendMsg(char *data, short len, int seq_num, char* msg_type)
{
    if (!is_conn_)
    {
        spdlog::info("The Neon connection is broken!");
        return;
    }
    else
    {
        spdlog::info("Send Message to Neon FIX...");
        data_store_->StoreOutgoingMsg(data, &len, &seq_num, msg_type);
    }
    write(sock_fd_, data, len);
}

void FixClient::RecvMsg()
{
    char recv_buf[8192]; // 8K
    ssize_t recv_len;
#if __linux__
    int event_count, i;
#endif // __linux__
    // size_t bytes_read;
    while (true)
    {
        try
        {
#if __linux__
            event_count = epoll_wait(epoll_fd_, events_, MAX_EPOLL_EVENTS, 30000);
            // LOG(INFO) << "epoll_fd = " << epoll_fd_ << ", ready events = " << event_count;
            for (i = 0; i < event_count; i++)
            {
                // LOG(INFO) << "Reading file descriptor  -- " << events_[i].data.fd;
                recv_len = recv(events_[i].data.fd, recv_buf, sizeof(recv_buf), MSG_DONTWAIT);
                if (0 > recv_len)
                {
                    spdlog::info("[Neon FixClient] Shutdown the connection[host={}, port= {}]", server_ip_addr_, server_port_);
                    //LOG(ERROR) << "[Neon FixClient error]: RecvFromCSP failed, reason: " << strerror(errno) << ", errno is:" << errno;
                    break;
                }
                else if (0 == recv_len)
                {
                    spdlog::info("[Neon FixClient warning]: received message size is 0，the connection is broken, sock_fd:{}", sock_fd_);
                    break;
                }
                else
                {
                    on_rcv_msg_(recv_buf, recv_len);
                }
            }
#else
            fd_set read_flags;
            FD_ZERO(&read_flags);
            FD_SET(sock_fd_, &read_flags);

            if (select(sock_fd_ + 1, &read_flags, (fd_set *)0, (fd_set *)0, NULL) <= 0)
                continue;

            if (!FD_ISSET(sock_fd_, &read_flags))
                continue;

            recv_len = recv(sock_fd_, recv_buf, sizeof(recv_buf), MSG_DONTWAIT);
            if (0 > recv_len)
            {
                spdlog::info("[Neon FixClient] Shutdown the connection[host={}, port= {}]", server_ip_addr_, server_port_);
                break;
            }
            else if (0 == recv_len)
            {
                spdlog::info("[Neon FixClient warning]: received message size is 0，the connection is broken, sock_fd:{}", sock_fd_);
                break;
            }
            else
            {
                on_rcv_msg_(recv_len, recv_buf);
            }
#endif // __linux__
        }
        catch (std::exception e)
        {
            spdlog::info("Exception when received from OCG[{}]", e.what());
        }
    }
    spdlog::info("Neon FIX listening was broken!");
}
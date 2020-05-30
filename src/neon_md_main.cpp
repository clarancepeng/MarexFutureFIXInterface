#include <iostream>
#include <spdlog/spdlog.h>
#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <spdlog/async.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <mutex>
#include <condition_variable>
#include <boost/lockfree/queue.hpp>
#include "../include/message_encode.h"
#include "../include/fix_client.h"
#include "../include/incoming_message_handle.h"

std::mutex mtx_;
std::condition_variable cv_;
boost::lockfree::queue<char *, boost::lockfree::capacity<128>> lockfree_queue_;
MessageHeader message_header_;
MessageTrailer message_trailer_;
DataStore data_store_;
MessageEncode message_encode_;
FixClient fix_client_;
IncomingMessageHandle incoming_message_handle_;
char host_[] = "trainerlogin.neon.trade"; // "37.46.4.67"; //"trainerlogin.neon.trade";
short port_ = 5012;
char sender_comp_id_[] = "VBKRTEST";
char sender_sub_id_[] = "VBKRTEST";
char target_comp_id_[] = "Marex"; //"MAREX";
char target_sub_id_[] = "ES";
char username_[] = "vbkrfixtest";
char password_[] = "trainer";
char sending_data_[1024];
short sending_len_;
int sending_seq_;
int request_id_ = 1;

void initLog()
{
    // change log pattern
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%f] [%l] [thread %t] %v");

    spdlog::set_level(spdlog::level::debug); // Set global log level to debug
    spdlog::debug("This message should be displayed..");

    // Set the default logger to file logger
    try
    {
        auto daily_logger = spdlog::daily_logger_mt("daily_logger", "logs/neon_md", 0, 5);
        daily_logger->flush_on(spdlog::level::trace);
        spdlog::set_default_logger(daily_logger);
    }
    catch (const spdlog::spdlog_ex &ex)
    {
        std::cout << "Log initialization failed: " << ex.what() << std::endl;
    }
}
void handleHeartBeat()
{
    HeartBeat heart_beat;
    memset(&heart_beat, 0, sizeof(HeartBeat));
    sending_len_ = 0;
    memset(sending_data_, 0, 1024);
    memcpy(message_header_.msg_type, HEART_BEAT, sizeof(HEART_BEAT));
    heart_beat.message_header = &message_header_;
    heart_beat.message_trailer = &message_trailer_;
    message_encode_.EncodeHeartBeat(sending_data_, &sending_len_, &sending_seq_, &heart_beat);
    spdlog::info("{} HeartBeat - SeqNum({}), FIX: {}", __FILE__, sending_seq_, string(sending_data_, sending_len_));
}

void handleLogon()
{
    Logon logon;
    memset(&logon, 0, sizeof(Logon));
    sending_len_ = 0;
    memset(sending_data_, 0, 1024);
    logon.message_header = &message_header_;
    memcpy(message_header_.msg_type, LOGON, sizeof(LOGON));
    memcpy(logon.user_name, username_, strlen(username_));
    memcpy(logon.password, password_, strlen(password_));
    logon.message_trailer = &message_trailer_;
    message_encode_.EncodeLogon(sending_data_, &sending_len_, &sending_seq_, &logon);
    spdlog::info("Logon - SeqNum({}), FIX: {}", sending_seq_, string(sending_data_, sending_len_));
    fix_client_.SendMsg(sending_data_, sending_len_, sending_seq_, (char *)LOGON);
}

void handleSecurityDefinitionRequest(char* security_exchange)
{
    SecurityDefinitionRequest security_definition_req;
    memset(&security_definition_req, 0, sizeof(SecurityDefinitionRequest));
    sending_len_ = 0;
    memset(sending_data_, 0, 1024);
    security_definition_req.message_header = &message_header_;
    security_definition_req.security_request_type = 1;
    memcpy(security_definition_req.instrument.security_exchange, security_exchange, strlen(security_exchange));
    string req_id = std::to_string(request_id_++);
    memcpy(security_definition_req.security_req_id, req_id.c_str(), req_id.length());
    memcpy(message_header_.msg_type, SECURITY_DEFINITION_REQUEST, sizeof(SECURITY_DEFINITION_REQUEST));
    security_definition_req.message_trailer = &message_trailer_;
    message_encode_.EncodeSecurityDefinitionRequest(sending_data_, &sending_len_, &sending_seq_, &security_definition_req);
    spdlog::info("SecurityDefinitionRequest - SeqNum({}), FIX: {}", sending_seq_, string(sending_data_, sending_len_));
    fix_client_.SendMsg(sending_data_, sending_len_, sending_seq_, (char *)SECURITY_DEFINITION_REQUEST);
}

void handleMarketDataRequest()
{
    MarketDataRequest md_req;
    memset(&md_req, 0, sizeof(MarketDataRequest));
    sending_len_ = 0;
    memset(sending_data_, 0, 1024);
    md_req.message_header = &message_header_;
    memcpy(message_header_.msg_type, SECURITY_DEFINITION_REQUEST, sizeof(SECURITY_DEFINITION_REQUEST));
    md_req.message_trailer = &message_trailer_;
    message_encode_.EncodeMarketDataRequest(sending_data_, &sending_len_, &sending_seq_, &md_req);
    spdlog::info("MarketDataRequest: - SeqNum({}), FIX: {}", sending_seq_, string(sending_data_, sending_len_));
}

void messageHandle()
{
    char *msg_type;
    while (true)
    {
        // MarketData* md;
        while (lockfree_queue_.pop(msg_type))
        {
            try
            {
                if (strcmp(msg_type, HEART_BEAT) == 0)
                {
                    spdlog::info(">>>HeartBeat");
                    handleHeartBeat();
                }
                else if (strcmp(msg_type, LOGON) == 0)
                {
                    spdlog::info(">>>Logon");
                    handleLogon();
                }
                else if (strcmp(msg_type, SECURITY_DEFINITION_REQUEST) == 0)
                {
                    spdlog::info(">>>SecurityDefinitionRequest");
                    handleSecurityDefinitionRequest("XCBF");
                }
                else if (strcmp(msg_type, MARKET_DATA_REQUEST) == 0)
                {
                    spdlog::info(">>>MarketDataRequest");
                    handleMarketDataRequest();
                }
            }
            catch (std::exception &e)
            {
                spdlog::info("Error when Handle the message delivery: {}", e.what());
            }
            catch (...)
            {
                spdlog::info("Other errors!");
            }
        }
        usleep(1);
    }
}

void getIP(char *domain_name, char *ip_addr)
{
    hostent *host = gethostbyname(domain_name);
    sprintf(ip_addr, "%d.%d.%d.%d",
            host->h_addr_list[0][0] & 0x00ff,
            host->h_addr_list[0][1] & 0x00ff,
            host->h_addr_list[0][2] & 0x00ff,
            host->h_addr_list[0][3] & 0x00ff);
}

void OnRecvMsg(int len, char *data)
{
    incoming_message_handle_.OnReceiveMsg(len, data);
}

void Init()
{
    initLog();
    memset(&message_header_, 0, sizeof(MessageHeader));
    memcpy(message_header_.begin_string, FIX_VER_44, strlen(FIX_VER_44));
    //short body_length;
    //memcpy(message_header_.msg_type, HEART_BEAT, 2);
    memcpy(message_header_.sender_comp_id, sender_comp_id_, strlen(sender_comp_id_));
    memcpy(message_header_.target_comp_id, target_comp_id_, strlen(target_comp_id_));
    //int msg_seq_num;
    memcpy(message_header_.sender_sub_id, sender_sub_id_, strlen(sender_sub_id_));
    memcpy(message_header_.target_sub_id, target_sub_id_, strlen(target_sub_id_));
    //char sending_time[22];
    //char poss_dup_flag; //N
    //char poss_resend; //N
    //char orig_sending_time[22]; //N

    std::thread msg_send_thread(&messageHandle);
    spdlog::info("Fix Message Send Thread: {}", (unsigned long long)msg_send_thread.native_handle());
    msg_send_thread.detach();

    data_store_.Init(5, 1);
    message_encode_.Init(&data_store_);
    char ip_addr[32];
    getIP(host_, ip_addr);
    fix_client_.Init(&data_store_, &OnRecvMsg, ip_addr, port_, false, 1);
}

void Add(MsgType msg_type)
{
    while (!lockfree_queue_.push(msg_type))
        ;
}

int main(int argc, char **argv)
{
    if (argc > 0)
    {
        std::cout << "" << argv[1];
    }

    Init();
    //Add((char *)HEART_BEAT);
    Add((char *)LOGON);
    sleep(1);
    Add((char *)SECURITY_DEFINITION_REQUEST);
    //Add((char *)MARKET_DATA_REQUEST);
    std::unique_lock<std::mutex> lck(mtx_);

    cv_.wait(lck);
    return 0;
}
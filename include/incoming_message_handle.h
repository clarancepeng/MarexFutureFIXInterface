#ifndef INCOMING_MESSAGE_HANDLE_H_
#define INCOMING_MESSAGE_HANDLE_H_
#include <memory>
#include <spdlog/spdlog.h>
#include "message_decode.h"

class IncomingMessageHandle {
public:
    IncomingMessageHandle() {
        //logger_ = spdlog::get("some_logger");
        message_decode_ = std::make_shared<MessageDecode>();
    }
    ~IncomingMessageHandle() {
        if(message_decode_) {
            message_decode_.reset();
            message_decode_ = nullptr;
        }
    }
    void OnReceiveMsg(int& len, char* data);
    void HandleMsg(int& len, char* data);
private:
    char temp_buf_[2048];
    int pos_ = 0;
    std::shared_ptr<MessageDecode> message_decode_;
    //std::shared_ptr<spdlog::logger> logger_;
};
#endif // INCOMING_MESSAGE_HANDLE_H_
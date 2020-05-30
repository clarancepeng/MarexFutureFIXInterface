#ifndef MESSAGE_ENCODE_H_
#define MESSAGE_ENCODE_H_
#include <sys/time.h>
#include <ctime>
#include "fix/fix_data_type.h"
#include "data_store.h"

class MessageEncode {
public:
    void Init(DataStore* data_store) {
        data_store_ = data_store;
    }
    void EncodeHeartBeat(char* data, short* len, int* seq_num, HeartBeat* heart_beat);
    void EncodeLogon(char* data, short* len, int* seq_num, Logon* logon);
    void EncodeTestRequest(char* data, short* len, int* seq_num, TestRequest* test_request);
    void EncodeResendRequest(char* data, short* len, int* seq_num, ResendRequest* resend_request);
    void EncodeReject(char* data, short* len, int* seq_num, Reject* reject);
    void EncodeSequenceReset(char* data, short* len, int* seq_num, SequenceReset* sequence_reset);
    void EncodeLogout(char* data, short* len, int* seq_num, Logout* logout);
    void EncodeMarketDataRequest(char* data, short* len, int* seq_num, MarketDataRequest* market_data_request);
    void EncodeSecurityDefinitionRequest(char* data, short* len, int* seq_num, SecurityDefinitionRequest* request);
    void EncodeBusinessMessageReject(char* data, short* len, int* seq_num, BusinessMessageReject* business_message_reject);

private:
    DataStore* data_store_;
    void setOutgoingSeqNum(MessageHeader *message_header, int* seq_num) {
        *seq_num = data_store_->GetNextOutMsgSeq();
        message_header->msg_seq_num = *seq_num;
    }

    void encodeHeaderCommon(char* data, short* len, int* seq_num, MessageHeader* message_header) {
        setOutgoingSeqNum(message_header, seq_num);
        getCurrentDateTime(message_header->sending_time);
        message_header->sending_time[21] = 0;
        appendFIX(data +(*len), len, FixTags::MsgType, message_header->msg_type);
        appendFIX(data +(*len), len, FixTags::MsgSeqNum, message_header->msg_seq_num);
        appendFIX(data +(*len), len, FixTags::SenderCompID, message_header->sender_comp_id);
        appendFIX(data +(*len), len, FixTags::SenderSubID, message_header->sender_sub_id);
        appendFIX(data +(*len), len, FixTags::SendingTime, message_header->sending_time);
        appendFIX(data +(*len), len, FixTags::TargetCompID, message_header->target_comp_id);
        appendFIX(data +(*len), len, FixTags::TargetSubID, message_header->target_sub_id);
    }

    void appendFIX(char* data, short* len, const int& tag, int& tag_val) {
        string tag_s = std::to_string(tag);
        int pos = tag_s.length();
        memcpy(data, tag_s.c_str(), pos);
        data[pos] = EQUAL;
        pos += 1;
        string tag_v = std::to_string(tag_val);
        memcpy(data + pos, tag_v.c_str(), tag_v.length());
        pos += tag_v.length();
        data[pos] = SOH;
        *len = (*len) + 1 + pos;
    }

    void appendFIX(char* data, short* len, const int& tag, const char* tag_val) {
        string tag_s = std::to_string(tag);
        int pos = tag_s.length();
        memcpy(data, tag_s.c_str(), pos);
        data[pos] = EQUAL;
        pos += 1;
        int tag_v_len = strlen(tag_val);
        memcpy(data + pos, tag_val, tag_v_len);
        pos += tag_v_len;
        data[pos] = SOH;
        *len = (*len) + 1 + pos;
    }

    void appendFIX(char* data, short* len, const int& tag, double& tag_val) {
        string tag_s = std::to_string(tag);
        int pos = tag_s.length();
        memcpy(data, tag_s.c_str(), pos);
        data[pos] = EQUAL;
        pos += 1;
        string tag_v = std::to_string(tag_val);
        memcpy(data + pos, tag_v.c_str(), tag_v.length());
        pos += tag_v.length();
        data[pos] = SOH;
        *len = (*len) + 1 + pos;
    }

    void appendFIX(char* data, short* len, const int& tag, short& tag_val) {
        string tag_s = std::to_string(tag);
        int pos = tag_s.length();
        memcpy(data, tag_s.c_str(), pos);
        data[pos] = EQUAL;
        pos += 1;
        string tag_v = std::to_string(tag_val);
        memcpy(data + pos, tag_v.c_str(), tag_v.length());
        pos += tag_v.length();
        data[pos] = SOH;
        *len = (*len) + 1 + pos;
    }

    void appendFIX(char* data, short* len, const int& tag, char tag_val) {
        string tag_s = std::to_string(tag);
        int pos = tag_s.length();
        memcpy(data, tag_s.c_str(), pos);
        data[pos] = EQUAL;
        pos += 1;
        data[pos] = tag_val;
        pos += 1;
        data[pos] = SOH;
        *len = (*len) + 1 + pos;
    }

    void encodeHeaderAndTrailer(char* data, short* len) {
        string body_len_s = std::to_string(*len);
        int hl = 13 + body_len_s.length(); //strlen(FIX_VER_44) + 3 + body_len_s.length() + 3;
        memmove((data+hl), data, *len);
        short len2 = 0;
        
        appendFIX(data, &len2, FixTags::BeginString, FIX_VER_44);
        appendFIX(data + len2, &len2, FixTags::BodyLength, const_cast<char *>(body_len_s.c_str()));

        // append checksum
        int sum = 0;
        *len = len2 + (*len);
        short total_len = *len;
        for(int i = 0; i < total_len; i++) {
            sum += (int)data[i];
        }
        sum = sum &0xFF;
        char checksum[4] = {'0', '0', '0', 0};
        if(sum < 10) {
            checksum[2] = '0' + sum;
        } else if(sum < 100) {
            checksum[1] = '0' + sum/10;            
            checksum[2] = '0' + sum%10;
        } else {
            checksum[0] = '0' + sum/100;
            sum %= 100;
            checksum[1] = '0' + sum/10;
            checksum[2] = '0' + sum%10;
        }
        appendFIX(data + (*len), len, FixTags::CheckSum, checksum);
    }
    
    void getCurrentDateTime(char* current_time) {
        timeval cur_time;
        gettimeofday(&cur_time, nullptr);
        int milli = cur_time.tv_usec / 1000;
        char buf[80];
        strftime(buf, sizeof(buf), "%Y%m%d-%X", gmtime(&cur_time.tv_sec));
        sprintf(current_time, "%s.%03d", buf, milli);
    }
    
};
#endif // MESSAGE_ENCODE_H_
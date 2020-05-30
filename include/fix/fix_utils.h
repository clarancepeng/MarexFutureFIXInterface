#ifndef FIX_UTILS_H_
#define FIX_UTILS_H_
#include "fix_data_type.h"
#include <spdlog/spdlog.h>

namespace FixUtils
{
inline void GetFIXVersion(char *fix_version, char *data, int &pos)
{
    bool is_val = false;
    int count = 0;
    int tags = 0;
    for (int i = 0; i < 12; i++)
    {
        char curr = data[pos++];
        if (curr == SOH)
        {
            break;
        }
        if (curr == EQUAL)
        {
            if (tags != 8)
            {
                spdlog::info("Invalid tags for BeginString, expect 8, get {}", tags);
                break;
            }
            is_val = true;
        }
        else
        {
            if (is_val)
            {
                fix_version[count++] = curr;
            }
            else
            {
                tags = tags * 10 + curr - 48;
            }
        }
    }
}

inline void GetBodyLen(int &body_len, int &body_data_len, char *data, int &pos)
{
    bool is_val = false;
    int tags = 0;
    for (int i = 0; i < 12; i++)
    {
        char curr = data[pos++];
        if (curr == SOH)
        {
            break;
        }
        if (curr == EQUAL)
        {
            if (tags != 9)
            {
                spdlog::info("Invalid tags for BeginString, expect 8, get {}", tags);
                break;
            }
            is_val = true;
        }
        else
        {
            if (is_val)
            {
                body_len = body_len * 10 + curr - 48;
                ++body_data_len;
            }
            else
            {
                tags = tags * 10 + curr - 48;
            }
        }
    }
}

inline void GetMsgType(char *msg_type, char *data, int &pos)
{
    bool is_val = false;
    int count = 0;
    int tags = 0;
    for (int i = 0; i < 12; i++)
    {
        char curr = data[pos++];
        if (curr == SOH)
        {
            break;
        }
        if (curr == EQUAL)
        {
            if (tags != 35)
            {
                spdlog::info("Invalid tags for BeginString, expect 35, get {}", tags);
                break;
            }
            is_val = true;
        }
        else
        {
            if (is_val)
            {
                msg_type[count++] = curr;
            }
            else
            {
                tags = tags * 10 + curr - 48;
            }
        }
    }
}

inline void DecodeHeader(char *data, int &pos, int &data_len, MessageHeader *message_header)
{
    int tags = 0;
    bool is_val = false;
    char tmp_val[100];
    int count = 0;
    for (int i = pos; i < data_len; i++)
    {
        char curr = data[i];
        if (curr == SOH)
        {
            if (tags == FixTags::BeginString || tags == FixTags::BodyLength || tags == FixTags::MsgType || tags == FixTags::SenderCompID || tags == FixTags::TargetCompID || tags == FixTags::MsgSeqNum || tags == FixTags::SenderSubID || tags == FixTags::TargetSubID || tags == FixTags::PossDupFlag || tags == FixTags::PossResend || tags == FixTags::SendingTime || tags == FixTags::OrigSendingTime)
            {
                pos = i;
            }
            else
            {
                break;
            }

            switch (tags)
            {
            case FixTags::BeginString:
                memcpy(message_header->begin_string, tmp_val, count);
                break;
            case FixTags::BodyLength:
                tmp_val[count] = 0;
                message_header->body_length = atoi(tmp_val);
                break;
            case FixTags::MsgType:
                memcpy(message_header->msg_type, tmp_val, count);
                break;
            case FixTags::SenderCompID:
                memcpy(message_header->sender_comp_id, tmp_val, count);
                break;
            case FixTags::TargetCompID:
                memcpy(message_header->target_comp_id, tmp_val, count);
                break;
            case FixTags::MsgSeqNum:
                tmp_val[count] = 0;
                message_header->msg_seq_num = atoi(tmp_val);
                break;
            case FixTags::SenderSubID:
                memcpy(message_header->sender_sub_id, tmp_val, count);
                break;
            case FixTags::TargetSubID:
                memcpy(message_header->target_sub_id, tmp_val, count);
                break;
            case FixTags::PossDupFlag:
                message_header->poss_dup_flag = tmp_val[0];
                break;
            case FixTags::PossResend:
                message_header->poss_resend = tmp_val[0];
                break;
            case FixTags::SendingTime:
                memcpy(message_header->sending_time, tmp_val, count);
                break;
            case FixTags::OrigSendingTime:
                memcpy(message_header->orig_sending_time, tmp_val, count);
                break;
            default:
                break;
            }
            is_val = false;
            tags = 0;
            count = 0;
            continue;
        }

        if (curr == EQUAL)
        {
            is_val = true;
        }
        else
        {
            if (is_val)
            {
                tmp_val[count++] = curr;
            }
            else
            {
                tags = tags * 10 + curr - 48;
            }
        }
    }
}

}; // namespace FixUtils

#endif // FIX_UTILS_H_
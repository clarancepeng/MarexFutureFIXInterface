#include "../include/incoming_message_handle.h"
#include "../include/message_decode.h"
#include "../include/fix/fix_utils.h"

void IncomingMessageHandle::OnReceiveMsg(int &len, char *data)
{
    spdlog::debug("Recv: len={}, content={}", len, data);
    int total_len = pos_ + len;
    if (pos_ > 0)
    {
        memcpy(temp_buf_ + pos_, data, len);
        HandleMsg(total_len, temp_buf_);
    }
    else
    {
        HandleMsg(len, data);
    }
}

/**
 * Handle TCP stick or divide packed
 */
void IncomingMessageHandle::HandleMsg(int &len, char *data)
{
    int curr_pos = 0;
    int left;
    while (true)
    {
        left = len - curr_pos;
        if (left < 50)
        {
            if (left > 0)
            {
                pos_ = len;
                memcpy(temp_buf_, data + curr_pos, left);
            }
            else
            {
                pos_ = 0;
            }
            return;
        }
        char fix_ver[10] = {0};
        int tmp_pos = 0;
        FixUtils::GetFIXVersion(fix_ver, data + curr_pos, tmp_pos);
        int body_len = 0, body_data_len = 0;
        FixUtils::GetBodyLen(body_len, body_data_len, data, tmp_pos);
        int msg_len = 3 + strlen(fix_ver) + 3 + body_data_len + body_len + 7;
        if (msg_len > left)
        {
            pos_ = left;
            memcpy(temp_buf_, data + curr_pos, left);
            break;
        }
        else
        {
            MessageHeader message_header;
            memset(&message_header, 0, sizeof(MessageHeader));
            int header_pos = 0;
            FixUtils::DecodeHeader(data + curr_pos, header_pos, msg_len, &message_header);
            if (strcmp(message_header.msg_type, MARKET_DATA_UPDATE) == 0)
            {
                spdlog::info("[MARKET_DATA_UPDATE] - {}", string(data+curr_pos, msg_len));
                MarketDataUpdate update;
                memset(&update, 0, sizeof(MarketDataUpdate));
                update.message_header = &message_header;
                message_decode_.get()->DecodeMarketDataUpdate(data+curr_pos, &header_pos, &update);
            }
            else if (strcmp(message_header.msg_type, MARKET_DATA_SNAPSHOT) == 0)
            {
                spdlog::info("[MARKET_DATA_SNAPSHOT] - {}", string(data+curr_pos, msg_len));
                MarketDataSnapshot snapshot;
                memset(&snapshot, 0, sizeof(MarketDataSnapshot));
                snapshot.message_header = &message_header;
                message_decode_.get()->DecodeMarketDataSnapshot(data+curr_pos, &header_pos, &snapshot);
            }
            else if (strcmp(message_header.msg_type, MARKET_DATA_REQUEST_REJECT) == 0)
            {
                spdlog::info("[MARKET_DATA_REQUEST_REJECT] - {}", string(data+curr_pos, msg_len));
                MarketDataRequestReject reject;
                memset(&reject, 0, sizeof(MarketDataRequestReject));
                reject.message_header = &message_header;
                message_decode_.get()->DecodeMarketDataRequestReject(data+curr_pos, &header_pos, &reject);
            }
            else if (strcmp(message_header.msg_type, SECURITY_DEFINITION) == 0)
            {
                spdlog::info("[SECURITY_DEFINITION] - {}", string(data+curr_pos, msg_len));
                SecurityDefinition ref_data;
                memset(&ref_data, 0, sizeof(SecurityDefinition));
                ref_data.message_header = &message_header;
                message_decode_.get()->DecodeSecurityDefinition(data+curr_pos, &header_pos, &ref_data);
            }
            else if (strcmp(message_header.msg_type, LOGON) == 0)
            {
                spdlog::info("[LOGON] - {}", string(data+curr_pos, msg_len));
                Logon logon;
                memset(&logon, 0, sizeof(Logon));
                logon.message_header = &message_header;
                message_decode_.get()->DecodeLogon(data+curr_pos, &header_pos, &logon);
            }
            else if (strcmp(message_header.msg_type, TEST_REQUEST) == 0)
            {
                spdlog::info("[TEST_REQUEST] - {}", string(data+curr_pos, msg_len));
                TestRequest test_request;
                memset(&test_request, 0, sizeof(TestRequest));
                test_request.message_header = &message_header;
                message_decode_.get()->DecodeTestRequest(data+curr_pos, &header_pos, &test_request);
            }
            else if (strcmp(message_header.msg_type, RESEND_REQUEST) == 0)
            {
                spdlog::info("[RESEND_REQUEST] - {}", string(data+curr_pos, msg_len));
                ResendRequest resend_request;
                memset(&resend_request, 0, sizeof(ResendRequest));
                resend_request.message_header = &message_header;
                message_decode_.get()->DecodeResendRequest(data+curr_pos, &header_pos, &resend_request);
            }
            else if (strcmp(message_header.msg_type, REJECT) == 0)
            {
                spdlog::info("[REJECT] - {}", string(data+curr_pos, msg_len));
                Reject reject;
                memset(&reject, 0, sizeof(Reject));
                reject.message_header = &message_header;
                message_decode_.get()->DecodeReject(data+curr_pos, &header_pos, &reject);
            }
            else if (strcmp(message_header.msg_type, SEQUENCE_RESET) == 0)
            {
                spdlog::info("[SEQUENCE_RESET] - {}", string(data+curr_pos, msg_len));
                SequenceReset sequence_reset;
                memset(&sequence_reset, 0, sizeof(SequenceReset));
                sequence_reset.message_header = &message_header;
                message_decode_.get()->DecodeSequenceReset(data+curr_pos, &header_pos, &sequence_reset);
            }
            else if (strcmp(message_header.msg_type, LOGOUT) == 0)
            {
                spdlog::info("[LOGOUT] - {}", string(data+curr_pos, msg_len));
                Logout logout;
                memset(&logout, 0, sizeof(Logout));
                logout.message_header = &message_header;
                message_decode_.get()->DecodeLogout(data+curr_pos, &header_pos, &logout);
            }
            else if (strcmp(message_header.msg_type, BUSINESS_MESSAGE_REJECT) == 0)
            {
                spdlog::info("[BUSINESS_MESSAGE_REJECT] - {}", string(data+curr_pos, msg_len));
                BusinessMessageReject business_reject;
                memset(&business_reject, 0, sizeof(BusinessMessageReject));
                business_reject.message_header = &message_header;
                message_decode_.get()->DecodeBusinessMessageReject(data+curr_pos, &header_pos, &business_reject);
            }
            else if (strcmp(message_header.msg_type, HEART_BEAT) == 0)
            {
                spdlog::info("[HEART_BEAT] - {}", string(data+curr_pos, msg_len));
                HeartBeat heart_beat;
                memset(&heart_beat, 0, sizeof(HeartBeat));
                heart_beat.message_header = &message_header;
                message_decode_.get()->DecodeHeartBeat(data+curr_pos, &header_pos, &heart_beat);
            }
            curr_pos += msg_len;
        }
    }
}
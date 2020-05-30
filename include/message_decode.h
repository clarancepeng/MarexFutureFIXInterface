#ifndef MESSAGE_DECODE_H_
#define MESSAGE_DECODE_H_
#include "fix/fix_data_type.h"
class MessageDecode {
public:
    void DecodeHeartBeat(char* data, int* pos, HeartBeat* heart_beat);
    void DecodeLogon(char* data, int* pos, Logon* logon);
    void DecodeTestRequest(char* data, int* pos, TestRequest* test_request);
    void DecodeResendRequest(char* data, int* lposen, ResendRequest* resend_request);
    void DecodeReject(char* data, int* pos, Reject* reject);
    void DecodeSequenceReset(char* data, int* pos, SequenceReset* sequence_reset);
    void DecodeLogout(char* data, int* pos, Logout* logout);
    void DecodeMarketDataSnapshot(char* data, int* pos, MarketDataSnapshot* snapshot);
    void DecodeMarketDataUpdate(char* data, int* pos, MarketDataUpdate* update);
    void DecodeMarketDataRequestReject(char* data, int* pos, MarketDataRequestReject* reject);
    void DecodeSecurityDefinition(char* data, int* pos, SecurityDefinition* security_definition);
    void DecodeBusinessMessageReject(char* data, int* pos, BusinessMessageReject* business_message_reject);
};
#endif // MESSAGE_DECODE_H_
#include "../include/message_decode.h"

void MessageDecode::DecodeHeartBeat(char *data, int *pos, HeartBeat *heart_beat)
{
    
}

void MessageDecode::DecodeLogon(char *data, int *pos, Logon *logon)
{
}
void MessageDecode::DecodeTestRequest(char *data, int *pos, TestRequest *test_request)
{
}
void MessageDecode::DecodeResendRequest(char *data, int *pos, ResendRequest *resend_request)
{
}
void MessageDecode::DecodeReject(char *data, int *pos, Reject *reject)
{
}
void MessageDecode::DecodeSequenceReset(char *data, int *pos, SequenceReset *sequence_reset)
{
}
void MessageDecode::DecodeLogout(char *data, int *pos, Logout *logout)
{
}
void MessageDecode::DecodeMarketDataSnapshot(char *data, int *pos, MarketDataSnapshot *snapshot)
{
}

void MessageDecode::DecodeMarketDataUpdate(char* data, int* pos, MarketDataUpdate* update) {

}
void MessageDecode::DecodeMarketDataRequestReject(char *data, int *pos, MarketDataRequestReject *reject)
{
}
void MessageDecode::DecodeSecurityDefinition(char *data, int *pos, SecurityDefinition *security_definition)
{
}
void MessageDecode::DecodeBusinessMessageReject(char *data, int *pos, BusinessMessageReject *business_message_reject)
{
}
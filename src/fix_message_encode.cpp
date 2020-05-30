#include "../include/message_encode.h"

void MessageEncode::EncodeHeartBeat(char* data, short* len, int* seq_num, HeartBeat *heart_beat)
{
    encodeHeaderCommon(data, len, seq_num, heart_beat->message_header);
    if(strlen(heart_beat->test_request_id) > 0) {
        appendFIX(data + (*len), len, FixTags::TestReqID, heart_beat->test_request_id);
    }
    encodeHeaderAndTrailer(data, len);
}

void MessageEncode::EncodeLogon(char* data, short* len, int* seq_num, Logon *logon)
{
    encodeHeaderCommon(data, len, seq_num, logon->message_header);
    appendFIX(data + (*len), len, FixTags::EncryptMethod, '0');
    short hb_int = logon->heart_beat_int > 0? logon->heart_beat_int: 30;
    appendFIX(data + (*len), len, FixTags::HeartBtInt, hb_int);
    appendFIX(data + (*len), len, FixTags::ResetSeqNumFlag, 'N');
    appendFIX(data + (*len), len, FixTags::Username, logon->user_name);
    appendFIX(data + (*len), len, FixTags::Password, logon->password);
    encodeHeaderAndTrailer(data, len);
}

void MessageEncode::EncodeTestRequest(char* data, short* len, int* seq_num, TestRequest *test_request)
{
    encodeHeaderCommon(data, len, seq_num, test_request->message_header);
    appendFIX(data + (*len), len, FixTags::TestReqID, test_request->test_request_id);
    encodeHeaderAndTrailer(data, len);
}

void MessageEncode::EncodeResendRequest(char* data, short* len, int* seq_num, ResendRequest *resend_request)
{
    encodeHeaderCommon(data, len, seq_num, resend_request->message_header);
    appendFIX(data + (*len), len, FixTags::BeginSeqNo, resend_request->begin_seq_no);
    appendFIX(data + (*len), len, FixTags::EndSeqNo, resend_request->end_seq_no);
    encodeHeaderAndTrailer(data, len);
}

void MessageEncode::EncodeReject(char* data, short* len, int* seq_num, Reject *reject)
{
    encodeHeaderCommon(data, len, seq_num, reject->message_header);
    appendFIX(data + (*len), len, FixTags::RefSeqNum, reject->ref_seq_no);
    appendFIX(data + (*len), len, FixTags::RefTagID, reject->ref_tag_id);
    appendFIX(data + (*len), len, FixTags::RefMsgType, reject->ref_msg_type);
    appendFIX(data + (*len), len, FixTags::SessionRejectReason, reject->session_reject_reason);
    appendFIX(data + (*len), len, FixTags::Text, reject->text);
    encodeHeaderAndTrailer(data, len);
}

void MessageEncode::EncodeSequenceReset(char* data, short* len, int* seq_num, SequenceReset *sequence_reset)
{
    encodeHeaderCommon(data, len, seq_num, sequence_reset->message_header);
    appendFIX(data + (*len), len, FixTags::GapFillFlag, 'Y'); //sequence_reset->gap_fill_flag);
    appendFIX(data + (*len), len, FixTags::NewSeqNo, sequence_reset->new_seq_no);
    encodeHeaderAndTrailer(data, len);
}

void MessageEncode::EncodeLogout(char* data, short* len, int* seq_num, Logout *logout)
{
    encodeHeaderCommon(data, len, seq_num, logout->message_header);
    appendFIX(data + (*len), len, FixTags::Text, logout->text);
    encodeHeaderAndTrailer(data, len);
}

void MessageEncode::EncodeMarketDataRequest(char* data, short* len, int* seq_num, MarketDataRequest *market_data_request)
{
    encodeHeaderCommon(data, len, seq_num, market_data_request->message_header);
    appendFIX(data + (*len), len, FixTags::MDReqID, market_data_request->md_req_id);
    appendFIX(data + (*len), len, FixTags::SubscriptionRequestType, market_data_request->subscription_request_type);
    appendFIX(data + (*len), len, FixTags::MarketDepth, market_data_request->market_depth);
    if(market_data_request->subscription_request_type == 1) {
        appendFIX(data + (*len), len, FixTags::MDUpdateType, '1'); // 1 = Incremental Refresh
    }
    // 0 = Bid
    // 1 = Offer
    // 2 = Trade
    // 4 = Opening
    // 5 = Closing
    // 6 = Settlement
    // 7 = TradingHigh
    // 8 = TradingLow
    // B = TradeVolume (FIX 4.4 only)
    appendFIX(data + (*len), len, FixTags::NoMDEntryTypes, '9');
    appendFIX(data + (*len), len, FixTags::MDEntryType, '0');
    appendFIX(data + (*len), len, FixTags::MDEntryType, '1');
    appendFIX(data + (*len), len, FixTags::MDEntryType, '2');
    appendFIX(data + (*len), len, FixTags::MDEntryType, '4');
    appendFIX(data + (*len), len, FixTags::MDEntryType, '5');
    appendFIX(data + (*len), len, FixTags::MDEntryType, '6');
    appendFIX(data + (*len), len, FixTags::MDEntryType, '7');
    appendFIX(data + (*len), len, FixTags::MDEntryType, '8');
    appendFIX(data + (*len), len, FixTags::MDEntryType, 'B');
    if(market_data_request->no_related_sym > 0) {
        appendFIX(data + (*len), len, FixTags::NoRelatedSym, market_data_request->no_related_sym);
        for(int i = 0; i < market_data_request->no_related_sym; i++) {
            appendFIX(data + (*len), len, FixTags::Symbol, market_data_request->instrument_details[i]->symbol);
        }
    }
    encodeHeaderAndTrailer(data, len);
}

void MessageEncode::EncodeSecurityDefinitionRequest(char* data, short* len, int* seq_num, SecurityDefinitionRequest* request)
{
    encodeHeaderCommon(data, len, seq_num, request->message_header);
    appendFIX(data + (*len), len, FixTags::SecurityReqID, request->security_req_id);
    appendFIX(data + (*len), len, FixTags::SecurityRequestType, request->security_request_type);
    if(strlen(request->instrument.symbol) > 0) {
        appendFIX(data + (*len), len, FixTags::Symbol, request->instrument.symbol);
    }
    if(strlen(request->instrument.security_exchange) > 0) {
        appendFIX(data + (*len), len, FixTags::SecurityExchange, request->instrument.security_exchange);
    }
    appendFIX(data + (*len), len, FixTags::SecurityType, "FUT");
    encodeHeaderAndTrailer(data, len);
}

void MessageEncode::EncodeBusinessMessageReject(char* data, short* len, int* seq_num, BusinessMessageReject* business_message_reject)
{
    encodeHeaderCommon(data, len, seq_num, business_message_reject->message_header);
    appendFIX(data + (*len), len, FixTags::RefSeqNum, business_message_reject->ref_seq_num);
    appendFIX(data + (*len), len, FixTags::RefMsgType, business_message_reject->ref_msg_type);
    appendFIX(data + (*len), len, FixTags::BusinessRejectReason, business_message_reject->business_reject_reason);
    appendFIX(data + (*len), len, FixTags::Text, business_message_reject->text);
    encodeHeaderAndTrailer(data, len);
}
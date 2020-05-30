#ifndef FIX_DATA_TYPE_H_
#define FIX_DATA_TYPE_H_
#include <cstring>
#include <string>
#include <iostream>
#include <cstddef>
#include "fix_tags.h"
#define FIX_VER_42 "FIX.4.2"
#define FIX_VER_43 "FIX.4.3"
#define FIX_VER_44 "FIX.4.4"
#define SOH 1
#define EQUAL '='

#if (WINDOWS | WIN32 | WINNT)
#define SOCK_ERR 10000
#else
#define SOCK_ERR -1
#endif

typedef char MsgType[2];
#define HEART_BEAT "0"
#define LOGON "A"
#define TEST_REQUEST "1"
#define RESEND_REQUEST "2"
#define REJECT "3"
#define SEQUENCE_RESET "4"
#define LOGOUT "5"
#define MARKET_DATA_REQUEST "V"
#define MARKET_DATA_SNAPSHOT "W"
#define MARKET_DATA_UPDATE "X"
#define MARKET_DATA_REQUEST_REJECT "Y"
#define SECURITY_DEFINITION_REQUEST "c"
#define SECURITY_DEFINITION "d"
#define BUSINESS_MESSAGE_REJECT "j"
using std::string;
typedef void (*OnRcvMsg)(int, char*);

struct MessageHeader {
    char begin_string[12];
    short body_length;
    char msg_type[2];
    char sender_comp_id[16];
    char target_comp_id[16];
    int msg_seq_num;
    char sender_sub_id[16];
    char target_sub_id[16];
    char sending_time[22];
    char poss_dup_flag; //N
    char poss_resend; //N
    char orig_sending_time[22]; //N
};

struct MessageTrailer {
    char check_sum[3];
};

struct HeartBeat {
    HeartBeat() {
        
    }
    //virtual ~HeartBeat();
    MessageHeader* message_header;
    char test_request_id[16];
    MessageTrailer* message_trailer;
};

struct Logon {
    Logon() {
        
    }
    //virtual ~Logon();
    MessageHeader* message_header;
    int encrypt_method;
    int heart_beat_int;
    char user_name[31];
    char reset_seq_num_flag;
    char password[40];
    MessageTrailer* message_trailer;
};

struct TestRequest {
    TestRequest() {
        
    }
    //virtual ~TestRequest();
    MessageHeader* message_header;
    char test_request_id[16];
    MessageTrailer* message_trailer;
};

struct ResendRequest {
    ResendRequest() {
        
    }
    //virtual ~ResendRequest();
    MessageHeader* message_header;
    int begin_seq_no;
    int end_seq_no;
    MessageTrailer* message_trailer;
};

struct Reject {
    Reject() {
        
    }
    //virtual ~Reject();
    MessageHeader* message_header;
    int ref_seq_no;
    int ref_tag_id;
    char ref_msg_type[2];
    int session_reject_reason;
    char text[60];
    MessageTrailer* message_trailer;
};

struct SequenceReset {
    SequenceReset() {
        
    }
    //virtual ~SequenceReset();
    MessageHeader* message_header;
    int new_seq_no;
    char gap_fill_flag;
    MessageTrailer* message_trailer;
};

struct Logout {
    Logout() {
        
    }
    //virtual ~Logout();
    MessageHeader* message_header;
    char text[60];
    MessageTrailer* message_trailer;
};

struct Instrument {
    char symbol[28];
    char security_exchange[4];
    char cfi_code[6];
    char security_type[6];
    char put_or_call;
    int maturity_month_year;
    double strike_price;
    char opt_attribute;
    int maturity_day;
    char symbol_sfx[28];
    char security_desc[80];
    int maturity_date;
    char security_sub_type[6];
};

struct MarketDataRequest {
    MarketDataRequest() {
        
    }
    //virtual ~MarketDataRequest();
    MessageHeader* message_header;
    char md_req_id[20];
    char subscription_request_type;
    short market_depth;
    char md_update_type;
    short no_md_entry_types;
    char md_entry_type[9]; // repeat group
    short no_related_sym;
    Instrument** instrument_details; // repeat group
    MessageTrailer* message_trailer;
};

struct MDEntry {
    char md_update_action; // only update message have this filed(0 - New, 1 - Change, 2 - Delete)
    char md_entry_type;
    char md_entry_ref_id[20];
    double md_entry_px;
    int md_entry_size;
    char md_entry_date[10];
    char md_entry_time[8];
    short md_entry_position_no;
};

struct MarketDataSnapshot {
    MarketDataSnapshot() {
        
    }
    //virtual ~MarketDataSnapshot();
    MessageHeader* message_header;
    char md_req_id[20];
    Instrument instrument;
    short no_md_entries;
    MDEntry md_entries[9];
    MessageTrailer* message_trailer;
};

struct MarketDataUpdate {
    MarketDataUpdate() {
        
    }
    //virtual ~MarketDataUpdate();
    MessageHeader* message_header;
    short no_md_entries;
    MDEntry md_entries[9];
    Instrument instrument;
    MessageTrailer* message_trailer;
};

struct MarketDataRequestReject {
    MarketDataRequestReject() {
        
    }
    //virtual ~MarketDataRequestReject();
    MessageHeader* message_header;
    char md_req_id[20];
    char md_req_rej_reason;
    char text[60];
    MessageTrailer* message_trailer;
};

struct SecurityDefinitionRequest {    
    SecurityDefinitionRequest() {
        
    }
    //virtual ~SecurityDefinitionRequest();
    MessageHeader* message_header;
    char security_req_id[20];
    short security_request_type;
    Instrument instrument;
    MessageTrailer* message_trailer;

};

struct SecurityDefinition {
    SecurityDefinition() {
        
    }
    //virtual ~SecurityDefinition();
    MessageHeader* message_header;
    char security_req_id[20];
    char security_response_id[20];
    short security_response_type;
    Instrument instrument;
    short total_num_seucrities;
    char text[60];
    MessageTrailer* message_trailer;
};

struct BusinessMessageReject {
    BusinessMessageReject() {
        
    }
    //virtual ~BusinessMessageReject();
    MessageHeader* message_header;
    int ref_seq_num;
    char ref_msg_type[2];
    short business_reject_reason;
    char text[60];
    MessageTrailer* message_trailer;
};
#endif // FIX_DATA_TYPE_H_
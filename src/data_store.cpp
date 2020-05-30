#include "../include/data_store.h"

void DataStore::Init(int next_out_msg_seq, int curr_in_coming_seq) {
    next_out_msg_seq_ = next_out_msg_seq;
    curr_in_coming_seq_ = curr_in_coming_seq;
}
void DataStore::StoreOutgoingMsg(const char* data, const short* len, const int* seq_num, const char* msg_type) {

}

void DataStore::StoreIncomingMsg(const char* data, const short* len, const int* seq_num) {

}

void DataStore::RecoverMsg() {

}

int DataStore::GetNextOutMsgSeq() {
    return next_out_msg_seq_++;
}

int DataStore::GetCurrIncomingSeq() {
    return curr_in_coming_seq_;
}
#ifndef DATA_STORE_H_
#define DATA_STORE_H_
class DataStore {
public:
    void Init(int next_out_msg_seq, int curr_in_coming_seq);
    void StoreOutgoingMsg(const char* data, const short* len, const int* seq_num, const char* msg_type);
    void StoreIncomingMsg(const char* data, const short* len, const int* seq_num);
    void RecoverMsg();
    int GetNextOutMsgSeq();
    int GetCurrIncomingSeq();
private:
    volatile int next_out_msg_seq_;
    volatile int curr_in_coming_seq_;
};

#endif // DATA_STORE_H_
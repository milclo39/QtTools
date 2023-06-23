#ifndef session_control_def_h
#define session_control_def_h


using namespace std;


class CommonResponse
{
public:
    int rval;
    int msg_id;
};

class StartSession_Rsp : public CommonResponse
{
public:
    int param;
    StartSession_Rsp(){}
    virtual ~StartSession_Rsp(){}
};

class ClCommonRPC_Rsp : public CommonResponse
{
public:
    string response_data;
};

#endif


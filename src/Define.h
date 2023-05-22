#ifndef DEFINE_H_
#define DEFINE_H_

#define SUCCESS 0
#define FAILD 1

#define ERR_SEND_UNCOMP 3

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }


struct PacketMeta
{
    uint32_t mMsgId;
    uint32_t mDataLen;   // the total len of one packet including the metadata
};

#endif /* DEFINE_H_ */
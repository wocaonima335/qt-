
#include "widget.h"
#include "protocol.h"
using namespace std;
PDU *mkPDU(uint uiMsgLen)
{
    try
    {
        uint uiPDULen = sizeof(PDU) + uiMsgLen;
                PDU *pdu = (PDU *)malloc(uiPDULen);

                if (NULL == pdu)
                {
                    exit(EXIT_FAILURE); // 错误退出程序
                }
                memset(pdu, 0, uiPDULen); // 数据初始化为0
                pdu->uiPDULen = uiPDULen; // 数据参数初始化
                pdu->uiMsgLen = uiMsgLen;  // 返回对象的指针
    }
    catch (bad_alloc &e) // 捕获内存分配失败的异常
    {
        cerr << "Memory allocation failed: " << e.what() << endl; // 输出错误信息

        return nullptr; // 返回空指针
    }
}

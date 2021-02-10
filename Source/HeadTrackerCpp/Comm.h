#ifndef _COMM
#define _COMM

/*
    Serial Communications (RS-232) for Windows
    C++ Version by Uwe Simmer, February 2012
*/

typedef float (*RCB)(unsigned char *Block, int nBytes);

class Comm
{
    public:
        Comm(int PortNr = 1, int Baudrate = 9600);
        int WriteBlock(unsigned char *Block, int nBytesToWrite);
        void SetReadCallback(RCB rcb) { ReadCallback = rcb; };
        int IsOpen(void) { return(Connected); };
        int WatchThread(void);
        void setPointer(float* ptr_yaw, bool* ptr_cal) { m_ptr_yaw = ptr_yaw; m_ptr_cal = ptr_cal; };
        void unsetPointer() { m_ptr_yaw = nullptr; m_ptr_cal = nullptr; };
        
        ~Comm(void);

    protected:
        char szPort[16];
        int  Connected;
        void *hComm;
        void *ReadEvent;
        void *WriteEvent;
        void *hWatchThread;
        unsigned long dwThreadID;
        RCB  ReadCallback;
        float m_cal = 0.0f;

        float* m_ptr_yaw = nullptr;
        bool* m_ptr_cal = nullptr;
        
};

#endif

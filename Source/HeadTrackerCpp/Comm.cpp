//------------------------------------------------------------------------------
//
//  Module: COMM
//
//  Purpose:
//     Serial Communication in Win32.
//
//  Description of functions:
//     Descriptions are contained in the function headers.
//
//  Written by Microsoft Product Support Services, Windows Developer Support.
//
//  C++ Version by Uwe Simmer, February 2012
//
//------------------------------------------------------------------------------

#include <windows.h>
#include <stdio.h>

#include "Comm.h"
#include "JuceHeader.h"
//#include "ErrorMsg.h"

#define MAXBLOCK 128
#define READ_TIMEOUT 500
DWORD CommWatchProc(LPSTR lpData);

//------------------------------------------------------------------------------

Comm::Comm(int PortNr, int Baudrate)
{
    DCB dcb;

    Connected = 0;
    hComm = INVALID_HANDLE_VALUE;
    hWatchThread = NULL;
    dwThreadID = 0;
    ReadCallback = NULL;
    ReadEvent = new OVERLAPPED;
    WriteEvent = new OVERLAPPED;

    if (ReadEvent == NULL || WriteEvent == NULL)
        return;

    FillMemory(ReadEvent, sizeof(OVERLAPPED), 0);
    FillMemory(WriteEvent, sizeof(OVERLAPPED), 0);

    // create the overlapped read event
    OVERLAPPED *osRead = (OVERLAPPED *) ReadEvent;
    osRead->hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (osRead->hEvent == NULL)
    {
        DBG("Failed to create read event!");
        return;
    }

    // create the overlapped write event
    OVERLAPPED *osWrite = (OVERLAPPED *) WriteEvent;
    osWrite->hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (osWrite->hEvent == NULL)
    {
        // error creating overlapped event handle
        DBG("Failed to create write event!");
        return;
    }

    if (PortNr >= 1 && PortNr <= 256)
    {
        sprintf(szPort, "\\\\.\\COM%d", PortNr);

        hComm = CreateFile(szPort,
                           GENERIC_READ | GENERIC_WRITE,
                           0,
                           0,
                           OPEN_EXISTING,
                           FILE_FLAG_OVERLAPPED,
                           0);
    }
    else
    {
        DBG("Invalid port number %d", PortNr);
        return;
    }

    // error opening port; abort
    if (hComm == INVALID_HANDLE_VALUE)
    {
        DBG("Cannot open COM%d", PortNr);
        return;
    }

    // get any early notifications
    SetCommMask(hComm, EV_RXCHAR);

    // setup device buffers
    SetupComm(hComm, 4096, 4096);

    // purge any information in the buffer
    PurgeComm(hComm, PURGE_TXABORT | PURGE_RXABORT |
                     PURGE_TXCLEAR | PURGE_RXCLEAR);

    // set up for overlapped I/O
    COMMTIMEOUTS CommTimeOuts;
    CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;
    CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
    CommTimeOuts.ReadTotalTimeoutConstant = 0;
    CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
    CommTimeOuts.WriteTotalTimeoutConstant = 0;

    if (!SetCommTimeouts(hComm, &CommTimeOuts))
    {
        DBG("Error setting time-outs");
        return;
    }

    FillMemory(&dcb, sizeof(dcb), 0);
    dcb.DCBlength = sizeof(dcb);

    if (!BuildCommDCB("9600,n,8,1", &dcb))
    {
        // Couldn't build the DCB. Usually a problem
        // with the communications specification string.
        DBG("Cannot build the DCB");
        return;
    }

    dcb.BaudRate = Baudrate;

    if (SetCommState(hComm, &dcb))
    {
        Connected = TRUE;

        // create a secondary thread to watch for an event
        hWatchThread = CreateThread((LPSECURITY_ATTRIBUTES) NULL,
                                    0,
                                    (LPTHREAD_START_ROUTINE) CommWatchProc,
                                    (LPVOID) this,
                                    0,
                                    &dwThreadID);

        if (hWatchThread == NULL)
        {
            // CreateThread failed
            Connected = FALSE;
            CloseHandle(hComm);
            DBG("Cannot create thread");
        }
        else
        {
            // assert DTR
            EscapeCommFunction(hComm, SETDTR);
        }
    }
    else
    {
        Connected = FALSE;
        CloseHandle(hComm);
    }

    if (!Connected)
    {
        hComm = INVALID_HANDLE_VALUE;
        DBG("Cannot open COM%d (%d Baud)\n", PortNr, Baudrate);
    }
}

//------------------------------------------------------------------------------
//
//  int WriteBlock(unsigned char *Block, int nBytesToWrite)
//
//  Description:
//     Writes a block of data to the COMM port
//
//  Parameters:
//     unsigned char *Block
//        pointer to data to write to port
//
//------------------------------------------------------------------------------

int Comm::WriteBlock(unsigned char *Block, int nBytesToWrite)
{
    DWORD dwWritten;
    BOOL bRes;

    if (!Connected)
        return FALSE;

    OVERLAPPED *osWrite = (OVERLAPPED *) WriteEvent;

    // issue write
    if (!WriteFile(hComm, Block, nBytesToWrite, &dwWritten, osWrite))
    {
        if (GetLastError() != ERROR_IO_PENDING)
        {
            // WriteFile failed, but it isn't delayed. Report error and abort.
            bRes = FALSE;
        }
        else
        {
            // write is pending
            if (!GetOverlappedResult(hComm, osWrite, &dwWritten, TRUE))
            {
                bRes = FALSE;
            }
            else
            {
                // write operation completed successfully
                bRes = TRUE;
            }
        }
    }
    else
    {
      // WriteFile completed immediately
      bRes = TRUE;
    }

   return bRes;

} // end of WriteBlock()

//------------------------------------------------------------------------------
//
//  DWORD CommWatchProc(LPSTR lpData)
//
//  Description:
//     A secondary thread that will watch for COMM events.
//
//  Parameters:
//     LPSTR lpData
//
//------------------------------------------------------------------------------

int Comm::WatchThread(void)
{
    BYTE  cIn[MAXBLOCK];
    DWORD dwEvtMask;
    DWORD dwRead;
    DWORD dwRes;
    BOOL  bWaitingOnRead = FALSE;

    if (!SetCommMask(hComm, EV_RXCHAR))
        return FALSE;

    OVERLAPPED *osRead = (OVERLAPPED *) ReadEvent;

    while (Connected)
    {
        if (!bWaitingOnRead)
        {
            dwEvtMask = 0;
            WaitCommEvent(hComm, &dwEvtMask, NULL);

            // issue read operation
            if (!ReadFile(hComm, cIn, MAXBLOCK, &dwRead, osRead))
            {
                // read not delayed?
                if (GetLastError() != ERROR_IO_PENDING)
                {
                    // error in communications; report it
                    DBG("Immediate Read Error");
                }
                else
                {
                    bWaitingOnRead = TRUE;
                }
            }
            else
            {
                // read completed immediately
                if (ReadCallback && m_ptr_yaw != nullptr && m_ptr_cal != nullptr) {
            
                    if (*m_ptr_cal) {
                        *(m_ptr_yaw) = ReadCallback(cIn, dwRead);
                        m_cal = *(m_ptr_yaw);
                        *m_ptr_yaw = 0.0f;
                        *m_ptr_cal = false;
                    }
                    else {
                        *(m_ptr_yaw) = ReadCallback(cIn, dwRead) - m_cal;
                    }

                }
            }
        }

        if (bWaitingOnRead)
        {
            dwRes = WaitForSingleObject(osRead->hEvent, READ_TIMEOUT);
            switch(dwRes)
            {
                // read completed
                case WAIT_OBJECT_0:
                    if (!GetOverlappedResult(hComm, osRead, &dwRead, FALSE))
                    {
                        // error in communications; report it
                        DBG("Delayed Read Error");
                    }
                    else
                    {   // delayed read completed successfully
                        if (ReadCallback)
                            ReadCallback(cIn, dwRead);
                    }
                    //  reset flag so that another opertion can be issued
                    bWaitingOnRead = FALSE;
                    break;

                case WAIT_TIMEOUT:
                    // Operation isn't complete yet. bWaitingOnRead flag isn't
                    // changed since I'll loop back around, and I don't want
                    // to issue another read until the first one finishes.
                    break;

                default:
                    // Error in the WaitForSingleObject; abort.
                    // This indicates a problem with the OVERLAPPED structure's
                    // event handle.
                    DBG("Error in the WaitForSingleObject");
                    break;
            }
        }
    }

    // clear information in structure (kind of a "we're done flag")
    dwThreadID = 0;

    return TRUE;
}

//------------------------------------------------------------------------------

DWORD CommWatchProc(LPSTR lpData)
{
    Comm *comm = (Comm *) lpData;

    return comm->WatchThread();

} // end of CommWatchProc()

//------------------------------------------------------------------------------

Comm::~Comm(void)
{
    unsetPointer();

    // set connected flag to FALSE
    Connected = FALSE;

    // disable event notification and wait for thread to halt
    SetCommMask(hComm, 0);

    // block until thread has been halted
    while (dwThreadID != 0)
        Sleep(100);

    // drop DTR
    EscapeCommFunction(hComm, CLRDTR);

    // purge any outstanding reads/writes and close device handle
    PurgeComm(hComm, PURGE_TXABORT | PURGE_RXABORT |
                     PURGE_TXCLEAR | PURGE_RXCLEAR);

    CloseHandle(hComm);

    OVERLAPPED *osRead = (OVERLAPPED *) ReadEvent;
    CloseHandle(osRead->hEvent);

    OVERLAPPED *osWrite = (OVERLAPPED *) WriteEvent;
    CloseHandle(osWrite->hEvent);

    delete ReadEvent;
    delete WriteEvent;
}

//------------------------------------------------------------------------------

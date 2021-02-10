#include <stdio.h>

#include "Comm.h"

#include "audio_processing_float.h"

int PortTracker = 16;

void head_tracker(byte *block, int nbytes);

const int numBytes = 40;
static byte byteStream[numBytes];
static uint32_t headTrackerKey;
static int count = numBytes;

static float yaw, pitch, roll, diffYaw, diffPitch, diffRoll;

static int calib = 0;

static Comm *comm = NULL;

//------------------------------------------------------------------------------

float map180(float angle)
{
    while (angle < -180)
        angle = angle + 360;

    while (angle > +180)
        angle = angle - 360;

    return angle;
}

//------------------------------------------------------------------------------

void init_audio_processing_float(void)
{
    if (PortTracker != -1)
    {
        comm = new Comm(PortTracker, 115200);
        if (comm->IsOpen())
            comm->SetReadCallback(head_tracker);
    }
}

//------------------------------------------------------------------------------

void head_tracker(byte *input, int ninput)
{
    for (int n = 0; n < ninput; n++)
    {
        if (count >= numBytes)
        {
            headTrackerKey = (headTrackerKey << 8) | input[n];

            if (headTrackerKey == 0x303120ff)
            {
                count = 0;
                headTrackerKey = 0;
            }
        }
        else
        {
            byteStream[count++] = input[n];

            if (count == numBytes)
            {
                float x, y, z;

                byte *temp = (byte*)&x;
                temp[0] = byteStream[11];
                temp[1] = byteStream[12];
                temp[2] = byteStream[13];
                temp[3] = byteStream[14];

                temp = (byte*)&y;
                temp[0] = byteStream[15];
                temp[1] = byteStream[16];
                temp[2] = byteStream[17];
                temp[3] = byteStream[18];

                temp = (byte*)&z;
                temp[0] = byteStream[19];
                temp[1] = byteStream[20];
                temp[2] = byteStream[21];
                temp[3] = byteStream[22];

                yaw = x;
                pitch = y;
                roll = z;

                printf("x = %g, y = %g, z = %g\n", yaw, pitch, roll);
            }
        }
    }
}

//------------------------------------------------------------------------------

void audio_processing_float( void )
{

    // Calibration
    if (calib)
    {
        diffYaw = yaw;
        diffPitch = pitch;
        diffRoll = roll;
    }

    float cyaw = yaw - diffYaw;
    float cpitch = pitch - diffPitch;
    float croll = roll - diffRoll;

    cyaw = map180(cyaw);
    cpitch = map180(cpitch);
    croll = map180(croll);

}

//------------------------------------------------------------------------------

void delete_audio_processing_float(void)
{
    delete comm;
    comm = NULL;
}

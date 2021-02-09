#include "HeadTracking.h"

std::vector<float> m_directions;
const int numBytes = 40;
static uint32_t headTrackerKey;
static byte byteStream[numBytes];
int count = numBytes;
static float yaw, pitch, roll, diffYaw, diffPitch, diffRoll;	
static float* ptr_directions;

int PortTracker = 3;

const static int calib = 0;





HeadTracking::HeadTracking() {
   
}

HeadTracking::~HeadTracking() {}

void HeadTracking::setPointer(float* ptr_yaw, bool* ptr_cal) {
    comm->setPointer(ptr_yaw, ptr_cal);
}

void HeadTracking::unsetPointer() {
    comm->unsetPointer();
}

float HeadTracking::map180(float angle)
{
    while (angle < -180)
        angle = angle + 360;

    while (angle > +180)
        angle = angle - 360;

    return angle;
}

//------------------------------------------------------------------------------

void HeadTracking::init_audio_processing_float(void)
{

    if (PortTracker != -1)
    {
        comm = new Comm(PortTracker, 115200);

        std::string mess = "Comm open: ";
        mess.append(std::to_string(comm->IsOpen()));

        if (comm->IsOpen())
            comm->SetReadCallback(head_tracker);
    }

}

//------------------------------------------------------------------------------

float head_tracker(byte* input, int ninput)
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

                byte* temp = (byte*)&x;
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

            }
        }
    }
    return yaw;
}




//------------------------------------------------------------------------------

void HeadTracking::audio_processing_float(void)
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

void HeadTracking::delete_audio_processing_float(void)
{
    delete comm;
    comm = NULL;
}


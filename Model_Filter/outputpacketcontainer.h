#ifndef outputpacketcontainer_h
#define outputpacketcontainer_h

#include "outputpacket.h"

struct OutputPacketContainer
{
   OutputPacket packets[2];
   OutputPacket* readPacketPointer;
   OutputPacket* writePacketPointer;
   volatile unsigned canRead;
   volatile unsigned canWrite;
};

void ocInit(OutputPacketContainer* oc);
void ocDestroy(OutputPacketContainer* oc);
OutputPacket* ocStartReadPacket(OutputPacketContainer* oc);
void ocFinishReadPacket(OutputPacketContainer* oc);
OutputPacket* ocStartWritePacket(OutputPacketContainer* oc);
void ocFinishWritePacket(OutputPacketContainer* oc);

#endif

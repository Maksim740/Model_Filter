#ifndef inputpacketcontainer_h
#define inputpacketcontainer_h

#include "inputpacket.h"

struct InputPacketContainer
{
   InputPacket packets[2];
   InputPacket* readPacketPointer;
   InputPacket* writePacketPointer;
	volatile unsigned canRead; 
	volatile unsigned canWrite;
};

void icInit(InputPacketContainer* ic);
void icDestroy(InputPacketContainer* ic);
InputPacket* icStartReadPacket(InputPacketContainer* ic);
void icFinishReadPacket(InputPacketContainer* ic);
InputPacket* icStartWritePacket(InputPacketContainer* ic);
void icFinishWritePacket(InputPacketContainer* ic);

#endif

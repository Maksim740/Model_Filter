#ifndef outputpacket_h
#define outputpacket_h

struct OutputPacketItem
{
	unsigned level;
};

#define OUTPUTPACKET_MAXCOUNT 100

struct OutputPacket
{
	unsigned count;
	OutputPacketItem data[OUTPUTPACKET_MAXCOUNT];
};

#endif

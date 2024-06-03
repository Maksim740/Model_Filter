#include "consumerapp.h"
#include "outputpacket.h"
#include <unistd.h>
#include <cstdlib>
#include <iostream>
using namespace std;

static void consumerAppOutPacket(ConsumerApp& app, OutputPacket& packet);
static bool consumerAppReceivePacket(ConsumerApp& app, OutputPacket& packet);

int consumerAppRun(ConsumerApp& app) {

	OutputPacket packet;
	while ( 1 ) {
		bool needContinue = consumerAppReceivePacket(app,packet);
		if ( !needContinue )
			break;

		consumerAppOutPacket(app,packet);
	}

	return 0;
}

static void consumerAppOutPacket(ConsumerApp& /*app*/, OutputPacket& packet) {

	cout  << "count=" << packet.count << endl;
		cout << "items= ";
		for(unsigned i = 0; i<packet.count; ++i)
			cout << "(" << packet.data[i].level << ") ";
		cout << endl;
		return;
}

static bool consumerAppReceivePacket(ConsumerApp& app, OutputPacket& packet)
{
	int ret = read(app.readFd,&packet.count,sizeof(packet.count));
	if ( ret == 0 || ret == -1 )
		return false;
	if ( packet.count == 0 )
		return true;
	ret = read(app.readFd,packet.data,packet.count*sizeof(OutputPacketItem));
	if ( ret == 0 || ret == -1 )
		return false;
	return true;
}

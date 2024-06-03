#include "procapp.h"
#include "inputpacket.h"
#include "outputpacket.h"

#include <unistd.h>
#include <iostream>
using namespace std;

#include "inputpacketcontainer.h"
#include "outputpacketcontainer.h"


static bool procAppReceivePacket(ProcApp& app, InputPacket& packet);
static void procAppProcessing(ProcApp& app, InputPacket& input, OutputPacket& output);
static void procAppSendPacket(ProcApp& app, OutputPacket& packet);

static void processingImpl(const InputPacket& input, OutputPacket& output, const ProcConfig& config);


//функции, исполняемые потоками
static void* processingExecute(void* arg);
static void* inputExecute(void* arg);
static void* outputExecute(void*);

//структурные типы данных для передачи параметров в функцию потока
struct InputThreadData {
   ProcApp* app;
   InputPacketContainer* ic;
};

struct OutputThreadData {
   ProcApp* app;
   OutputPacketContainer* oc;
};

struct ProcessingThreadData {
   ProcApp* app;
   InputPacketContainer* ic;
   OutputPacketContainer* oc;
};


int procAppRun(ProcApp& app)
{


	//создание буферов совместного доступа потоками
	   InputPacketContainer ic;
	   OutputPacketContainer oc;
	   icInit(&ic);
	   ocInit(&oc);

	   //заполнение структур-параметров функций потоков
	      InputThreadData itData;
	      itData.app = &app;
	      itData.ic = &ic;
	      ProcessingThreadData ptData;
	      ptData.app = &app;
	      ptData.ic = &ic;
	      ptData.oc = &oc;
	      OutputThreadData otData;
	      otData.app = &app;
	      otData.oc = &oc;

	//объявление дескрипторов потоков
	   pthread_t inputThread;
	   pthread_t processingThread;
	   pthread_t outputThread;

	   //порождение потоков управления
	   pthread_create(&inputThread,NULL,inputExecute,&itData);
	   pthread_create(&processingThread,NULL,processingExecute,&ptData);
	   pthread_create(&outputThread,NULL,outputExecute,&otData);


	 //ожидание завершения потоков
	   pthread_join(inputThread,0);	
	   pthread_join(processingThread,0);
	   pthread_join(outputThread,0);

	   //освобождение ресурсов буферов
	      ocDestroy(&oc);
	      icDestroy(&ic);
	   return 0;
}

static bool procAppReceivePacket(ProcApp& app, InputPacket& packet)
{//прием количества отсчетов
	int ret = read(app.readFd,&packet.count,sizeof(packet.count));
		if ( ret == 0 || ret == -1 )
			return false;
		if ( packet.count == 0 )
			return true;

		//прием набоа отсчетов
		ret = read(app.readFd,packet.data,packet.count*sizeof(InputPacketItem));
		if ( ret == 0 || ret == -1 )
			return false;

		return true;
	}


static void procAppProcessing(ProcApp& app, InputPacket& input, OutputPacket& output)
{
	  struct timespec procTime;
	   clock_gettime(CLOCK_MONOTONIC,&procTime);
	   procTime.tv_nsec += app.procDelay*1000000;
	   if ( procTime.tv_nsec >= 1000000000 ) {
	        ++procTime.tv_sec;
	        procTime.tv_nsec -= 1000000000;
	   }
	   processingImpl(input,output,app.procConfig);

	      clock_nanosleep(CLOCK_MONOTONIC,TIMER_ABSTIME,&procTime,NULL);
	      return;
	   }
static void procAppSendPacket(ProcApp& app, OutputPacket& packet)
{
	write(app.consumerFd,&packet.count,sizeof(packet.count));
	write(app.consumerFd,packet.data,packet.count*sizeof(OutputPacketItem));
	return;
}

static void processingImpl(const InputPacket& input, OutputPacket& output, const ProcConfig& config)
{
	output.count=0;
	for (unsigned i=0; i<input.count; i++)
	{
	output.data[i].level=0;
	output.count++;
	for (unsigned j=0; j<3; j++){
	if(i>=j)
		output.data[i].level+= config.coeff[j]*input.data[i-j].level;
	}
	}

	return;
}

static void* processingExecute(void* arg) {

	   ProcessingThreadData* params = (ProcessingThreadData*)(arg);
	   InputPacketContainer* ic = params->ic;
	   OutputPacketContainer* oc = params->oc;
	   while ( 1 ) {
	      InputPacket* input = icStartReadPacket(ic);
	      OutputPacket* output = ocStartWritePacket(oc);
	      procAppProcessing(*params->app,*input,*output);
	      icFinishReadPacket(ic);
	      ocFinishWritePacket(oc);
	   }
	   return 0;
	}

static void* inputExecute(void* arg) {

	 InputThreadData* params = (InputThreadData*)(arg);
	   InputPacketContainer* ic = params->ic;
	   while ( 1 ) {
	      InputPacket* input = icStartWritePacket(ic);
	      if ( !procAppReceivePacket(*params->app,*input) )
	         break;
	      icFinishWritePacket(ic);
	   }
	   return 0;
	}

static void* outputExecute(void* arg) {

	   OutputThreadData* params = (OutputThreadData*)(arg);
	   OutputPacketContainer* oc = params->oc;
	   while ( 1 ) {
	      OutputPacket* output = ocStartReadPacket(oc);
	      procAppSendPacket(*params->app,*output);
	      ocFinishReadPacket(oc);
	   }
	   return 0;
	}

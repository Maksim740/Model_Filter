#include "inputpacketcontainer.h"

void icInit(InputPacketContainer* ic) {

	ic->canRead = 0;
	ic->canWrite = 2;
	ic->readPacketPointer = ic->packets;
   ic->writePacketPointer = ic->packets;
	return;
}

void icDestroy(InputPacketContainer* /*ic*/) {

}

InputPacket* icStartReadPacket(InputPacketContainer* ic) {

   //ожидаем готовности пакета
   while ( !ic->canRead )
      continue;
   //блокируем область пакета
   ic->canRead--;

   InputPacket* ret = ic->readPacketPointer;
   //переключить буфер
   if ( ic->readPacketPointer == ic->packets )
      ic->readPacketPointer = ic->packets + 1;
   else
      ic->readPacketPointer = ic->packets;

   return ret;
}

void icFinishReadPacket(InputPacketContainer* ic) {

   //область памяти стала доступной для записи очередного пакета
   ic->canWrite++;
   return;
}

InputPacket* icStartWritePacket(InputPacketContainer* ic) {

   //ожидаем свободной для записи области памяти
   while ( !ic->canWrite )
      continue;
   //блокируем область пакета
   ic->canWrite--;

   InputPacket* ret = ic->writePacketPointer;
   //переключить буфер
   if ( ic->writePacketPointer == ic->packets )
      ic->writePacketPointer = ic->packets + 1;
   else
      ic->writePacketPointer = ic->packets;

   return ret;
}

void icFinishWritePacket(InputPacketContainer* ic) {

   //пакет готов
   ic->canRead++;
   return;
}

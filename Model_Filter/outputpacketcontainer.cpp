#include "outputpacketcontainer.h"

void ocInit(OutputPacketContainer* oc) {

   oc->canRead = 0;
   oc->canWrite = 2;
   oc->readPacketPointer = oc->packets;
   oc->writePacketPointer = oc->packets;
   return;
}

void ocDestroy(OutputPacketContainer* /*oc*/) {

}

OutputPacket* ocStartReadPacket(OutputPacketContainer* oc) {

   //ожидаем готовности пакета
   while ( !oc->canRead )
      continue;
   //блокируем область пакета
   oc->canRead--;

   OutputPacket* ret = oc->readPacketPointer;
   //переключить буфер
   if ( oc->readPacketPointer == oc->packets )
      oc->readPacketPointer = oc->packets + 1;
   else
      oc->readPacketPointer = oc->packets;

   return ret;
}

void ocFinishReadPacket(OutputPacketContainer* oc) {

   //область памяти стала доступной для записи очередного пакета
   oc->canWrite++;
   return;
}

OutputPacket* ocStartWritePacket(OutputPacketContainer* oc) {

   //ожидаем свободной для записи области памяти
   while ( !oc->canWrite )
      continue;
   //блокируем область пакета
   oc->canWrite--;

   OutputPacket* ret = oc->writePacketPointer;
   //переключить буфер
   if ( oc->writePacketPointer == oc->packets )
      oc->writePacketPointer = oc->packets + 1;
   else
      oc->writePacketPointer = oc->packets;

   return ret;
}

void ocFinishWritePacket(OutputPacketContainer* oc) {

   //пакет готов
   oc->canRead++;
   return;
}

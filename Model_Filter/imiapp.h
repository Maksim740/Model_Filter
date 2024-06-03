#ifndef imiapp_h
#define imiapp_h

#include <time.h>

struct ImiApp {
   int writeFd;

   struct timespec actTime; //время генерации пакета
   	bool fromUser; //признак формирования данных от пользователя
   	int packetCount; //оставшееся колочиство имитируемых пакетов
      unsigned packetSize; //размер пакета (в отсчетах)
      unsigned maxLevel; //максимальное значенние отсчетов

};

int imiAppRun(ImiApp& app);

#endif


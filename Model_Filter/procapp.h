#ifndef procapp_h
#define procapp_h

struct ProcConfig
{
   unsigned coeff[3];
};

struct ProcApp
{
    ProcConfig procConfig;
    int readFd;
    int consumerFd;
    unsigned procDelay; //задержка для имитации обработки указанной длительности
};

int procAppRun(ProcApp& app);

#endif

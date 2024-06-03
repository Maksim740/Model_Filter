//============================================================================
#include "imiapp.h"
#include "procapp.h"
#include "consumerapp.h"
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
using namespace std;
struct ControlParameters
{
	bool fromUser; 
	unsigned packetCount; 
	unsigned packetSize; 
	unsigned maxLevel; 
	unsigned procCoeff[3];  
	unsigned procDelay; //длительность времени обработки в миллисекундах (не менее)
};
bool optionsParse(int argc, char** argv, ControlParameters& params);

int main(int argc, char** argv) 
{
	ControlParameters params;
		params.fromUser = true;
		params.packetCount = 2;
		params.packetSize = 4;
		params.maxLevel = 1024;
		params.procCoeff[0]=2;
		params.procCoeff[1]=3;
		params.procCoeff[2]=4;
		params.procDelay = 0;
		if ( !optionsParse(argc, argv, params) )
			return 5;

	int pipeFd[2];	
		if ( pipe(pipeFd) == -1 ) { 
			perror("pipe");
			return 2;
		}
		int pipeConsumer[2];
			if ( pipe(pipeConsumer) == -1 ) {
				perror("pipe");
				return 3;
			}

		//Порождение процесса обработчика
	pid_t pid = fork(); 
	if ( pid < 0 ) {
		perror("fork");
		return 1;
	}
	if ( pid == 0 ) {
		close(pipeFd[1]);	
		close(pipeConsumer[0]);

		ProcApp app;
		app.procConfig.coeff[0] = params.procCoeff[0];
		app.procConfig.coeff[1] = params.procCoeff[1];
		app.procConfig.coeff[2] = params.procCoeff[2];
			app.readFd = pipeFd[0];
			app.consumerFd = pipeConsumer[1];
			app.procDelay = params.procDelay;
			int ret = procAppRun(app);
			close(app.readFd);
			close(app.consumerFd);
			return ret;
	}

	//Порождение процесса имитатора
		pid_t imiPid = fork();
		if ( imiPid < 0 ) {
			perror("fork");
			return 1;
		}
		if ( imiPid == 0 ) {
			close(pipeFd[0]);
			close(pipeConsumer[0]);
					close(pipeConsumer[1]);

			ImiApp app;
			app.writeFd = pipeFd[1];
			app.fromUser = params.fromUser;
			app.packetCount = params.packetCount;
			app.packetSize = params.packetSize;
			app.maxLevel = params.maxLevel;

			int ret = imiAppRun(app);
			close(app.writeFd);
			return ret;
		}

		//Порождение процесса потребителя
			pid_t consumerPid = fork();
			if ( consumerPid < 0 ) {
				perror("fork");
				return 1;
			}
			if ( consumerPid == 0 ) {
				close(pipeFd[0]);
				close(pipeFd[1]);
				close(pipeConsumer[1]);

				ConsumerApp app;
				app.readFd = pipeConsumer[0];
				int ret = consumerAppRun(app);
				close(app.readFd);
				return ret;
			}

	close(pipeFd[0]);
		close(pipeFd[1]);
		close(pipeConsumer[0]);
			close(pipeConsumer[1]);

		//ожидание завершения всех потомков
			for( unsigned i = 0; i < 3; ++i) {
				int status;
				while ( wait(&status) == -1 )
					continue;
			}
			
	return 0;
}

bool optionsParse(int argc, char **argv, ControlParameters& params)
{
	while ( 1 ) {
		int c = getopt (argc, argv, "ic:s:l:k:u:h:t:");

		if ( c == -1 )
			break;
		switch (c) {
			case 'i':
				params.fromUser = false;
				break;
			case 'c':
				params.packetCount = atoi(optarg); 
				break;
			case 's':
				params.packetSize = atoi(optarg);
				break;
			case 'l':
				params.maxLevel = atoi(optarg);
				break;
			case 'k':
				params.procCoeff[0] = atoi(optarg);
				break;
			case 'u':
				params.procCoeff[1] = atoi(optarg);
				break;
			case 'h':
				params.procCoeff[2] = atoi(optarg);
				break;
			case 't':
				params.procDelay = atoi(optarg);
				break;
			default:
				return false;
      }
	}
	return true;
}

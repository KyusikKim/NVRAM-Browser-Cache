#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/user.h>
#include <signal.h>

int seg_id;
double* shdmem;

static void sig_int(int signo)
{
	shmdt(shdmem);
	shmctl(seg_id, IPC_RMID, NULL);
	exit(1);
}

void main()
{
	int len, i;
	char args[1024];
	int size = PAGE_SIZE;
	double tmpValue = 0.0;
	double busyValue = 0.0;
	char* token;
	char* endptr;

	if (signal(SIGINT, sig_int) == SIG_ERR)
		fprintf(stderr, "signal error");

	seg_id = shmget(1234, size, IPC_CREAT | S_IRUSR | S_IWUSR);

	memset(args, 0x00, sizeof(args));
	read(STDIN_FILENO, args, sizeof(args));	//cut out of the kernel info's header

	while (1) {
		retry:
		memset(args, 0x00, sizeof(args));
		read(STDIN_FILENO, args, sizeof(args));	//include busy value
		if(strncmp(args, "avg-cpu:", 8)) {
			sleep(1);
			continue;
		}
		token = strtok(args,"\n");
//		printf("[line(1)]=%s\n", token);		//dbg msg
		for(i=0;i<3;i++){
			token = strtok(NULL,"\n");	
//			printf("[line(%d)]=%s\n",i+2, token);	//dbg msg
			if (!token) goto retry;	//Fixme: why occured?
		}
		
		token = strtok(token," \t\n");
		for(i=0;i<13;i++)
			token = strtok(NULL," \t\n");		

		if (!token)	//Fixme: why occured?
			printf("[(NULL token)txfer: %.2lf]\n", busyValue);
		else {
			tmpValue = strtod(token, &endptr);
			if (tmpValue <= 100 && tmpValue >= 0)
				busyValue = tmpValue;
			printf("[real: %.2lf]\t", tmpValue);
			printf("[txfer: %.2lf]\n", busyValue);
		}		

		shdmem = (double*)shmat(seg_id, NULL, 0);
		*shdmem = busyValue;
//		*shdmem = 0.00;
		shmdt(shdmem);
	}
}



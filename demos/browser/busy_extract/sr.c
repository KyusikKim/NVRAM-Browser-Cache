#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/user.h>
#include <signal.h>

int seg_sr;				//0:normal mode, 1:cache mode, 2:network mode  //**persist
int seg_flag;			//A flag of the "SR update time" is reached (default: 10s) //value is 0 or 1
int seg_readyUrl;		//target URL for that update SR 
int seg_resourceCnt;	//non-evaluated resources  //**persist
int seg_cacheTime;		//Load time of resource in cache for comparison with network's
int seg_cacheHitCnt;
int seg_totalEstCnt;
int seg_selecNetCnt;

int* shdsr;
int* shdflag;
char* shdurl;
long long int* shdresCnt;
long long int* shdcacheTime;
long long int* shdcacheHitCnt;
long long int* shdtotalEstCnt;
long long int* shdselecNetCnt;

static void sig_int(int signo)
{
	shmdt(shdsr);
	shmctl(seg_sr, IPC_RMID, NULL);
	shmdt(shdflag);
	shmctl(seg_flag, IPC_RMID, NULL);
	shmdt(shdurl);
	shmctl(seg_readyUrl, IPC_RMID, NULL);
	shmdt(shdresCnt);
	shmctl(seg_resourceCnt, IPC_RMID, NULL);
	shmdt(shdcacheTime);
	shmctl(seg_cacheTime, IPC_RMID, NULL);
	shmdt(shdcacheHitCnt);
	shmctl(seg_cacheHitCnt, IPC_RMID, NULL);
	shmdt(shdtotalEstCnt);
	shmctl(seg_totalEstCnt, IPC_RMID, NULL);
	shmdt(shdselecNetCnt);
	shmctl(seg_selecNetCnt, IPC_RMID, NULL);
	fprintf(stdout, "ipcs are destroyed.\n");
	exit(1);
}

void main()
{
	int tmp;
	int len, i;
	int size = PAGE_SIZE;
	int sr = 0;
	int abort = 0;
	const int max_abort = 5;

	if (signal(SIGINT, sig_int) == SIG_ERR)
		fprintf(stderr, "signal error");

	seg_sr = shmget(1234, size, IPC_CREAT | S_IRUSR | S_IWUSR);
	seg_flag = shmget(1111, size, IPC_CREAT | S_IRUSR | S_IWUSR);
	seg_readyUrl = shmget(2222, (sizeof(char)*4096), IPC_CREAT | S_IRUSR | S_IWUSR);
	seg_resourceCnt = shmget(3333, size, IPC_CREAT | S_IRUSR | S_IWUSR);
	seg_cacheTime = shmget(4444, size, IPC_CREAT | S_IRUSR | S_IWUSR);
	seg_cacheHitCnt = shmget(5555, size, IPC_CREAT | S_IRUSR | S_IWUSR);
	seg_totalEstCnt = shmget(6666, size, IPC_CREAT | S_IRUSR | S_IWUSR);
	seg_selecNetCnt = shmget(7777, size, IPC_CREAT | S_IRUSR | S_IWUSR);

	shdsr = (int*)shmat(seg_sr, NULL, 0);
	*shdsr = 0;		//init scheme	
	shmdt(shdsr);

	shdflag = (int*)shmat(seg_flag, NULL, 0);
	*shdflag = 1;
	shmdt(shdflag);

	/* Counter variables */
	shdresCnt = (long long int*)shmat(seg_resourceCnt, NULL, 0);
	*shdresCnt = 0;
	shmdt(shdresCnt);
	shdcacheHitCnt = (long long int*)shmat(seg_cacheHitCnt, NULL, 0);
	*shdcacheHitCnt = 0;
	shmdt(shdcacheHitCnt);
	shdtotalEstCnt = (long long int*)shmat(seg_totalEstCnt, NULL, 0);
	*shdtotalEstCnt = 0;
	shmdt(shdtotalEstCnt);
	shdselecNetCnt = (long long int*)shmat(seg_selecNetCnt, NULL, 0);
	*shdselecNetCnt = 0;
	shmdt(shdselecNetCnt);

	while (1) {
			shdflag = (int*)shmat(seg_flag, NULL, 0);

		for (abort = 0;*shdflag != 0;) {
			shmdt(shdflag);
			sleep(2);
			shdflag = (int*)shmat(seg_flag, NULL, 0);
			shdsr = (int*)shmat(seg_sr, NULL, 0);
			shdresCnt = (long long int*)shmat(seg_resourceCnt, NULL, 0);
			shdurl = (char*)shmat(seg_readyUrl, NULL, 0);
			shdtotalEstCnt = (long long int*)shmat(seg_totalEstCnt, NULL, 0);
			shdselecNetCnt = (long long int*)shmat(seg_selecNetCnt, NULL, 0);

			printf("[Available Estimation]SR:%d, Flag:%d, URL:%s, ResCnt:%lld, EstCnt:%lld, ToNetCnt:%lld \n", 
					*shdsr,
					*shdflag,
					shdurl,
					*shdresCnt,
					*shdtotalEstCnt,
					*shdselecNetCnt);
				
			tmp = *shdflag;	
			shmdt(shdsr);
			shmdt(shdresCnt);
			shmdt(shdurl);
			shmdt(shdtotalEstCnt);
			shmdt(shdselecNetCnt);
			if ((++abort > max_abort) && (tmp == 1)) break;
		}
		shmdt(shdflag);

		/*test message*/
		shdsr = (int*)shmat(seg_sr, NULL, 0);
		shdflag = (int*)shmat(seg_flag, NULL, 0);
		shdurl = (char*)shmat(seg_readyUrl, NULL, 0);
		shdcacheTime = (long long int*)shmat(seg_cacheTime, NULL, 0);
		shdresCnt = (long long int*)shmat(seg_resourceCnt, NULL, 0);
		shdtotalEstCnt = (long long int*)shmat(seg_totalEstCnt, NULL, 0);
		shdselecNetCnt = (long long int*)shmat(seg_selecNetCnt, NULL, 0);

		printf("[Maintain policy(10s) & Init]SR:%d, Flag:%d, URL:%s, ResCnt:%lld, EstCnt:%lld, ToNetCnt:%lld \n", 
					*shdsr,
					*shdflag,
					shdurl,
					*shdresCnt,
					*shdtotalEstCnt,
					*shdselecNetCnt);

		shmdt(shdsr);
		shmdt(shdresCnt);
		/*end message*/

		*shdflag = 0;	//start estimation
		memset(shdurl,0x00,(sizeof(char)*4096));
		*shdcacheTime = 0;
		shmdt(shdflag);
		shmdt(shdurl);
		shmdt(shdcacheTime);
		shmdt(shdtotalEstCnt);
		shmdt(shdselecNetCnt);

		sleep(5);		

		shdflag = (int*)shmat(seg_flag, NULL, 0);
		*shdflag = 1;	//start estimation			
		shmdt(shdflag);
	}
}



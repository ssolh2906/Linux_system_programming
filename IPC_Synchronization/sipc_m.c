// Inter Thread communication
// Using mutex and condition variable
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>

#define     MAX_STR     100

// global var instead of shared memory
char            msg[MAX_STR];

// condition and mutex
pthread_cond_t  Request;
pthread_cond_t  Reply;
pthread_mutex_t Mutex;


void
ThreadUsleep(int usecs)
{
	pthread_cond_t		cond;
	pthread_mutex_t		mutex;
	struct timespec		ts;
	struct timeval		tv;

	if (pthread_cond_init(&cond, NULL) < 0)  {
		perror("pthread_cond_init");
		pthread_exit(NULL);
	}
	if (pthread_mutex_init(&mutex, NULL) < 0)  {
		perror("pthread_mutex_init");
		pthread_exit(NULL);
	}

	gettimeofday(&tv, NULL);
	ts.tv_sec = tv.tv_sec + usecs/1000000;
	ts.tv_nsec = (tv.tv_usec + (usecs%1000000)) * 1000;
	if (ts.tv_nsec >= 1000000000)  {
		ts.tv_nsec -= 1000000000;
		ts.tv_sec++;
	}

	if (pthread_mutex_lock(&mutex) < 0)  {
		perror("pthread_mutex_lock");
		pthread_exit(NULL);
	}
	if (pthread_cond_timedwait(&cond, &mutex, &ts) < 0)  {
		perror("pthread_cond_timedwait");
		pthread_exit(NULL);
	}

	if (pthread_cond_destroy(&cond) < 0)  {
		perror("pthread_cond_destroy");
		pthread_exit(NULL);
	}
	if (pthread_mutex_destroy(&mutex) < 0)  {
		perror("pthread_mutex_destroy");
		pthread_exit(NULL);
	}
}

void 
thread1(void *dummy)
// sipc1 의 역할을 하는 Thread
// request 받으면 출력하고, reply 보냄
{
    printf("Thread1: Start.....\n");

    if (pthread_mutex_lock(&Mutex) < 0 )
    {
        perror("pthread_mutex_lock");
        exit(1);
    }
    if (msg[0] == NULL)
    // 들어온 request 없는 상태
    // wait 하고 mutex풀어줌
    {
        if (pthread_cond_wait(&Request, &Mutex) < 0) 
        {
            perror("pthread_cond_wait");
            exit(1);
        }        
    }

    //request 받았을 시
    printf("Thread1 got an request.....\n");
    printf("Request from thread2 : %s\n", msg);

    // write reply on glabal var
    sprintf(msg, "Reply from thread1\n");
    
    // signal 주고 mutex 언락
    if (pthread_cond_signal(&Reply) < 0)
    {
        perror("pthread_cond_signal");
        exit(1);
    }
    if (pthread_mutex_unlock(&Mutex) < 0)
    {
        perror("pthread_mutex_unlock");
        exit(1);
    }

    ThreadUsleep(3000);
    
    pthread_exit(NULL);
}

void
thread2(void *dummy) 
// sipc2 의 역할을 하는 Thread
// request 를 보내고, reply 를 받아 출력함
{
    char    *replyMsg[MAX_STR];

    printf("Thread2: Start.....\n");

    // critical section 진입
    if (pthread_mutex_lock(&Mutex) < 0 )
    {
        perror("pthread_mutex_lock");
        exit(1);
    }

    // request 작성
    sprintf(msg, "This is request from Thread2....\n");

    // signal 주고 mutex 해제
    if (pthread_cond_signal(&Request) < 0)
    {
        perror("pthread_cond_signal");
        exit(1);
    }

    if (pthread_mutex_unlock(&Mutex) < 0)
    {
        perror("pthread_mutex_unlock");
        exit(1);
    }

    printf("Sent a request...\n");

    // reply 기다림
    if (pthread_cond_wait(&Reply, &Mutex) < 0)
    {
        perror("pthread_cond_wait");
        exit(1);
    }

    // reply 받았을 시
    printf("Received reply from Thread1 : %s\n", msg);
    

    ThreadUsleep(3000);


    
    
    pthread_exit(NULL);
}



int main()
{
	pthread_t	tid1, tid2;
    msg[0] = NULL;

    if (pthread_cond_init(&Request, NULL) < 0)
    {
        perror("pthread_cond_init");
        exit(1);
    }
    if (pthread_cond_init(&Reply, NULL) < 0)
    {
        perror("pthread_cond_init");
        exit(1);
    }
    if (pthread_mutex_init(&Mutex, NULL) < 0)
    {
        perror("pthread_mutex_init");
        exit(1);
    }

    if (pthread_create(&tid1, NULL, (void *)thread1, (void *)NULL) < 0)
    {
        perror("pthread_create");
        exit(1);
    }
    if (pthread_create(&tid2, NULL, (void *) thread2, (void *)NULL) < 0)
    {
        perror("pthread_create");
        exit(1);
    }
    
    if (pthread_join(tid1, NULL) < 0)  {
		perror("pthread_join");
		exit(1);
	}
	if (pthread_join(tid2, NULL) < 0)  {
		perror("pthread_join");
		exit(1);
	}

    printf("Request and Reply Done succesfully....\n");
    printf("Destroy condition vars...\n");


	if (pthread_cond_destroy(&Request) < 0)  {
		perror("pthread_cond_destroy");
	}
	if (pthread_cond_destroy(&Reply) < 0)  {
		perror("pthread_cond_destroy");
	}
	if (pthread_mutex_destroy(&Mutex) < 0)  {
		perror("pthread_mutex_destroy");
	}



    return 0;
}

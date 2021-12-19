#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include "chat.h"

#define	DEBUG

#define	MAX_CLIENT	5
#define	MAX_ID		32
#define	MAX_BUF		256

typedef	struct  {
	int			sockfd;
	int			inUse;
	pthread_t	tid;
	char		uid[MAX_ID];
}
	ClientType;

int				Sockfd;

ClientType		Client[MAX_CLIENT];


int
GetID()
{
	int	i;

	for (i = 0 ; i < MAX_CLIENT ; i++)  {
		if (! Client[i].inUse)  {
			Client[i].inUse = 1;
			return i;
		}
	}
}

void
SendToOtherClients(int id, char* buf) 
{
    int     i;
    char    msg[MAX_BUF + MAX_ID];

    sprintf(msg, "%s> %s", Client[id].uid, buf);

    #ifdef DEBUG
        printf("%s", msg);
        fflush(stdout);
    #endif

	// send(write) message to all other users
    for (i = 0; i < MAX_CLIENT; i++)
    {
        if (Client[i].inUse && (i != id))
        {
            if (send(Client[i].sockfd, msg, strlen(msg) + 1, 0) < 0)
            {
                perror("send");
                exit(1);
            }
            
        }
        
    }
    

}

void
ProcessClient(int id)
{
    char buf[MAX_BUF];
    int n;

    // first received message : Log in ifo
    if ((n = recv(Client[id].sockfd, Client[id].uid, MAX_ID, 0)) < 0)
    {
        perror("recv");
        exit(1);
    }
	printf("Client %d log-in(ID: %s).....\n", id, Client[id].uid);
    

    // while logged-in
    while (1)
    {
        // wait for packet(msg from user)
		if ((n = recv(Client[id].sockfd, buf, MAX_BUF, 0)) < 0)  {
			perror("recv");
			exit(1);
		}
        // case : Log out
        if (n == 0)  {
			printf("Client %d log-out(ID: %s).....\n", id, Client[id].uid);

			close(Client[id].sockfd);
			Client[id].inUse = 0;

			strcpy(buf, "log-out.....\n");
			SendToOtherClients(id, buf);
            break;
		} else {
            // case : message
            SendToOtherClients(id, buf);
        }
    }
    

}

void
CloseServer(int signo)
{
    int i;
    close(Sockfd);

	printf("\nChat server terminated.....\n");

	exit(0);
}




int main(int argc, char const *argv[])
{
    int                         count,cliAddrLen,newSockfd,id, one = 1;
    fd_set                      fdvar;
    struct      sockaddr_in     cliAddr, servAddr;    

    // signal handler
    signal(SIGINT, CloseServer);


    // socket
    if ((Sockfd = socket(PF_INET, SOCK_STREAM,0)) < 0)
    {
        perror("socket");
        exit(1);
    }
    

    // set socket
    if (setsockopt(Sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) < 0)
    {
        perror("setsockopt");
        exit(1);
    }


    // 메모리할당하여 servAddr 구조체 생성
	bzero((char *)&servAddr, sizeof(servAddr));
	servAddr.sin_family = PF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(SERV_TCP_PORT);

    // bind
	if (bind(Sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)  {
		perror("bind");
		exit(1);
	}
    

    // listen
    listen(Sockfd, 5);

	printf("Chat server started.....\n");
    
	cliAddrLen = sizeof(cliAddr);
    // loop
    while (1)
    {
        // init FD_SET
		FD_ZERO(&fdvar);
		FD_SET(Sockfd, &fdvar);

        // select
        if ((count = select(10, &fdvar, 
        (fd_set *) NULL, (fd_set *) NULL,(struct timeval *) NULL )) < 0)
        {
			perror("select");
			exit(1);
        }
        

        // signal
        signal(SIGINT, CloseServer);

        // make connection
        while (count --)
        {
            // accept new socket
            newSockfd = accept(Sockfd, (struct sockaddr *) &cliAddr, &cliAddrLen);
            if (newSockfd < 0) 
            {
                perror("accept");
                exit(1);
            }
            

            // getID
            id = GetID();
            Client[id].sockfd = newSockfd;

            // processClient
            ProcessClient(id);
            
        }
        
    }
    


    return 0;
}

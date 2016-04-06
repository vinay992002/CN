#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stropts.h>
#include <string.h>
#include <fcntl.h>
#include <curses.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/select.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
int fds;
int l=0;
pthread_mutex_t mutex;
int recv_fd(
  int param) /* Socket from which the file descriptor is read */
{

 int socket = (param);

 int sent_fd;
 struct msghdr message;
 struct iovec iov[1];
 struct cmsghdr *control_message = NULL;
 char ctrl_buf[CMSG_SPACE(sizeof(int))];
 char data[1];
 int res;

 //printf("Waiting to receive fd %d\n",socket);
     memset(&message, 0, sizeof(struct msghdr));
     memset(ctrl_buf, 0, CMSG_SPACE(sizeof(int)));

     /* For the dummy data */
     iov[0].iov_base = data;
     iov[0].iov_len = sizeof(data);

     message.msg_name = NULL;
     message.msg_namelen = 0;
     message.msg_control = ctrl_buf;
     message.msg_controllen = CMSG_SPACE(sizeof(int));
     message.msg_iov = iov;
     message.msg_iovlen = 1;

     // if((res = recvmsg(socket, &message, 0)) <= 0)
         // printf("Does not receive\n");
    if(recvmsg(socket, &message, 0)<0){
      perror("recvmsg\n");
    }
  
     /* Iterate through header to find if there is a file descriptor */
     control_message = CMSG_FIRSTHDR(&message);
      if((control_message->cmsg_level == SOL_SOCKET) &&
          (control_message->cmsg_type == SCM_RIGHTS) )
      {
       int sd  =  *((int *) CMSG_DATA(control_message));
      // csfd[ccount++] = sd;
       
       //printf("Recieved\n");
      return sd;
      }
     
      return -1;
}
void *serv(void *num){
	int rsfd = socket(AF_INET,SOCK_RAW,250);
	if(rsfd<0){
		perror("socket error!!\n");
		exit(0);
	}	
	struct sockaddr_in saddr;
	char packet[50];

	saddr.sin_family = AF_INET;
	saddr.sin_port = 0;
	inet_pton(AF_INET,"127.0.0.1",(struct in_addr*)&saddr.sin_addr.s_addr);

	bzero(saddr.sin_zero,sizeof(saddr.sin_zero));
	while(1){
	bzero(packet,50);
	socklen_t *len = (socklen_t *)sizeof(saddr);
	int llen = sizeof(saddr);
	int n;

	if((n=recvfrom(rsfd,(char *)&packet,sizeof(packet),0,(struct sockaddr *)&saddr,&llen))<0){
		perror("packet recieve error!!\n");
		exit(0);
	}
	int i = sizeof(struct iphdr);
	printf("\n--------Addvertisement--------\n");
	for(;i<n;i++){
		printf("%c",packet[i]);
	}
	printf("\n------------------------------\n");
	
}
}
int main(int argc,char *argv[])
{
	printf("%d\n", argc);
	if(argc<2){
		printf("usage:-  ./pl <platform no.>");
		exit(0);
	}
	struct sockaddr_un userv_addr,ucli_addr;
	pthread_mutex_init(&mutex,0);
	//printf("yaar yeh kyu ni chal raha\n");
	char socks[15];
	int usfd = socket(AF_UNIX,SOCK_STREAM,0);
		if(usfd<0)
		{
			printf("socket error!!\n");
			exit(0);
		}
		bzero((struct sockaddr_un*)&userv_addr,sizeof(userv_addr));
  	userv_addr.sun_family = AF_UNIX;
	bzero(socks,15);
	sprintf(socks,"%s%s","skserv",argv[1]);
  	//sprintf(socks,"%s%d","skserv",i);
  	strcpy(userv_addr.sun_path,socks);
  	//strncpy(serv_addr.sun_path, "socket", sizeof(serv_addr.sun_path)-1);
    unlink(userv_addr.sun_path);
    int len = strlen(userv_addr.sun_path) + sizeof(userv_addr.sun_family);
    if(bind(usfd, (struct sockaddr*)&userv_addr, len)<0){
    	printf("bind error!!\n");
    	exit(0);
    }
    listen(usfd,5);
    int cllen =0;
    int nsfd = accept(usfd,(struct sockaddr*)&ucli_addr,&cllen);
    if(nsfd <0){
    	printf("accept error!!\n");
    	exit(0);
    }
    pthread_t pd;
    int x=0;
    //printf("noooooo-------------------------------------------------\n");
   pthread_create(&pd,NULL,serv,(void*)&x);
    //printf("yessss--------------------------------------------------\n");
    	//printf("wrongggg\n");
    	//pthread_mutex_lock(&mutex);

    	//printf("Train arrived..\n");
   while(1){
       char tr[10];
       bzero(tr,10);
       recv(nsfd,tr,10,0);
       printf("train from %s arriving at station %c\n",tr,argv[1][6]);
        fds=recv_fd(nsfd);
        printf("fd recieved\n");
       //printf("--->%d\n",fds);
       char buf[128];
       //sleep(2);
       while(1){
       	bzero(buf,128);
       	//printf("reading\n");
       	if(recv(fds,buf,128,0)==0)
       	{
       		int leng;
        struct ucred ucr;

        leng = sizeof(struct ucred);

        if (getsockopt(nsfd,SOL_SOCKET, SO_PEERCRED, &ucr, &leng) == -1) {
    //getsockopt failed
            exit(0);
            }
           // kill(ucr.pid,SIGUSR1);
           send(nsfd,buf,strlen(buf),0);
            printf("Train leaving station\n");
            
           
           sleep(2);
           break;
       	}
       	printf("%s\n",buf);
       }
       }	
    	//pthread_mutex_unlock(&mutex);
	//	pthread_join(pd,NULL);
}
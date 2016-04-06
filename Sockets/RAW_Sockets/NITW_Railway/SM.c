#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stropts.h>
#include <string.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <curses.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <sys/select.h>
#include <pthread.h>
#include <sys/wait.h>
int portno[3]={8001,8002,8003};
static int send_fd(
  int socket, /* Socket through which the file descriptor is passed */
  int fd_to_send) /* File descriptor to be passed, could be another socket */
{
 struct msghdr message;
 struct iovec iov[1];
 struct cmsghdr *control_message = NULL;
 char ctrl_buf[CMSG_SPACE(sizeof(int))];
 char data[1];

 memset(&message, 0, sizeof(struct msghdr));
 memset(ctrl_buf, 0, CMSG_SPACE(sizeof(int)));

 /* We are passing at least one byte of data so that recvmsg() will not return 0 */
 data[0] = ' ';
 iov[0].iov_base = data;
 iov[0].iov_len = sizeof(data);

 message.msg_name = NULL;
 message.msg_namelen = 0;
 message.msg_iov = iov;
 message.msg_iovlen = 1;
 message.msg_controllen =  CMSG_SPACE(sizeof(int));
 message.msg_control = ctrl_buf;

 control_message = CMSG_FIRSTHDR(&message);
 control_message->cmsg_level = SOL_SOCKET;
 control_message->cmsg_type = SCM_RIGHTS;
 control_message->cmsg_len = CMSG_LEN(sizeof(int));

 *((int *) CMSG_DATA(control_message)) = fd_to_send;

 return sendmsg(socket, &message, 0);
}
int status[3]={0};
int usfd[3];
void handler(){
       int i=0,st;
       printf("\n------------train left station------------\n");
	fd_set rdfd;
	
	struct timeval t;
	t.tv_sec = 2;
	t.tv_usec = 100;
	
	int maxfd =0;
	FD_ZERO(&rdfd);
	for(i=0;i<3;i++)
	{
        FD_SET(usfd[i],&rdfd);
        maxfd = maxfd>usfd[i] ? maxfd:usfd[i];
	}
	int ret = select(maxfd+1,&rdfd,NULL,NULL,&t);
	if(ret>0){
		char buf[10];
		bzero(buf,10);
		for(i=0;i<3;i++){
			if(FD_ISSET(usfd[i],&rdfd)){
				status[i]=0;
				printf("platform %d is now vaccant\n",i);
			}
		}
	
}
}
int main(){
	fd_set readfd;
	char se[7];
	sprintf(se,"%s","./pl");
	struct sockaddr_in serv_addr[3],cli_addr;
	struct sockaddr_un  userv_addr[3],ucli_addr;
	int i=0;
	int rsfd_send = socket(AF_INET,SOCK_RAW,251);
	if(rsfd_send<0){
		perror("socket!!\n");
		exit(0);
	}
	int sfd[3];
	char socks[15];
	for(i=0;i<3;i++){
		sfd[i] = socket(AF_INET,SOCK_STREAM,0);
		usfd[i] = socket(AF_UNIX,SOCK_STREAM,0);
		if(sfd[i]<0 || usfd[i]<0)
		{
			printf("socket error!!\n");
			exit(0);
		}
		bzero((struct sockaddr_un*)&userv_addr[i],sizeof(userv_addr[i]));
  	userv_addr[i].sun_family = AF_UNIX;
	bzero(socks,15);
	//printf("%d\n",portno[i] );
  	sprintf(socks,"%s%d","skserv",i);
  	strcpy(userv_addr[i].sun_path,socks);
  	//strncpy(serv_addr.sun_path, "socket", sizeof(serv_addr.sun_path)-1);
   // unlink(userv_addr[i].sun_path);
    

		bzero((char *) &serv_addr[i],sizeof(serv_addr[i]));
	serv_addr[i].sin_family = AF_INET;
	printf("%d\n",portno[i]);
	serv_addr[i].sin_port = htons(portno[i]);//htons  converts a port number in host byte order to a port number in network byte order
	serv_addr[i].sin_addr.s_addr = INADDR_ANY;
	if(bind(sfd[i],(struct sockaddr *) &serv_addr[i],sizeof(serv_addr[i]))<0)
	{
		printf("bind Error!!\n");
		exit(0);
	}
	listen(sfd[i],5);
		
					 int len = strlen(userv_addr[i].sun_path) + sizeof(userv_addr[i].sun_family);
					if(connect(usfd[i],(struct sockaddr*)&userv_addr[i],len)<0){
						printf("connect error!!\nplease start platform %d\n",i);
						exit(0);
					}
	}
	int maxfd =0;
	int nsfd;
	pthread_t pd;
	//pthread_create(&pd,NULL,handler,(void*)&maxfd);
	struct sockaddr_in saddr1;
	signal(SIGUSR1,handler);
	 saddr1.sin_family = AF_INET;
	 saddr1.sin_port=0;
	 inet_pton(AF_INET,"127.0.0.1",(struct in_addr*)&saddr1.sin_addr.s_addr);
	char *stations[] = {"delhi","vijaywada","hyderabad"};
	while(1){
		maxfd=0;
		FD_ZERO(&readfd);
		for(i=0;i<3;i++){
			FD_SET(sfd[i],&readfd);
			maxfd = maxfd>sfd[i]?maxfd:sfd[i];
		}

		int ret = select(maxfd+1,&readfd,NULL,NULL,NULL);
		if(ret>0){
			for(i=0;i<3;i++)
			{
				if(FD_ISSET(sfd[i],&readfd))
				{
					bzero((char *) &cli_addr,sizeof(cli_addr));
					int cllen =0;

					 nsfd = accept(sfd[i],(struct sockaddr*)&cli_addr,&cllen);
					if(nsfd<0){
						printf("accept error!!\n");
						exit(0);
					}
					printf("connection succesfull\n");
					int j;
					for(j=0;j<3;j++){
						if(status[j]==0)
						{
							break;
						}
					}

					if(j!=3){
						sprintf(socks,"%s","helo");
						send(nsfd,socks,strlen(socks),0);
						status[j]=1;
						
					
					printf("unix connection succesfull\n");
					send(usfd[j],stations[j],strlen(stations[j]),0);
					printf("sending fd\n");
					 memset(saddr1.sin_zero,0,sizeof(saddr1.sin_zero));
	 				char packet[100];
	 				bzero(packet,sizeof(packet));
	 				sprintf(packet,"%s%s%s%d","train from ",stations[j]," is arriving on platform no. ",j);
	 				if(sendto(rsfd_send,(char *)packet,sizeof(packet),0,(struct sockaddr *)&saddr1,(socklen_t)sizeof(saddr1))<0){
	 					perror("packet send error!!\n");
	 					exit(0);
	 				}
					sleep(1);
					send_fd(usfd[j],nsfd);
					printf("fd sent\n");
					close(nsfd);
				}
				else{
					char buf[]={"0"};
					send(nsfd,buf,strlen(buf),0);
					printf("stations are full!!\n");
				}
				}
			}
		}
	}
}
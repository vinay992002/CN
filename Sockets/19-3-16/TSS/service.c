#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stropts.h>
#include <string.h>
#include <fcntl.h>
#include <curses.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/select.h>
#include <pthread.h>
#include <sys/time.h>
int fds[10]={-1};
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
    recvmsg(socket, &message, 0);
  
     /* Iterate through header to find if there is a file descriptor */
     for(control_message = CMSG_FIRSTHDR(&message);
         control_message != NULL;
         control_message = CMSG_NXTHDR(&message,
                                       control_message))
     {
      if((control_message->cmsg_level == SOL_SOCKET) &&
          (control_message->cmsg_type == SCM_RIGHTS) )
      {
       int sd  =  *((int *) CMSG_DATA(control_message));
      // csfd[ccount++] = sd;
       
       //printf("Recieved\n");
      return sd;
      }
     
 }
}
void *serv(void *num){
	fd_set readfd;
	int maxfd =0;
	int i;
	struct timeval t;
	t.tv_sec=1;
	t.tv_usec =100;
	while(1){
		//printf("fuckkkkkkkkkkkkkkk");
		//pthread_mutex_lock(&mutex);
	//	printf("ok\n");
		maxfd =0;
		if(l>0){
		FD_ZERO(&readfd);
		//printf("%d",l);
		for(i=0;i<l;i++)
		{
			FD_SET(fds[i],&readfd);
			maxfd = maxfd>fds[i]?maxfd:fds[i];
		}
		//pthread_mutex_unlock(&mutex);
		int ret = select(maxfd+1,&readfd,NULL,NULL,&t);
		if(ret>0){
			for(i=0;i<l;i++){
				if(FD_ISSET(fds[i],&readfd))
				{
					//FD_CLR(fds[i],&readfd);
					char buf[1024];
					bzero(buf,1024);

					if(recv(fds[i],buf,1024,MSG_DONTWAIT)==0)
					{
						printf("client exiting\n");
						fds[i]=-1;
					}
					printf("%s\n",buf );
					if(strcmp(buf,"-1")==0)
					{
						fds[i]=-1;
						printf("client exiting\n");
					}
					else{
						int j;
						char temp[1024];
						for(j=0;j<strlen(buf);j++)
						{
							if(buf[j]>=97)
								temp[j] = buf[j]-32;
							else
								temp[j]=buf[j];
			//printf("%c",temp[i]);
						}
						temp[j]='\0';
						send(fds[i],temp,strlen(temp),0);
					}

				}
			}
		}
		
	}	}
}
int main(int argc,char *argv[])
{
	struct sockaddr_un userv_addr,ucli_addr;
	pthread_mutex_init(&mutex,0);
	printf("yaar yeh kyu ni chal raha\n");
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
  	//sprintf(socks,"%s%d","skserv",i);
  	strcpy(userv_addr.sun_path,argv[1]);
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
    printf("noooooo-------------------------------------------------\n");
    pthread_create(&pd,NULL,serv,(void*)&x);
    printf("yessss--------------------------------------------------\n");
    while(1){
    	printf("wrongggg\n");
    	//pthread_mutex_lock(&mutex);
    	printf("working");
    	fds[l++]=recv_fd(nsfd);
    	//pthread_mutex_unlock(&mutex);
}
	//	pthread_join(pd,NULL);
}
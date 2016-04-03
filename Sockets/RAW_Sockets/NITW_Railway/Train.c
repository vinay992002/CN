#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
int port;
int main(int argc,char *argv[]){
	//signal(SIGUSR1,handler);
	struct sockaddr_in serv_addr;
	struct hostent *serv;
	port = atoi(argv[1]);
	int sfd = socket(AF_INET,SOCK_STREAM,0);
	if(sfd<0)
	{
		printf("socket error!!\n");
		exit(0);
	}	
    serv = gethostbyname("localhost");
    if(serv==NULL){
    	printf("gethostbyname error!!\n");
    	exit(0);
    }
    bcopy((char *)serv->h_addr,(char*)&serv_addr.sin_addr.s_addr,serv->h_length);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port= htons(port);
    printf("%d\n",port);
    if(connect(sfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0){
    	printf("connect error!!\n");   	
    	exit(0);
    }
    printf("connection succesfull\n");
    char msg[20];
    recv(sfd,msg,20,0);
    printf("%s\n",msg );
    if(msg[0]!='0'){
    sleep(3);
    port = atoi(msg);
    //shutdown(sfd,SHUT_RDWR);
    bzero(msg,20);

    sprintf(msg,"%s","train has arrived!!");
    send(sfd,msg,strlen(msg),0);
    int j;
    char ste[]={"S"};
    bzero(msg,20);
    printf("sending info\n");
    sprintf(msg,"%s","Gen");
    send(sfd,msg,strlen(msg),0);
    sleep(0.5);
    for(j=1;j<=10;j++){
        bzero(msg,20);
        sleep(0.5);
        sprintf(msg,"%s%d",ste,j);
        send(sfd,msg,strlen(msg),0);
    }
    sleep(6);
    bzero(msg,20);
    sprintf(msg,"%s","train is leaving!!");
    send(sfd,msg,strlen(msg),0);
}else{
	printf("stations are not free!!\n");
}
}
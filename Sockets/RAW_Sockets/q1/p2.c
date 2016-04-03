#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
int main(){
	int rsfd_recv;
	rsfd_recv = socket(AF_INET,SOCK_RAW,250);
	if(rsfd_recv<0){
		perror("socket error!!\n");
		exit(0);
	}
	struct sockaddr_in raddr;
	char packet[50];

	raddr.sin_family = AF_INET;
	raddr.sin_port=0;
	inet_pton(AF_INET,"127.0.0.1",(struct in_addr *)&raddr.sin_addr.s_addr);
	bzero(raddr.sin_zero,sizeof(raddr.sin_zero));
	bzero(packet,50);
	socklen_t *len = (socklen_t *)sizeof(raddr);
	int llen = sizeof(raddr);

	int n;
	if((n=recvfrom(rsfd_recv,(char *)&packet,sizeof(packet),0,(struct sockaddr *)&raddr,&llen))<0){
		perror("packet recieve error!!\n");
		exit(0);
	}
	int flag=0;
	struct iphdr *ip=  (struct iphdr*)packet;
	printf("-------------------------------------\n");
	printf("|%4d|%4d|%8d|%16d|\n",ip->version,ip->ihl,ip->tos,ip->tot_len);
	printf("-------------------------------------\n");
	printf("|%12d|%8d|%13d|\n",ip->id,flag,ip->frag_off);
	printf("-------------------------------------\n");
	printf("|%8d|%8d|%17d|\n",ip->ttl,ip->protocol,ip->check );
	printf("-------------------------------------\n");
	printf("|%35d|\n",ip->saddr);
	printf("-------------------------------------\n");
	printf("|%35d|\n",ip->daddr);
	printf("-------------------------------------\n");
}	

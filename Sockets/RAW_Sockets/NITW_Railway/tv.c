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
	int rsfd_send = socket(AF_INET,SOCK_RAW,250);
	if(rsfd_send<0){
		perror("socket!!\n");
		exit(0);
	}
	int x=1;
	const int *y = &x;
	char add[]={"addvertisements are now available "};
 int i=0;
	for(;i<20;i++){
	struct sockaddr_in saddr;
	char packet[50];
	 struct iphdr *ip =(struct iphdr *)packet;
     
	 saddr.sin_family = AF_INET;
	 saddr.sin_port=0;
	 inet_pton(AF_INET,"127.0.0.1",(struct in_addr*)&saddr.sin_addr.s_addr);
	 memset(saddr.sin_zero,0,sizeof(saddr.sin_zero));
	 bzero(packet,sizeof(packet));
	 ip->version = 4;
	 ip-> ihl = 5;
	 ip->tos=0;
	 ip->frag_off =0;
	 ip->ttl = 64;
	 ip->protocol = 250;
	 ip->check = 0;
	 ip->saddr = saddr.sin_addr.s_addr;
	 ip->daddr = saddr.sin_addr.s_addr;

	 
	 if(setsockopt(rsfd_send,IPPROTO_IP,IP_HDRINCL,y,sizeof(x))<0){
		perror("setsockopt!!\n");
		exit(0);
	}
	 
	 
	 strcpy(packet+sizeof(struct iphdr),add);
	 ip->tot_len = htons(sizeof(struct iphdr)+sizeof(add));
	
	 if(sendto(rsfd_send,(char *)packet,sizeof(packet),0,(struct sockaddr *)&saddr,(socklen_t)sizeof(saddr))<0){
	 	perror("packet send error!!\n");
	 	exit(0);
	 }
	 sleep(2);
}
}

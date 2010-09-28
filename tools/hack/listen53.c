#include<string.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
		
#define	BUF_SIZE	1024

int main( int argc , char *argv[] )
{
	struct 	sockaddr_in host_address;
	int 	hst_addr_size = sizeof(host_address);
	char 	buffer[BUF_SIZE];
	int 	sock;
	int 	port = 53;
	int 	length;
	if ( argc == 2 ){
		port = atoi(argv[1]);
	}

	sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	/*init the host_address the socket is beeing bound to*/
	memset((void*)&host_address, 0, sizeof(host_address));
	/*set address family*/
	host_address.sin_family		= PF_INET;
	/*accept any incoming messages:*/
	host_address.sin_addr.s_addr	= INADDR_ANY;
	/*the port the socket i to be bound to:*/
	host_address.sin_port		= htons(port);

	bind( sock , (struct sockaddr*)&host_address, sizeof(host_address) );
	
	length = recvfrom( sock, buffer, BUF_SIZE, 0, (struct sockaddr*)&host_address, &hst_addr_size );
	printf("%d\n",length);
	
	
	
	
}

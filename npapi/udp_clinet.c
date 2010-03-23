#include "winsock2.h"
#include "stdio.h"
#include "stdlib.h"

#define WSVERS MAKEWORD(2, 0)

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
    if (host != NULL) {
       return (struct in_addr *) *host->h_addr_list;
    }
    return NULL;
}

int main( int argc , char **argv)
{
    SOCKET UDP_CLINET_SOCKET;
    WSADATA WSAData;
    WSAStartup(WSVERS, &WSAData);  // start winsock
    struct in_addr *addr;
    struct sockaddr_in server;
    int len = sizeof(server);
    int portnumber;
    char address[255];
    // intput server address or domain name
    printf("Clinet to :");
    scanf("%s",address);
    // domain name -> IP
    addr = atoaddr(address);
    if ( addr == NULL ) 
    {
        fprintf(stderr,"Invalid network address.\n");
        return -1;
    }
    // intput server listen port
    //printf("Clinet to port: ");
    //scanf("%d",&portnumber);
    
    // combine server info into struct sockaddr_in
    server.sin_family=AF_INET;
    server.sin_port=htons(53);//portnumber);
    server.sin_addr.s_addr=addr->s_addr;
    
    //  create socket with UDP
    UDP_CLINET_SOCKET = socket(AF_INET,SOCK_DGRAM,0);
    
    if ( UDP_CLINET_SOCKET == INVALID_SOCKET) 
    {
       printf("Error at socket(): %ld\n", WSAGetLastError() );
       WSACleanup();
       return -1 ;
    }
    
    //  main loop
    while (1)
    {
         char buffer[1024]="\0";
         // input message
         printf("Input message: ");
         scanf("%s",buffer);
         // send message by UDP
         sendto(UDP_CLINET_SOCKET,buffer,sizeof(buffer),0,(struct sockaddr*)&server,len);
         
         // exit main loop
         if (strcmp(buffer,"exit")==0)
         {
             printf("Exit!\n");
             Sleep(100);
             closesocket(UDP_CLINET_SOCKET);
             break;
         }
         
         
    }
     
    // colse socket 
    closesocket(UDP_CLINET_SOCKET);
    WSACleanup();
    system("pause");
    return 0;
}


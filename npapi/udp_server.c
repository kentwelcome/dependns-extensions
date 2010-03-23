#include "winsock2.h"
#include "stdio.h"
#include "stdlib.h"

#define WSVERS MAKEWORD(2, 0)

int main( int argc , char **argv)
{
    SOCKET UDP_SERVER_SOCKET;
    WSADATA WSAData;
    WSAStartup(WSVERS, &WSAData);  // start winsock
    
    struct sockaddr_in local;
    struct sockaddr_in from;
    int fromlen =sizeof(from);
    int portnumber;
    
    // intput listen port
    printf("Clinet to port: ");
    scanf("%d",&portnumber);
    
    // combine server info into struct sockaddr_in
    local.sin_family=AF_INET;
    local.sin_port=htons(portnumber); 
    local.sin_addr.s_addr=INADDR_ANY; 
    
    // create sock with UDP
    UDP_SERVER_SOCKET=socket(AF_INET,SOCK_DGRAM,0);
    if ( UDP_SERVER_SOCKET == INVALID_SOCKET) 
    {
       printf("Error at socket(): %ld\n", WSAGetLastError());
       WSACleanup();
       return -1 ;
    }
    
    // bind server info with socket
    if( bind(UDP_SERVER_SOCKET,(struct sockaddr*)&local,sizeof(local))== SOCKET_ERROR )
    {
       printf("bind() failed. %ld\n", WSAGetLastError());
       closesocket(UDP_SERVER_SOCKET);
       return -1;
     
    }
    
    // main loop
    while (1)
    {
        char buffer[1024]="\0";
        printf("waiting for message...\n");
        
        // get recv and print message
        if ( recvfrom(UDP_SERVER_SOCKET,buffer,sizeof(buffer),0,(struct sockaddr*)&from,&fromlen)!=SOCKET_ERROR )
        {
           printf("Received datagram from %s:%d -- %s\n",inet_ntoa(from.sin_addr),(int)ntohs(from.sin_port),buffer);
           
        }
        // exit main loop
        if( strcmp( buffer , "exit") == 0 )
            break;
        
    }
    
    // close sock
    closesocket(UDP_SERVER_SOCKET);
    WSACleanup();  // close winsock
    system("pause");
    return 0;
}

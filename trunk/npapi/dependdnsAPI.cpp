/**********************************************************\
Original Author: Richard Bateman and Georg Fritzsche

Created:    December 3, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass Inc, Georg Fritzsche,
               Firebreath development team
\**********************************************************/

#include "BrowserObjectAPI.h"
#include "variant_list.h"
#include "DOM/JSAPI_DOMDocument.h"

#include "dependdnsAPI.h"

dependdnsAPI::dependdnsAPI(FB::BrowserHostWrapper *host) : m_host(host)
{
    registerMethod("echo",  make_method(this, &dependdnsAPI::echo));
	registerMethod("SendQuery",  make_method(this, &dependdnsAPI::SendQuery));
	registerMethod("SendLocal",  make_method(this, &dependdnsAPI::SendLocal));

    // Read-write property
    registerProperty("testString",
                     make_property(this,
                        &dependdnsAPI::get_testString,
                        &dependdnsAPI::set_testString));

    // Read-only property
    registerProperty("version",
                     make_property(this,
                        &dependdnsAPI::get_version));
}

dependdnsAPI::~dependdnsAPI()
{
}

// Read/Write property testString
std::string dependdnsAPI::get_testString()
{
    return m_testString;
}
void dependdnsAPI::set_testString(std::string val)
{
    m_testString = val;
}

// Read-only property version
std::string dependdnsAPI::get_version()
{
    return "CURRENT_VERSION";
}

// Method echo
FB::variant dependdnsAPI::echo(FB::variant msg)
{
	return msg;
}

// Method SendQuery
FB::VariantList dependdnsAPI::SendQuery(std::string msg , std::string resolver)
{
	SOCKET UDP_CLINET_SOCKET;
    WSADATA WSAData;
    WSAStartup(WSVERS, &WSAData);  // start winsock

	//struct in_addr *DNS_server_addr;
	struct sockaddr_in DNS_server;
	int len , query_len;
	int port = 53;
	unsigned char buffer[1024];
	unsigned char url[256];
	unsigned char *h_ptr;
	struct DNS_header	*dns_ptr;
	struct RR			*rr_ptr;
	struct Quset		*que_ptr;
	unsigned char *que_url;
	unsigned char *ans_ptr;
	int ans_num;
	std::string ip_address;
	FB::VariantList return_value;
	
	//char *test;
	
	
	DNS_server.sin_family = AF_INET;
	DNS_server.sin_addr.S_un.S_addr = inet_addr(resolver.c_str());
	DNS_server.sin_port = htons(port);
	
	UDP_CLINET_SOCKET = socket( AF_INET , SOCK_DGRAM , 0 );

	if (UDP_CLINET_SOCKET == INVALID_SOCKET){
		WSACleanup();
		return return_value;
	}
	strncpy ( (char*)url , msg.c_str() , msg.length() );
	// DNS Header
	h_ptr = buffer;
	dns_ptr = (struct DNS_header*)h_ptr;
	dns_ptr->QID = htons(rand()%0xFFFF);
	dns_ptr->opt = htons(0x0100);
	dns_ptr->Questions = htons(1);
	dns_ptr->Answer_RRS = 0;
	dns_ptr->Authority_RRS = 0;
	dns_ptr->Additional_RRS = 0;
	query_len = sizeof(struct DNS_header);
	// Question Header
	h_ptr = buffer + sizeof(struct DNS_header);
	//strncpy( (char*)url , "www.google.com" , strlen("www.google.com") );
	//url[strlen("www.google.com")] = '\0';
	len = cpy_url( (unsigned char*)msg.c_str() , h_ptr , msg.length() );
	query_len += len;
	que_ptr = (struct Quset*) (h_ptr + len);
	que_ptr->Type = htons(0x01);
	que_ptr->Class = htons(0x01);
	query_len += sizeof(struct Quset);
	
	sendto(UDP_CLINET_SOCKET,(char*)buffer,query_len,0,(struct sockaddr*)&DNS_server,sizeof(DNS_server));
	
	
	len = sizeof(DNS_server);
	recvfrom(UDP_CLINET_SOCKET,(char*)buffer,sizeof(buffer),0,(struct sockaddr*)&DNS_server,&len);
	// DNS Header
	h_ptr = buffer;
	dns_ptr = (struct DNS_header*)h_ptr;
	ans_num = ntohs(dns_ptr->Answer_RRS);
	// Parsing Question URL
	que_url = (unsigned char*) &dns_ptr[1];
	que_ptr = (struct Quset*) &que_url[strlen((char*)que_url)+1];
	ans_ptr = (unsigned char*) &que_ptr[1];
	
	
	for ( int i = 0 ; i < ans_num ; i++ ){
		// Parsing Answer
		if ( ans_ptr[0] == 0xc0 ){
			que_ptr = (struct Quset*)&ans_ptr[2];
			rr_ptr = (struct RR*)&que_ptr[1];
			ans_ptr = (unsigned char*)&rr_ptr[1];

			if ( ntohs(que_ptr->Type) == 0x05 ){	// CNAME record
				//printf(" CNAME: ");
				//print_url( buffer , ans_ptr );
				//printf("\n");
			} else if ( ntohs(que_ptr->Type) == 0x01 ){	// A record
				struct in_addr Server_ip;
				Server_ip.S_un.S_addr = ( (struct in_addr*)ans_ptr )->S_un.S_addr;
				ip_address = inet_ntoa(Server_ip);
				return_value.push_back(ip_address);
			}
			ans_ptr = ans_ptr + ntohs(rr_ptr->Rdata_len);
		}
	}
	WSACleanup();
	return return_value;
}




// Method SendLocal
FB::VariantList dependdnsAPI::SendLocal(std::string msg)
{
	std::string Hostname;
	//std::vector<std::string>::iterator it;
	struct hostent *remoteHost;
	static struct in_addr saddr;
	FB::VariantList return_value;
	//FB::VariantList::iterator it;
	//it = return_value.begin();
	

	WSADATA WSAData;
    WSAStartup(WSVERS, &WSAData);  // start winsock

	remoteHost = gethostbyname(msg.c_str());
	if ( remoteHost != NULL ){
		int i=0;
		while ( remoteHost->h_addr_list[i] ){
			saddr.S_un.S_addr = ((struct in_addr *)remoteHost->h_addr_list[i])->S_un.S_addr;
			Hostname = inet_ntoa(saddr);
			return_value.push_back( Hostname );
			i++;
		}
		
	}
	WSACleanup();
	return return_value;
}


int cpy_url( unsigned char* url , unsigned char* buffer , int url_size )
{

	unsigned char counter; //8 bit
	unsigned char *tmp_ptr , *url_ptr;
	int i = 0;
	int url_len = 0;
	url_ptr = url;

	while ( i < url_size ){	
		counter = 0x0;
		tmp_ptr = url_ptr;
		while ( *url_ptr != '.' && i < url_size ){
			counter++;
			url_ptr++;
			i++;
		}
		i++;
		//*url_ptr = '\0';
		buffer[(int)url_len++] = counter;
		strncpy( ((char*)buffer+(int)url_len) , (char*)tmp_ptr , counter );
		url_len += (int) counter;
		url_ptr++;
		tmp_ptr = url_ptr;

	}
	buffer[(int)url_len++] = 0x0;
	return url_len;
}
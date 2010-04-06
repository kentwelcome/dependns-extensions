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

#include "winsock2.h"
#include <string>
#include <sstream>
#include "JSAPIAuto.h"
#include "BrowserHostWrapper.h"


#define WSVERS MAKEWORD(2, 0)


class dependdnsAPI : public FB::JSAPIAuto
{
public:
    dependdnsAPI(FB::BrowserHostWrapper *host);
    virtual ~dependdnsAPI();

    // Read/Write property ${PROPERTY.ident}
    std::string get_testString();
    void set_testString(std::string val);

    // Read-only property ${PROPERTY.ident}
    std::string get_version();

    // Method echo
    FB::variant echo(FB::variant msg);

	// Method SendQuery
	FB::VariantList dependdnsAPI::SendQuery(std::string msg,std::string resolver);

	// Method SendLocal
	FB::VariantList dependdnsAPI::SendLocal(std::string msg);

private:
    FB::AutoPtr<FB::BrowserHostWrapper> m_host;

    std::string m_testString;
};

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

int cpy_url( unsigned char* url , unsigned char* buffer , int url_size );
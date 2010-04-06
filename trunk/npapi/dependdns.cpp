/**********************************************************\

  Auto-generated dependdns.cpp

  This file contains the auto-generated main plugin object
  implementation for the dependdns project

\**********************************************************/

#include "NpapiTypes.h"
#include "dependdnsAPI.h"

#include "dependdns.h"

void dependdns::StaticInitialize()
{
    // Place one-time initialization stuff here; note that there isn't an absolute guarantee that
    // this will only execute once per process, just a guarantee that it won't execute again until
    // after StaticDeinitialize is called
}

void dependdns::StaticDeinitialize()
{
    // Place one-time deinitialization stuff here
}


dependdns::dependdns()
{
}

dependdns::~dependdns()
{
}

FB::JSAPI* dependdns::createJSAPI()
{
    // m_host is the BrowserHostWrapper
    return new dependdnsAPI(m_host);
}

bool dependdns::onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow *)
{
    //printf("Mouse down at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}

bool dependdns::onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow *)
{
    //printf("Mouse up at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}

bool dependdns::onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow *)
{
    //printf("Mouse move at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}
bool dependdns::onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *)
{
    // The window is attached; act appropriately
    return false;
}

bool dependdns::onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *)
{
    // The window is about to be detached; act appropriately
    return false;
}

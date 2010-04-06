/**********************************************************\

  Auto-generated FactoryMain.cpp

  This file contains the auto-generated factory methods
  for the dependdns project

\**********************************************************/

#include "FactoryDefinitions.h"
#include "dependdns.h"

FB::PluginCore *_getMainPlugin()
{
    return new dependdns();
}

void GlobalPluginInitialize()
{
    dependdns::StaticInitialize();
}

void GlobalPluginDeinitialize()
{
    dependdns::StaticDeinitialize();
}

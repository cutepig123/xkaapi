/*
** xkaapi
** 
** Copyright 2010 INRIA.
**
** Contributors :
**
** thierry.gautier@inrialpes.fr
** 
** This software is a computer program whose purpose is to execute
** multithreaded computation with data flow synchronization between
** threads.
** 
** This software is governed by the CeCILL-C license under French law
** and abiding by the rules of distribution of free software.  You can
** use, modify and/ or redistribute the software under the terms of
** the CeCILL-C license as circulated by CEA, CNRS and INRIA at the
** following URL "http://www.cecill.info".
** 
** As a counterpart to the access to the source code and rights to
** copy, modify and redistribute granted by the license, users are
** provided only with a limited warranty and the software's author,
** the holder of the economic rights, and the successive licensors
** have only limited liability.
** 
** In this respect, the user's attention is drawn to the risks
** associated with loading, using, modifying and/or developing or
** reproducing the software by the user in light of its specific
** status of free software, that may mean that it is complicated to
** manipulate, and that also therefore means that it is reserved for
** developers and experienced professionals having in-depth computer
** knowledge. Users are therefore encouraged to load and test the
** software's suitability as regards their requirements in conditions
** enabling the security of their systems and/or data to be ensured
** and, more generally, to use and operate it in the same conditions
** as regards security.
** 
** The fact that you are presently reading this means that you have
** had knowledge of the CeCILL-C license and that you accept its
** terms.
** 
*/
#include "kanet_device.h" 
#include <string.h>
#include <map>

namespace Net {
// --------------------------------------------------------------------
static std::map<const char*,DeviceFactory*> all_devicefact;


// --------------------------------------------------------------------
DeviceFactory::~DeviceFactory()
{ }


// --------------------------------------------------------------------
void DeviceFactory::destroy( Device* dev )
{ 
  if (dev !=0) {
    dev->terminate();
    delete dev;
  }
}


// --------------------------------------------------------------------
Device::Device( const char* name ) 
{
  memset(_name, 0, sizeof(_name));
  if (name != 0)
    strncpy(_name, name, 31);  
}


// --------------------------------------------------------------------
Device::~Device()
{ 
}


// --------------------------------------------------------------------
int Device::register_factory( const char* name, DeviceFactory* df )
{
  all_devicefact.insert( std::make_pair(name, df) );
}


// --------------------------------------------------------------------
DeviceFactory* Device::resolve_factory( const char* name )
{
  std::map<const char*,DeviceFactory*>::const_iterator iterator = all_devicefact.find(name);
  if (iterator == all_devicefact.end()) return 0;
  return iterator->second;
}

} // - namespace Net...

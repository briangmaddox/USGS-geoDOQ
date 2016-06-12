// $Id: outputfuncs.cpp,v 1.1.1.1 2002-02-20 16:25:09 bmaddox Exp $
// Brian G. Maddox
// Last modified by $Author: bmaddox $ on $Date: 2002-02-20 16:25:09 $

/* The contents of this file are subject to the Mozilla Public License
   Version 1.0 (the "License"); you may not use this file except in
   compliance with the License. You may obtain a copy of the License at
   http://www.mozilla.org/MPL/ */

#ifndef _OUTPUTFUNCS_CPP_
#define _OUTPUTFUNCS_CPP_

#include "outputfuncs.h"
#include <iostream>

void doq2geotiffusage(const std::string& myname) throw()
{
  std::cerr << "Usage: " << myname << " [options] doq-file basename"
            << std::endl;
  std::cerr << "where options are:" << std::endl;
  std::cerr << "  -p     use primary datum information (default)" << std::endl;
  std::cerr << "  -s     use secondary datum information" << std::endl;
  std::cerr << "  -t     generate ARC World file (basename.tfw)" << std::endl;
  std::cerr << "  -h     output header file (basename.hdr)" << std::endl;
  std::cerr << "  -V     output version number" << std::endl;
  std::cerr << "  -?     display this information" << std::endl;
  return;
}


// ***************************************************************************
void displayWin32Info(const std::string& myname) throw()
{
  int foo; // just to get the user to 
  std::cout << "Notice:  This Win32 version of the " << myname
	    << " product is unsupported." << std::endl;
  std::cout << "Use of this product is at your own risk.  NO level of support "
	    << std::endl
	    << "exists for this product other than the how to run the software"
	    << std::endl
	    << "on your platform.  There is also no guarantee that this "
	    << std::endl
	    << "software will be maintained in future versions.  The end user "
	    << std::endl
	    << "of this package assumes all responsibility and liability for "
	    << std::endl
	    << "any incorrect operation of this program or any incorrect "
	    << std::endl
	    << "output or damage as a result of using this version.  Usage "
	    << std::endl
	    << "of this software constitutes the end user's agreement to "
	    << std::endl
	    << "these terms." << std::endl;
  std::cout << "Press the Enter or Return key to continue...";
  std::cin.get();
  return;
}
    
#endif

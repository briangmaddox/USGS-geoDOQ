// $Id: outputfuncs.h,v 1.1.1.1 2002-02-20 16:25:09 bmaddox Exp $
// Brian G. Maddox
// Last modified by $Author: bmaddox $ on $Date: 2002-02-20 16:25:09 $

/* The contents of this file are subject to the Mozilla Public License
   Version 1.0 (the "License"); you may not use this file except in
   compliance with the License. You may obtain a copy of the License at
   http://www.mozilla.org/MPL/ */

#ifndef _OUTPUTFUNCS_H_
#define _OUTPUTFUNCS_H_

#include <string>
// These are output functions for doq2geotiff or other utilities that need
// them.

// Display the doq2geotiff usage statement
void doq2geotiffusage(const std::string& myname) throw();

// Display the generic Win32 warning
void displayWin32Info(const std::string& myname) throw();

#endif

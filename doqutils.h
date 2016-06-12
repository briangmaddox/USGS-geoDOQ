// $Id: doqutils.h,v 1.1.1.1 2002-02-20 16:25:09 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-20 16:25:09 $

// Header file for the functions in doqutils.cc
/* The contents of this file are subject to the Mozilla Public License
   Version 1.0 (the "License"); you may not use this file except in
   compliance with the License. You may obtain a copy of the License at
   http://www.mozilla.org/MPL/ */

#ifndef DOQUTILS_H
#define DOQUTILS_H
#include "ImageLib/DOQImageIFile.h"
#include "config.h"

bool writeHeader(USGSImageLib::DOQImageIFile* doqin, 
		 const std::string& doqname,
                 const std::string& basename);
bool makeKeywordTFW(USGSImageLib::DOQImageIFile* indoq, 
		    const std::string& basename, 
                    const bool& usePrimaryDatum);
// Process out command line arguments. Return false if the program is to exit,
// true if all is well
bool processArgs(bool& usePrimaryDatum, bool& makeTFW, bool& printHeader,
		 const std::string& myname, std::string& doqname,
		 std::string& basename, int& argc, char** argv) throw();



#endif

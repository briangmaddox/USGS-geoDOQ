// $Id: writeutils.h,v 1.2 2002-06-21 03:09:01 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-06-21 03:09:01 $

// Header file to define the output functions
/* The contents of this file are subject to the Mozilla Public License
   Version 1.0 (the "License"); you may not use this file except in
   compliance with the License. You may obtain a copy of the License at
   http://www.mozilla.org/MPL/ */

#ifndef _WRITEUTILS_H_
#define _WRITEUTILS_H_

#include "ImageLib/DOQImageIFile.h"
#include "ImageLib/DOQ_RecImageIFile.h"

bool writeKeywordHdr(USGSImageLib::DOQImageIFile* indoq, char* outfile,
                     bool useprimarydatum) throw();
bool writeRecordHdr(USGSImageLib::DOQ_RecImageIFile* indoq, char* outfile,
                    bool useprimarydatum)  throw();

#endif // #ifndef _WRITEUTILS_H_

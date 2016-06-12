// $Id: spcsutils.h,v 1.2 2002-06-21 03:09:01 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-06-21 03:09:01 $
// This is the header for the State Plane stuff.  Moved it here cause the
// init function would be way big since I don't want to rely on keeping
// a separate file with this one.

/* The contents of this file are subject to the Mozilla Public License
   Version 1.0 (the "License"); you may not use this file except in
   compliance with the License. You may obtain a copy of the License at
   http://www.mozilla.org/MPL/ */

#ifndef SPCSUTILS_H
#define SPCSUTILS_H

#include <vector>
#include <string>
#include "ImageLib/DOQImageIFile.h"
#include "geotiff.h"

// This structure hold the datum code, the state code, and the zone name
struct spcsstruct
{
  int zonecode;
  std::string statename;
  std::string stateabbrev;
  std::string zonename;
};


// This array holds the mappings needed to go from USGS SPCS codes to GeoTIFF
// codes.  Yeah, this is probably not the coolest way to do it, but I'll betcha
// it's faster ;)
extern int mappings[6000];

// These functions init the mappings array to either the NAD27 or NAD83 codes
void initNAD27Mappings() throw();
void initNAD83Mappings() throw();

// This makes sure that all the SPCS info is OK and also goes ahead and sets
// the units to encode into the GeoTIFF
bool validateSPCS(USGSImageLib::DOQImageIFile* doqin, 
		  unsigned short& whichunits, 
		  const bool& useprimarydatum) throw();
  
#endif

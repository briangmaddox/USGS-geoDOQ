// $Id: testdoq.cpp,v 1.1.1.1 2002-02-20 16:25:09 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-20 16:25:09 $

// This is a test program for the new Image library.  It basically instantiates
// a DOQImageIFile object and makes sure that it's being parsed correctly.

/* The contents of this file are subject to the Mozilla Public License
   Version 1.0 (the "License"); you may not use this file except in
   compliance with the License. You may obtain a copy of the License at
   http://www.mozilla.org/MPL/ */

#include "ImageLib/DOQImageIFile.h"
#include <iostream>
#include <iomanip>
#include <string>

int main (int argc, char* argv[])
{
  USGSImageLib::DOQImageIFile* foo = NULL;
  double dtemp;
  std::string stemp;
  long int litemp;
  int itemp;
  float ftemp;
  int loopcount;
  std::string exceptionstring;
  std::string filename(argv[1]);

  if (argc < 2)
  {
    std::cerr << "Usage: " << argv[0] << "DOQ-file" << std::endl;
    exit(-1);
  }

  try
  {
    if (!(foo = new(std::nothrow) USGSImageLib::DOQImageIFile(filename)))
      throw std::bad_alloc();
    if (!foo->good())
    {
      std::cerr << "An error has occurred opening the DOQ.  Exiting..."
		<< std::endl;
      delete foo;
      exit(-1);
    }
    
    foo->getQuadName(stemp);
    std::cout << "Quadname: " << stemp << std::endl;
    foo->getQuadrant(stemp);
    std::cout << "Quadrant: " << stemp << std::endl;
    foo->getWestHour(itemp);     std::cout << "West HMS: " << itemp;
    foo->getWestMinute(itemp);   std::cout << " " << itemp << " ";
    foo->getWestSecond(ftemp);   std::cout << ftemp << std::endl;
    foo->getEastHour(itemp);     std::cout << "East HMS: " << itemp;
    foo->getEastMinute(itemp);   std::cout << " " << itemp << " ";
    foo->getEastSecond(ftemp);   std::cout << ftemp << std::endl;
    foo->getNorthHour(itemp);    std::cout << "North HMS: " << itemp;
    foo->getNorthMinute(itemp);  std::cout << " " << itemp << " ";
    foo->getNorthSecond(ftemp);  std::cout << ftemp << std::endl;
    foo->getSouthHour(itemp);    std::cout << "South HMS: " << itemp;
    foo->getSouthMinute(itemp);  std::cout << " " << itemp << " ";
    foo->getSouthSecond(ftemp);  std::cout << ftemp << std::endl;
  
    foo->getProdDate(stemp);
    std::cout << "Production Date: " << stemp << std::endl;

    foo->getRasterOrder(stemp); std::cout << "Raster Order: " << stemp 
					  << std::endl;
    foo->getBandOrganization(stemp); std::cout << "Band Organization: " 
					       << stemp << std::endl;
    
    foo->getNumBandContents(itemp);
    std::cout << "Band Content Count: " << itemp << std::endl;
    for (loopcount = 1; loopcount <= itemp; loopcount++)
    {
      foo->getBandContent(stemp, loopcount);
      std::cout << "Band Content[" << loopcount << "]: " << stemp << std::endl;
    }

    foo->getBitsPerPixel(itemp); std::cout << "Bits/Pixel: " << itemp 
					   << std::endl;
    foo->getSamples(litemp); std::cout << "Samples: " << litemp << std::endl;
    foo->getLines(litemp); std::cout << "Lines: " << litemp << std::endl;

    foo->getHorizontalDatum(stemp); std::cout << "Datum: " << stemp 
					      << std::endl;
    foo->getHorizontalCoordinateSystem(stemp); std::cout << "Coord Sys: "
							 << stemp << std::endl;
    foo->getCoordinateZone(itemp); std::cout << "Zone: " << itemp << std::endl;
    foo->getHorizontalUnits(stemp); std::cout << "Horiz Units: " << stemp 
					      << std::endl;
    foo->getHorizontalResolution(ftemp); std::cout << "Horiz Res: " << ftemp 
						   << std::endl;
    foo->getSecondaryHorizontalDatum(stemp); std::cout << "Sec Horiz Datum: "
						       << stemp << std::endl;
    foo->getXOrigin(dtemp); std::cout << "Xorigin: " << dtemp << std::endl;
    foo->getYOrigin(dtemp); std::cout << "Yorigin: " << dtemp << std::endl;
    foo->getSecondaryXOrigin(dtemp); std::cout << "SXOrigin: " << dtemp 
					       << std::endl;
    foo->getSecondaryYOrigin(dtemp); std::cout << "SYOrigin: " << dtemp 
					       << std::endl;

    foo->getNumNations(itemp); std::cout << "Num Nations: " << itemp 
					 << std::endl;
    for (loopcount = 1; loopcount <= itemp; loopcount++)
    {
      foo->getNation(stemp, loopcount);
      std::cout << "Nation[" << loopcount << "]: " << stemp << std::endl;
    }
    foo->getNumStates(itemp); std::cout << "Num States: " << itemp 
					<< std::endl;
    for (loopcount = 1; loopcount <= itemp; loopcount++)
    {
      foo->getState(stemp, loopcount);
      std::cout << "State[" << loopcount << "]: " << stemp << std::endl;
    }
    foo->getNWQuadX(dtemp); std::cout << "NWQuadX: " << dtemp << std::endl;
    foo->getNWQuadY(dtemp); std::cout << "NWQuadY: " << dtemp << std::endl;
    foo->getNEQuadX(dtemp); std::cout << "NEQuadX: " << dtemp << std::endl;
    foo->getNEQuadY(dtemp); std::cout << "NEQuadY: " << dtemp << std::endl;
    foo->getSEQuadX(dtemp); std::cout << "SEQuadX: " << dtemp << std::endl;
    foo->getSEQuadY(dtemp); std::cout << "SEQuadY: " << dtemp << std::endl;
    foo->getSWQuadX(dtemp); std::cout << "SWQuadX: " << dtemp << std::endl;
    foo->getSWQuadY(dtemp); std::cout << "NWQuadY: " << dtemp << std::endl;

    foo->getSecNWQuadX(dtemp); std::cout << "SecNWQuadX: " << dtemp 
					 << std::endl;
    foo->getSecNWQuadY(dtemp); std::cout << "SecNWQuadY: " << dtemp 
					 << std::endl;
    foo->getSecNEQuadX(dtemp); std::cout << "SecNEQuadX: " << dtemp 
					 << std::endl;
    foo->getSecNEQuadY(dtemp); std::cout << "SecNEQuadY: " << dtemp 
					 << std::endl;
    foo->getSecSEQuadX(dtemp); std::cout << "SecSEQuadX: " << dtemp 
					 << std::endl;
    foo->getSecSEQuadY(dtemp); std::cout << "SecSEQuadY: " << dtemp 
					 << std::endl;
    foo->getSecSWQuadX(dtemp); std::cout << "SecSWQuadX: " << dtemp 
					 << std::endl;
    foo->getSecSWQuadY(dtemp); std::cout << "SecSWQuadY: " << dtemp 
					 << std::endl;

    foo->getRMSE(ftemp); std::cout << "RMSE: " << ftemp << std::endl;
    foo->getImageSource(stemp); std::cout << "Image Source: " << stemp 
					  << std::endl;
    foo->getNumImageSources(itemp); std::cout << "Image Sources: " << itemp 
					      << std::endl;
    for (loopcount = 1; loopcount <= itemp; loopcount++)
    {
      foo->getSourceImageID(stemp, loopcount);
      std::cout << "SrcImID[" << loopcount << "]: " << stemp << std::endl;
      foo->getSourceImageDate(stemp, loopcount);
      std::cout << "SrcImDate[" << loopcount << "]: " << stemp << std::endl;
    }
    foo->getSourceDEMDate(stemp); std::cout << "DEM Date: " << stemp 
					    << std::endl;
    foo->getAgency(stemp); std::cout << "Agency: " << stemp << std::endl;
    foo->getProducer(stemp); std::cout << "Producer: " << stemp << std::endl;
    foo->getProductionSystem(stemp); std::cout << "Prod Sys: " << stemp 
					       << std::endl;
    foo->getCompression(stemp); std::cout << "Compression: " << stemp 
					  << std::endl;
    foo->getStandardVersion(stemp); std::cout << "Version: " << stemp 
					      << std::endl;
    foo->getMetaDataDate(stemp); std::cout << "MetaDate: " << stemp 
					   << std::endl;
    foo->getDataSize(litemp); std::cout << "Data Size: " << litemp 
					<< std::endl;
    foo->getByteCount(litemp); std::cout << "Byte Count: " << litemp 
					 << std::endl;
    std::cout << "Finished displaying" << std::endl;
    delete foo;
    return 0;
  }
  catch(...)
  {
    std::cerr << "An exception has occurred while getting the data. Exiting..."
	      << std::endl;
    delete foo;
    exit(-1);
  }
}

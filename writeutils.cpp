// $Id: writeutils.cpp,v 1.2 2002-06-21 03:09:01 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-06-21 03:09:01 $

// This is the implementation for the write functions of the ARC HDR file.
// Each DOQ format type has its own output function for clarity.

/* The contents of this file are subject to the Mozilla Public License
   Version 1.0 (the "License"); you may not use this file except in
   compliance with the License. You may obtain a copy of the License at
   http://www.mozilla.org/MPL/ */

#include "writeutils.h"
#include <ostream>
#include <string>

// **************************************************************************
// This function writes an ARC HDR file for a keyword formatted DOQ
bool writeKeywordHdr(USGSImageLib::DOQImageIFile* indoq, char* outfile,
                     bool useprimarydatum)
  throw()
{
  std::ofstream output;
  long int      litemp;
  float         ftemp;
  double        xorg, yorg;
  std::string   stemp;

  try
  {
    // No real reason why this was opening for ios::binary BGM 20 June 2002
    output.open(outfile, std::ios::out);
    if (output.fail())
    {
      output.close();
      return false;
    }

    output.setf(std::ios::fixed);
    output.precision(6);

    // Go ahead and output our lines
    indoq->getSamples(litemp);
    output << "ncols " << litemp << std::endl;

    indoq->getLines(litemp);
    output << "nrows " << litemp << std::endl;

    indoq->getHorizontalResolution(ftemp);
    output << "xdim " << ftemp << std::endl;
    output << "ydim " << ftemp << std::endl;

    if (useprimarydatum)
    {
      indoq->getXOrigin(xorg);
      indoq->getYOrigin(yorg);
    }
    else
    {
      indoq->getSecondaryXOrigin(xorg);
      indoq->getSecondaryYOrigin(yorg);
    }
    output << "ulxmap " << xorg << std::endl;
    output << "ulymap " << yorg << std::endl;

    indoq->getBandOrganization(stemp);
    if (stemp == std::string("SINGLE FILE"))
    {
      output << "layout BIL" << std::endl;
      output << "nbands 1" << std::endl;
    }
    else
    {
      output << "layout BIP" << std::endl;
      output << "nbands 3" << std::endl;
    }

    indoq->getByteCount(litemp);
    output << "skipbytes " << litemp << std::endl;

    output.close();
    return true;
  }
  catch (...)
  {
    output.close();
    return false;
  }
}


// **************************************************************************
// This function writes an ARC HDR file for a keyword formatted DOQ
bool writeRecordHdr(USGSImageLib::DOQ_RecImageIFile* indoq, char* outfile,
                    bool useprimarydatum)
  throw()
{
  std::ofstream output;
  long int      litemp, _width, _height, scanlinesize;
  int           itemp;
  float         ftemp;
  double        xorg, yorg, dtemp;
  std::string   stemp;

  try
  {
    // No reason here either why this should be ios::binary BGM 20 June 2002
    output.open(outfile, std::ios::out);
    if (output.fail())
    {
      output.close();
      return false;
    }

    output.setf(std::ios::fixed);
    output.precision(6);

    // Go ahead and output our lines
    indoq->getSamples(_width);
    output << "ncols " << _width << std::endl;

    indoq->getLines(_height);
    output << "nrows " << _height << std::endl;

    indoq->getPixelXRes(dtemp);
    output << "xdim " << dtemp << std::endl;
    indoq->getPixelYRes(dtemp);
    output << "ydim " << dtemp << std::endl;

    if (useprimarydatum)
    {
      indoq->getXOrigin(xorg);
      indoq->getYOrigin(yorg);
    }
    else
    {
      indoq->getSecondaryXOrigin(xorg);
      indoq->getSecondaryYOrigin(yorg);
    }
    output << "ulxmap " << xorg << std::endl;
    output << "ulymap " << yorg << std::endl;

    indoq->getBandTypes(itemp);
    if ( (itemp <= 4) && (itemp >= 1) )
    {
      scanlinesize = _width;
      output << "layout BIL" << std::endl;
      output << "nbands 1" << std::endl;
    }
    else if (itemp == 5)
    {
      scanlinesize = _width * 3;
      output << "layout BIP" << std::endl;
      output << "nbands 3" << std::endl;
    }
    else
    {
      output.close();
      return false; // can't handle any of the other types
    }

    output << "skipbytes " << scanlinesize << std::endl;

    output.close();
    return true;
  }
  catch (...)
  {
    output.close();
    return false;
  }
}

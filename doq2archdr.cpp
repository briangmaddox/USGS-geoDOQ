// $Id: doq2archdr.cpp,v 1.1.1.1 2002-02-20 16:25:09 bmaddox Exp $
// Brian Maddox - USGS MCMC - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-20 16:25:09 $
// This program parses the DOQ header and produces a file in arc's .hdr format

/* The contents of this file are subject to the Mozilla Public License
   Version 1.0 (the "License"); you may not use this file except in
   compliance with the License. You may obtain a copy of the License at
   http://www.mozilla.org/MPL/ */

#include <fstream>
#include <iostream>
#include <string>
#include "ImageLib/DOQImageIFile.h"
#include "MiscUtils/compare_nocase.h"

int main (int argc, char* argv[])
{
  USGSImageLib::DOQImageIFile* indoq = NULL;
  std::ofstream os;
  std::string   stemp;
  std::string   infile;
  long int      litemp;
  int           itemp;
  double        dtemp;
  float         ftemp;

  if (argc < 2)
  {
    std::cerr << "Usage: " << argv[0] << " keyword-doq-file headerfilename" 
	      << std::endl;
    exit(-1);
  }

  try
  {
    infile = std::string(argv[1]);
    if (!(indoq = new(std::nothrow) USGSImageLib::DOQImageIFile(infile)))
      throw std::bad_alloc();
    if (!indoq->good())
    {
      std::cerr << "Error opening DOQ file: " << infile << std::endl 
		<< "Exiting...." << std::endl;
      delete indoq;
      exit(-1);
    }

    os.open(argv[2], std::ios::out | std::ios::trunc);
    if (os.fail())
    {
      std::cerr << "Error opening header file: " << argv[2] << std::endl;
      std::cerr << "Exiting..." << std::endl;
      delete indoq;
      os.close();
      exit(-1);
    }

    os.setf(std::ios::fixed);
    os.precision(6);

    // Now go and output the file
    indoq->getSamples(litemp);
    os << "ncols " << litemp << std::endl;
    indoq->getLines(litemp);
    os << "nrows " << litemp << std::endl;
    indoq->getHorizontalResolution(ftemp);
    os << "xdim " << ftemp << std::endl;
    os << "ydim " << ftemp << std::endl;
    indoq->getXOrigin(dtemp);
    os << "ulxmap " << dtemp << std::endl;
    indoq->getYOrigin(dtemp);
    os << "ulymap " << dtemp << std::endl;

    indoq->getBandOrganization(stemp);
    if (MiscUtils::compare_nocase(stemp, "SINGLE FILE"))
    {
      os << "layout BIL" << std::endl;
      os << "nbands 1" << std::endl;
    }
    else
    {
      os << "layout BIP" << std::endl;
      os << "nbands 3" << std::endl;
    }

    indoq->getByteCount(litemp);
    os << "skipbytes " << litemp << std::endl;
  
    std::cout << argv[2] << " created." << std::endl;

    // close our files and close shop
    os.close();
    delete indoq;
    return 0;
  }
  catch (...)
  {
    std::cerr << "An exception has been thrown." << std::endl;
    delete indoq;
    os.close();
    exit(-1);
  }
}

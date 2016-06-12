// $Id: doqutils.cpp,v 1.2 2002-06-21 03:09:01 bmaddox Exp $
// Brian Maddox - USGS MCMC - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-06-21 03:09:01 $
// This file contains common functions needed by my various DOQ programs
   
/* The contents of this file are subject to the Mozilla Public License
   Version 1.0 (the "License"); you may not use this file except in
   compliance with the License. You may obtain a copy of the License at
   http://www.mozilla.org/MPL/ */

#ifndef DOQUTILS_CC
#define DOQUTILS_CC

#include <iostream>
#include <string>
//#include <stdio.h>
#include <fstream>
#include "getopt.h"
#include "ImageLib/ImageException.h"
#include "outputfuncs.h"
#include "doqutils.h"

// *************************************************************************
// This function takes in a keyword DOQ and writes out a ARC World file with
// filename major.tfw
bool makeKeywordTFW(USGSImageLib::DOQImageIFile* indoq, 
		    const std::string& basename,
                    const bool& usePrimaryDatum)
{
  float doqscale = 0.0; // to avoid making 2 function calls instead of one
  std::string tfwname;  // name for the world file
  double xorigin, yorigin;
  std::ofstream outfile;

  try
  {
#ifdef EDC
    tfwname = basename + std::string(".TFW"); // append the tfw extension to it
#else
    tfwname = basename + std::string(".tfw"); // append the tfw extension to it
#endif
    outfile.open(tfwname.c_str(), std::ios::out | std::ios::trunc);
    if (outfile.fail())
    {
      std::cerr << "Error writing ARC World file " << tfwname << std::endl;
      outfile.close();
      return false;
    }

    outfile.setf(std::ios::fixed);
    outfile.precision(5);
    indoq->getHorizontalResolution(doqscale);
    outfile << doqscale  << std::endl;
    outfile << "0.00000" << std::endl;
    outfile << "0.00000" << std::endl;
    outfile << ( -1.0 * doqscale) << std::endl;
    if (usePrimaryDatum)
    {
      indoq->getXOrigin(xorigin);
      indoq->getYOrigin(yorigin);
    }
    else
    {
      indoq->getSecondaryXOrigin(xorigin);
      indoq->getSecondaryYOrigin(yorigin);
    }
    outfile << xorigin << std::endl;
    outfile << yorigin << std::endl;

    // all done so wrap up and exit
    outfile.close();
    return true;
  }
  catch (...)
  {
    std::cerr << "Error writing ARC World file " << tfwname << std::endl;
    outfile.close();
    return false;    
  }
}

// *************************************************************************
// This outputs the native header.  
bool writeHeader(USGSImageLib::DOQImageIFile* doqin, 
		 const std::string& doqname, const std::string& basename)
{
  unsigned char* buffer = NULL;  // to hold the header
  long int bytecount;
  std::string headername;
  std::ofstream outfile;
  std::ifstream infile;

  try
  {
#ifdef EDC
    headername = basename + std::string(".HDR");
#else
    headername = basename + std::string(".hdr");
#endif
    outfile.open(headername.c_str(), std::ios::out | std::ios::trunc);
    if (outfile.fail())
    {
      std::cerr << "Error opening " << headername 
		<< " for output. Header not written!" << std::endl;
      outfile.close();
      return false;
    }

    infile.open(doqname.c_str(), std::ios::in);
    if (infile.fail())
    {
      std::cerr << "Error opening " << doqname 
		<< " for input.  Header not written!" << std::endl;
      infile.close();
      outfile.close();
      return false;
    }

    doqin->getByteCount(bytecount);
    if (!(buffer = new(std::nothrow) unsigned char[bytecount]))
      throw std::bad_alloc();

    infile.read(reinterpret_cast<char*>(buffer), bytecount);
    if (infile.fail())
    {
      std::cerr << "Error occurred while reading file.  Header not written!" 
		<< std::endl;
      infile.close();
      outfile.close();
      delete [] buffer;
      return false;
    }

    outfile.write(reinterpret_cast<char*>(buffer), bytecount);
    if (outfile.fail())
    {
      std::cerr << "Error occurred while writing header file!"
		<<  " File may be incorrect." << std::endl;
      infile.close();
      outfile.close();
      delete [] buffer;
      return false;
    }

    infile.close();
    outfile.close();
    delete [] buffer;
    return true;
  }
  catch (...)
  {
    std::cerr << "Error occurred while writing header file!"
	      <<  " File may be incorrect." << std::endl;
    infile.close();
    outfile.close();
    delete [] buffer;
    return false;
  } 
}
   

// ***************************************************************************
bool processArgs(bool& usePrimaryDatum, bool& makeTFW, bool& printHeader,
		 const std::string& myname, std::string& doqname,
		 std::string& basename, int& argc, char** argv) throw()
{
  int myopt;             // for getopt

  // Process any command line options
  while ((myopt = getopt(argc, argv, "VJ:Dpsth?")) != -1)
    switch (myopt)
    {
    case 'V': // Version switch
      {
	std::cout << argv[0] << " Version 4.0" << std::endl;
	return false;
	break;
      }
    case 'p': // primary datum switch
      {
	usePrimaryDatum = true;
	break;
      }
    case 's': // secondary datum
      {
	usePrimaryDatum = false;
	break;
      }
    case 't': // world file
      {
	makeTFW = true;
	break;
      }
    case 'h': // user wants a header
      {
	printHeader = true;
	break;
      }
    case '?': // user needs help (heh)
      {
	doq2geotiffusage(myname);
	return false;
	break;
      }
    default: // user did something silly
      {
	doq2geotiffusage(myname);
	return false;
	break;
      }
    }

  // No command line args left?  Didn't call it correctly
  if (optind >= argc)
  {
    doq2geotiffusage(myname);
    return false;
  }
  // This is a hack.  If you just send in a -, getopt doesn't process it like
  // you'd expect.  Basically, if the next argv element is NULL, the strcmp
  // would fail anyways.  BGM 1 June 1998
  if (argv[optind + 1] == NULL)
  {
    doq2geotiffusage(myname);
    return false;
  }
  
  // Now set our input and output names
  doqname  = std::string(argv[optind++]);
  basename = std::string(argv[optind]);

  return true;

}

#endif

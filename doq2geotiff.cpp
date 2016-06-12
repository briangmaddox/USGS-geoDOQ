// $Id: doq2geotiff.cpp,v 1.2 2002-06-21 03:09:01 bmaddox Exp $
// Brian Maddox - USGS MCMC - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-06-21 03:09:01 $

/* The contents of this file are subject to the Mozilla Public License
   Version 1.0 (the "License"); you may not use this file except in
   compliance with the License. You may obtain a copy of the License at
   http://www.mozilla.org/MPL/ */

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <ctime>
#include <fstream>
#include <getopt.h>
#include "ImageLib/DOQImageIFile.h"
#include "ImageLib/GeoTIFFImageOFile.h"
#include "MiscUtils/SpinnerThingy.h"
#include "doqutils.h"
#include "spcsutils.h"
#include "outputfuncs.h"
#include "MiscUtils/compare_nocase.h"

#ifdef _WIN32
#pragma warning( disable : 4291 )  // Disable warning messages
#endif

const std::string MYVERSION("4.01");

const std::string SOFTWARE = 
  "USGS DOQ to GeoTIFF conversion program version " + MYVERSION;

//**************************************************************************
int main (int argc, char* argv[])
{
  bool usePrimaryDatum = true;   // whether or not to use the primary or
                                 // secondary datum 
  bool printHeader = false;      // self-evident
  bool makeTFW = false;          // whether or not to make the world file
  MiscUtils::SpinnerThingy thingy;                // Status spinner
  USGSImageLib::DOQImageIFile* doqin      = NULL; // Input DOQ Pointer
  USGSImageLib::GeoTIFFImageOFile* geoout = NULL; // Output GeoTIFF Pointer  
  unsigned char* lineu = NULL;       // pointer to the scanline buffer
  std::string desc_string;           // Description String
  std::string time_string;           // time string
  std::string _tname;                // Temp String to hold the name
  std::string _tstate;               // temp string to hold the state name
  std::string _tquadrant;            // "" "" Quadrant 
  std::string _tdate;                // "" "" Date
  std::string timebuff;              // To hold the output time
  int _tyear, _tmonth, _tday;        // temp date vars
  int tDatYear = 0;                  // etc
  std::string _datum;                // Datum string
  std::string footemp;               // Generic string pointer
  long int doqlines, doqsamples;
  std::string basename;              // to hold the base filename
  std::string gtiffile;              // to hold the filename of the GeoTIFF
  std::string doqname;               // hold the filename of the DOQ
  std::string coordsystem;           // To figure out if UTM or SPCS
  unsigned short whichunits;         // unit type on the DOQ
  int myBPS, SPP;                    // Bits/Sample and Samples/Pixel
  float horizres;                    // horizontal resolution
  double xorigin, yorigin;           // origins of the DOQ
  int coordzone;                     // coordinate zone
  std::string myname(argv[0]);       // program's name
  int photometric;                   // for opening the GeoTIFF
  std::stringstream tempstream;      // for formatting text

  
  if (argc <= 1) // if they just call it with no arguments
  {
    doq2geotiffusage(myname);
    exit(-1);
  }
  
  // If we're under Win32, display the warning statement as Win32 has 
  // absolutely no support
#ifdef _WIN32
  displayWin32Info(myname);
#endif

  try
  {
    if (!processArgs(usePrimaryDatum, makeTFW, printHeader, myname, doqname,
		     basename, argc, argv))
      exit(-1);

#ifdef EDC
    gtiffile = basename + std::string(".TIF");
#else
    gtiffile = basename + std::string(".tif");
#endif
    // added so users can't overwrite the input file.   BGM 20 Nov 1997
    // Changed the error message a small bit.  BGM 22 April 1998
    if (MiscUtils::compare_nocase(doqname, gtiffile))
    {
      std::cerr << "Error! Input and output filenames do not differ. Exiting.."
                << std::endl;
      exit(-1);
    }
    
    // Attempt to open files here
    // Open the DOQ and check to make sure everything is OK
    if (!(doqin = new(std::nothrow) USGSImageLib::DOQImageIFile(doqname)))
      throw std::bad_alloc();
    if (!doqin->good())
    {
      std::cerr << "Cannot open " << doqname << ". Exiting..." << std::endl;
      delete doqin;
      exit(-1);
    }
    
    doqin->getBandOrganization(footemp); // so we know how to handle later
    // Check to make sure this is a DOQ we can handle BGM 2 Apr 1998
    // Added support for BIL and BSQ DOQs BGM 13 Oct 1999
    if ( (!MiscUtils::compare_nocase(footemp, "SINGLE FILE"))
	 && (!MiscUtils::compare_nocase(footemp, "BIP")) 
	 && (!MiscUtils::compare_nocase(footemp, "BIL"))
	 && (!MiscUtils::compare_nocase(footemp, "BSQ")) )
    {
      std::cerr << "Unhandled format of " << footemp 
                << " encountered. Exiting..." << std::endl;
      delete doqin;
      exit(-1);
    }

    // Get our info from the DOQ
    doqin->getLines(doqlines);
    doqin->getSamples(doqsamples);
    doqin->getBitsPerSample(myBPS);
    doqin->getSamplesPerPixel(SPP);
    if (usePrimaryDatum)
    {
      doqin->getXOrigin(xorigin);
      doqin->getYOrigin(yorigin);
    }
    else
    {
      doqin->getSecondaryXOrigin(xorigin);
      doqin->getSecondaryYOrigin(yorigin);
    }
    doqin->getHorizontalResolution(horizres);
    doqin->getCoordinateZone(coordzone);
    doqin->getHorizontalCoordinateSystem(coordsystem);
    // Make sure the SPCS stuff is OK and also set some stuff up
    if (MiscUtils::compare_nocase(coordsystem, "SPCS"))
    {
      if (!validateSPCS(doqin, whichunits, usePrimaryDatum))
      {
      	delete doqin;
	exit(-1);
      }
    }
    
  
    // Open the GeoTIFF and make sure everything is OK
    // Changed this.  If it's single file, do grey otherwise use RGB for
    // everything else.  This works since it has to be a supported format it
    // it to get this far BGM 13 Oct 1999
    if (MiscUtils::compare_nocase(footemp, "SINGLE FILE"))
      photometric = PHOTOMETRIC_MINISBLACK;
    else
      photometric = PHOTOMETRIC_RGB;
    
    if (!(geoout = new(std::nothrow) 
	  USGSImageLib::GeoTIFFImageOFile(gtiffile, doqsamples, doqlines,
					  photometric)))
      throw std::bad_alloc();
    if (!geoout->good())
      throw USGSImageLib::ImageException(IMAGE_FILE_OPEN_ERR);

    geoout->setSubfileType(0);
    geoout->setBitsPerSample(myBPS);
    geoout->setCompression(COMPRESSION_NONE);
    geoout->setSamplesPerPixel(SPP);
    geoout->setRowsPerStrip(1);
    geoout->setPlanarConfig(PLANARCONFIG_CONTIG);
    geoout->setResUnits(RESUNIT_CENTIMETER);

    // Make any user specified files as needed
    // Output the header data to the text file if the user requested
    if (printHeader)
    {
      std::cout << "Writing header data... ";
      if (writeHeader(doqin, doqname, basename))
	std::cout << "Finished." << std::endl;
      else
	std::cout << "Failed!" << std::endl;
    }
    
    // Output the world file if the user requested
    if (makeTFW)
    {
      std::cout << "Writing ARC World file... ";
      if (makeKeywordTFW(doqin, basename, usePrimaryDatum))
      	std::cout << "Finished." << std::endl;
      else
      	std::cout << "Failed!" << std::endl;
    }
    
    
    // Set up which datum we're going to use
    if (usePrimaryDatum)
      doqin->getHorizontalDatum(_datum);
    else
      doqin->getSecondaryHorizontalDatum(_datum);
    
    // Changed this logic around a bit. Most DOQ's are being projected in NAD83
    // so we're going to use that as a default BGM 23 April 1998
    if (_datum == std::string("")) // OOPS, didn't find a datum in the DOQ.  
    {
      std::cerr << "Datum not found.  Assuming NAD83" << std::endl;
      tDatYear = 1983;
    }
    else
    {
      if (MiscUtils::compare_nocase(_datum, "NAD27"))
      	tDatYear = 1927;
      else if (MiscUtils::compare_nocase(_datum, "NAD83"))
      	tDatYear = 1983;
      else
      { 
      	std::cerr << "Unsupported datum of " << _datum 
                  << ". Using NAD 83" << std::endl;
      	tDatYear = 1983;
      }
    }
    

    // Get the production date of the DOQ and encode it into a GeoTIFF date
    // string
    doqin->getProdDate(_tdate);
    tempstream << _tdate << std::ends; // so we can parse out of it later
    tempstream >> _tyear >> _tmonth >> _tday;

    //clear the stream and setup the time string CBB 6/28/00
    tempstream.str("");
    tempstream.clear();

    tempstream << std::setw(4) << _tyear << ":" << std::setfill('0') 
               << std::setw(2) << _tmonth << ":" 
               << std::setw(2) << _tday << " 00:00:00" << std::ends;
    
    //get the time string
    time_string = tempstream.str();

    // Now clear out the stream for later use
    tempstream.str("");
    tempstream.clear();


    // Set the quad name
    doqin->getQuadName(_tname);
    if (_tname == std::string(""))
    {
      std::cerr << "No quad name.  Assuming name of NONAME DOQ" << std::endl;
      _tname = std::string("NONAME DOQ");
    }

    // find the state
    doqin->getState(_tstate, 1);

    // Fill the description strings
    doqin->getQuadrant(_tquadrant);
    if (_tquadrant == std::string("")) // Must be a full Quad
    {
      if (_tstate == std::string(""))
	tempstream << "USGS GeoTIFF Digital Orthoimage " << _tname
		   << "." << std::ends;
      else
	tempstream << "USGS GeoTIFF Digital Orthoimage " << _tname << ", "
		   << _tstate << "." << std::ends;
      geoout->setXResolution(240.0);
      geoout->setYResolution(240.0);
    }
    else // It's a quarter quad, assuming 12000 scale
    {
      if (_tstate == std::string(""))
	tempstream << "USGS GeoTIFF Digital Orthoimage " << _tname << " "
		   << _tquadrant << "." << std::ends;
      else
	tempstream << "USGS GeoTIFF Digital Orthoimage " << _tname << " "
		   << _tquadrant << ", " << _tstate << "." << std::ends;
      geoout->setXResolution(120.0);
      geoout->setYResolution(120.0);
    }
    
    desc_string = tempstream.str();
    tempstream.str("");
    tempstream.clear();

    // Set the GeoTIFF and TIFF tags
    geoout->setDateTime(time_string);
    geoout->setSoftware(SOFTWARE);
    geoout->setImageDescription(desc_string);
    
    if (MiscUtils::compare_nocase(coordsystem, "UTM"))
      geoout->setUSGS_UTM(xorigin, yorigin, horizres, horizres,
                          coordzone, tDatYear);
    else
    {
      geoout->setUSGS_SPCS(xorigin, yorigin, horizres, horizres,
                           coordzone, tDatYear);
      // Now set the units since we're in SPCS
      geoout->setGeoKey(GeogLinearUnitsGeoKey, whichunits);
      if (whichunits == 32767) // Define Intl Feet in terms of meters
      	geoout->setGeoKey(GeogLinearUnitSizeGeoKey, 0.3048);
    }

    
    if (!(lineu = new(std::nothrow) unsigned char[SPP * doqsamples]))
      throw std::bad_alloc();
    // Go do the actual conversion
    std::cout << "Converting " << doqlines << " scanlines to GeoTIFF..." 
	      << std::endl;
    for (int j=0; j < doqlines; j++)
    {
      if ((j % 128) == 0)
      	thingy.update(j); // Update the status spinner
      
      doqin->getRawScanline(j, lineu);
      geoout->putRawScanline(j, lineu);
    }
    
    thingy.done("Done.");
  
    delete doqin;
    delete geoout;
    delete [] lineu;
    return 0;
  }
  catch (...)
  {
    std::cerr << "An error has occurred while processing the DOQ "
              << ".  Exiting..." << std::endl;
    delete doqin;
    delete geoout;
    delete [] lineu;
    exit(-1);
  }
}

#ifdef _WIN32
#pragma warning( default : 4291 )  // Disable warning messages
#endif

// $Id: doq2jpeg.cpp,v 1.2 2002-06-21 03:09:01 bmaddox Exp $
// doq2jpeg
// Brian Maddox - USGS MCMC - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-06-21 03:09:01 $
// This program converts native format DOQ's to JFIF DOQ's for MCMC's
// county DOQ production
   
/* The contents of this file are subject to the Mozilla Public License
   Version 1.0 (the "License"); you may not use this file except in
   compliance with the License. You may obtain a copy of the License at
   http://www.mozilla.org/MPL/ */

#include "ImageLib/DOQImageIFile.h"
#include "ImageLib/RGBPalette.h"
#include "ImageLib/RGBPixel.h"
#include "MiscUtils/SpinnerThingy.h"
#include "MiscUtils/compare_nocase.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <fstream>
#include <getopt.h>

extern "C"
{
#include "jpeglib.h"
#include "jerror.h"
}

void usage(char* myname)
{
  std::cerr << "Usage: " << myname << " doq-file jpeg-file" << std::endl;
  std::cerr << "Where options are: " << std::endl;
  std::cerr << "  -g     always use greyscale as the output colorspace" 
	    << std::endl;
}

//**************************************************************************
int main (int argc, char* argv[])
{
  int myopt;           // for getopt
  // in case the luser wants everything in greyscale
  bool usegreyalways = false;
  MiscUtils::SpinnerThingy thingy;
  USGSImageLib::DOQImageIFile* doqin;
  unsigned char* lineu = NULL;
  long int doqlines, doqsamples;
  std::string bandorg;
  std::string myname;
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  FILE* outfile;
  JSAMPROW row[1];
  int SPP;

  try
  {
    if (argc == 1) // if they just call it with no arguments
    {
      usage(argv[0]);
      exit(-1);
    }
    
    while ((myopt = getopt(argc, argv, "g")) != -1)
      switch (myopt)
      {
      case 'g':
	usegreyalways = true;
	break;
      default:
	usage(argv[0]);
	exit(-1);
	break; // making the compilers happy...
      }
    
    // No command line args left?  Didn't call it correctly
    if (optind >= argc)
    {
      usage(argv[0]);
      exit(-1);
    }
    
    // This is a hack.  If you just send in a -, getopt doesn't process it like
    // you'd expect.  Basically, if the next argv element is NULL, the strcmp
    // would fail anyways.  BGM 1 June 1998
    if (argv[optind + 1] == NULL)
    {
      usage(argv[0]);
      exit(-1);
    }
    
    // added so users can't overwrite the input file... yep.   BGM 20 Nov 1997
    if (strcmp(argv[optind], argv[optind + 1]) == 0)
    {
      std::cerr << argv[0] << ": Input and output filenames do not differ. "
		<< "Exiting..." << std::endl;
      exit(-1);
    }
    
    myname = std::string(argv[optind++]);
    // Attempt to open files here
    // Open the DOQ and check to make sure everything is OK
    if (!(doqin = new(std::nothrow) USGSImageLib::DOQImageIFile(myname)))
      throw std::bad_alloc();
    if (!doqin->good())
    {
      std::cerr << "Cannot open " << argv[optind - 1] << std::endl;
      delete doqin;
      exit(-1);
    }

    doqin->getBandOrganization(bandorg);
    if (!MiscUtils::compare_nocase(bandorg, "SINGLE FILE") &&
	!MiscUtils::compare_nocase(bandorg, "BIP") &&
	!MiscUtils::compare_nocase(bandorg, "BIL") &&
	!MiscUtils::compare_nocase(bandorg, "BSQ"))
    {
      std::cerr << "Error! Unknown DOQ format encountered." << std::endl 
		<< "Exiting..." << std::endl;
      delete doqin;
      exit(-1);
    }

    doqin->getLines(doqlines);
    doqin->getSamples(doqsamples);

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    if ( (outfile = fopen(argv[optind], "wb")) == NULL)
    {
      std::cerr << "Error opening " << argv[optind] << ". Exiting..." 
		<< std::endl;
      delete doqin;
      jpeg_destroy_compress(&cinfo);
      exit(-1);
    }
    jpeg_stdio_dest(&cinfo, outfile);

    // set up the JPEG fields
    cinfo.image_width = doqsamples;
    cinfo.image_height = doqlines;
    if (MiscUtils::compare_nocase(bandorg, "SINGLE FILE"))
    {
      SPP = 1;
      cinfo.input_components = 1;
      cinfo.in_color_space = JCS_GRAYSCALE;
    }
    else // if we get here, it has to be color and in a format we support
    {
      SPP = 3;
      cinfo.input_components = 3;
      cinfo.in_color_space = JCS_RGB;
    }

    // set the defaults
    jpeg_set_defaults(&cinfo);
    if (usegreyalways)
    {
      jpeg_set_colorspace(&cinfo, JCS_GRAYSCALE);
    }
    jpeg_set_quality(&cinfo, 30, TRUE);
    
    jpeg_start_compress(&cinfo, TRUE);
    
    if (!(lineu = new(std::nothrow) unsigned char[SPP * doqsamples]))
      throw std::bad_alloc();
   
 // Go do the actual conversion
    std::cout << "Converting " << doqlines << " scanlines to JFIF..." 
	      << std::endl;
    for (int j = 0; j < doqlines; j++)
    {
      if ((j % 128) == 0)
	thingy.update(j); // Update the status spinner
      
      doqin->getRawScanline(j, lineu);
      row[0] = lineu;
      (void) jpeg_write_scanlines(&cinfo, row, 1);
    }
    
    jpeg_finish_compress(&cinfo);
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);
    

    thingy.done("Done.");
  
    delete doqin;
    delete [] lineu;

    return 0;
  }
  catch (...)
  {
    std::cerr << "An error has occurred while converting.  Exiting..."
	      << std::endl;
    delete [] lineu;
    delete doqin;
    jpeg_finish_compress(&cinfo);
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);
    return -1;
  }
}

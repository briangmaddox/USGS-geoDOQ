// $Id: bip2bil.cpp,v 1.2 2002-06-21 03:09:01 bmaddox Exp $
// Brian Maddox   USGS MCMC  3 Nov 1997
// Last modified by $Author: bmaddox $ on $Date: 2002-06-21 03:09:01 $
// This program takes in a BIP DOQ as input and outputs a BIL format DOQ. 
// The whole purpose of this is to create a BIL DOQ so I can do some testing.

/* The contents of this file are subject to the Mozilla Public License
   Version 1.0 (the "License"); you may not use this file except in
   compliance with the License. You may obtain a copy of the License at
   http://www.mozilla.org/MPL/ */

#include <iostream>
#include <fstream>
#include <cstring>
#include "ImageLib/DOQImageIFile.h"
#include "MiscUtils/compare_nocase.h"
#include "MiscUtils/SpinnerThingy.h"

int main (int argc, char* argv[])
{
  if (argc != 3)
  {
    std::cerr << "Usage: " << argv[0] << " indoq outbil" << std::endl;
    exit(-1);
  }

  USGSImageLib::DOQImageIFile* indoq;
  MiscUtils::SpinnerThingy thingy;
  std::ifstream infile;
  std::ofstream outfile;
  long int samples, lines, bytecount;
  std::string temp;
  char* headbuffer;
  unsigned char* inbuffer;  // holds the read-in scanline from the BIP
  unsigned char* red;       // red pixels for the BIL output
  unsigned char* green;     // green ""    "" ""
  unsigned char* blue;      // blue  ""    "" ""
  char* position;           // position in the buffers
  std::string myname(argv[1]);

  try
  {
    if (!(indoq = new(std::nothrow) USGSImageLib::DOQImageIFile(myname)))
      throw std::bad_alloc();
    if (!indoq->good())
    {
      std::cerr << "Error reading " << argv[1] << ". Exiting...." << std::endl;
      delete indoq;
      exit(-1);
    }

    indoq->getBandOrganization(temp);
    if (!MiscUtils::compare_nocase(temp, "BIP"))
    {
      std::cerr << "Error:  This program only accepts DOQ's in the BIP format."
		<< std::endl << "Exiting...." << std::endl;
      delete indoq;
      exit(-1);
    }
    
    // Grab the width and height so we'll know how big
    indoq->getSamples(samples);
    indoq->getLines(lines);
    indoq->getByteCount(bytecount);
    
    delete indoq; // close it since we have the data we need.

    infile.open(argv[1], std::ios::in);
    if (infile.fail())
    {
      std::cerr << "Error opening " << argv[1] << " for input.  Exiting...." 
		<< std::endl;
      infile.close();
      exit(-1);
    }

    outfile.open(argv[2], std::ios::out);
    if (outfile.fail())
    {
      std::cerr << "Error opening " << argv[2] << " for output.  Exiting...." 
		<< std::endl;
      infile.close();
      outfile.close();
      exit(-1);
    }

    if (!(headbuffer = new(std::nothrow) char[3 * samples])) // just for header
      throw std::bad_alloc();
    if (!(inbuffer = new(std::nothrow) unsigned char[3 * samples]))
      throw std::bad_alloc();  // allocate space for read-in

    // allocate space for the output buffers
    if (!(red = new(std::nothrow) unsigned char[samples]))
      throw std::bad_alloc();
    if (!(green = new(std::nothrow) unsigned char[samples]))
      throw std::bad_alloc();
    if (!(blue = new(std::nothrow) unsigned char[samples]))
      throw std::bad_alloc();
    
    infile.read(headbuffer, (3 * samples)); // grab the header
  
    // This is admittedly cheesy.  Read in the header, and go find the BIP
    // key.  Once found, change it to the BIL keyword.

    if ( (position = std::strstr(headbuffer, "BIP")) == NULL)
    { // should be there, but not.  something bad happened
      std::cerr << "Error.  Can't find BIP key.  Exiting...." << std::endl;
      delete [] headbuffer;
      delete [] red;
      delete [] green;
      delete [] blue;
      delete [] inbuffer;
      infile.close();
      outfile.close();
      exit(-1);
    }

    position += 2; // go to the P in BIP
    *position = 'L'; // change it to an L
    outfile.write(headbuffer, (3 * samples)); // write out the header

    delete [] headbuffer; // don't need this anymore

    std::cout << "Converting " << lines << " to BIL format....." << std::endl;
    // Now, loop through the file
    for (int counter = 1; counter <= lines; counter++)
    {
      if ((counter % 128) == 0)
	thingy.update(counter);
      infile.read(reinterpret_cast<char*>(inbuffer), 
                  (3 * samples)); // read in the line
      for (int linepos = 0; linepos < samples; linepos++)
      {
	red[linepos] = inbuffer[3 * linepos];
	green[linepos] = inbuffer[3 * linepos + 1];
	blue[linepos] = inbuffer[3 * linepos + 2];
      }
     
      // Now output the red, green, and blue bands
      outfile.write(reinterpret_cast<char*>(red), samples);
      outfile.write(reinterpret_cast<char*>(green), samples);
      outfile.write(reinterpret_cast<char*>(blue), samples);
    }
    
    thingy.done("Done.");
    
    infile.close();
    outfile.close();
    delete [] inbuffer;
    delete [] red;
    delete [] green;
    delete [] blue;

    return 0;
  }
  catch (...)
  {
    std::cerr << "An error has occurred while processing.  Exiting...."
	      << std::endl;
    infile.close();
    outfile.close();
    delete [] inbuffer;
    delete [] headbuffer;
    delete [] red;
    delete [] green;
    delete [] blue;
    return (-1);
  }
}

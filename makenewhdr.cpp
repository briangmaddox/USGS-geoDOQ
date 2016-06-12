// $Id: makenewhdr.cpp,v 1.2 2002-06-21 03:09:01 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-06-21 03:09:01 $
// This program will dump a copy of the header of a keyword format DOQ to the
// user supplied filename.

/* The contents of this file are subject to the Mozilla Public License
   Version 1.0 (the "License"); you may not use this file except in
   compliance with the License. You may obtain a copy of the License at
   http://www.mozilla.org/MPL/ */

#include <fstream>
#include <iostream>
#include <string>
#include "ImageLib/DOQImageIFile.h"

int main (int argc, char* argv[])
{
  long int headerlength = 0;     // Length of the Keyword header
  unsigned char* buffer = NULL;  // Hold the keyword
  USGSImageLib::DOQImageIFile* indoq = NULL;   // DOQ pointer
  std::string inname(argv[1]);
  std::ifstream infile;
  std::ofstream outfile;

  if (argc < 3)
  {
    std::cerr << "Usage: " << argv[0] << " keyword-doq header-file" 
	      << std::endl;
    exit(0);
  }

  try
  {
    if (!(indoq = new(std::nothrow) USGSImageLib::DOQImageIFile(inname)))
      throw std::bad_alloc();
    if (!indoq->good())
    {
      std::cerr << "Error opening " << argv[1] << ". Exiting..." << std::endl;
      delete indoq;
      exit(-1);
    }

    indoq->getByteCount(headerlength); // store it
    delete indoq; // Need to delete so we can get the header

    outfile.open(argv[2], std::ios::out);
    if (outfile.fail())
    {
      std::cerr << "Error opening " << argv[2] << " for output.  Exiting..." 
		<< std::endl;
      outfile.close();
      exit(-1);
    }


    infile.open(argv[1], std::ios::in);
    if (infile.fail())
    {
      std::cerr << "Error opening " << argv[1] << " for input.  Exiting..." 
		<< std::endl;
      infile.close();
      outfile.close();
      exit(-1);
    }

    if (!(buffer = new(std::nothrow) unsigned char[headerlength]))
      throw std::bad_alloc();

    infile.read(reinterpret_cast<char*>(buffer), headerlength);
    if (infile.fail()) // Error reading data
    {
      std::cerr << "Error occurred while reading file.  Exiting..." 
		<< std::endl;
      delete [] buffer;
      infile.close();
      outfile.close();
      exit(-1);
    }

    outfile.write(reinterpret_cast<char*>(buffer), headerlength);
    if (outfile.fail()) // error occurred while writing this
    {
      std::cerr << "Error occurred while writing file.  Exiting..." 
		<< std::endl;
      infile.close();
      outfile.close();
      delete [] buffer;
      exit(-1);
    }
    
    infile.close();
    outfile.close();
    delete [] buffer;
    
    return 0;
  }
  catch (...)
  {
    std::cerr << "An error has occurred while processing.  Exiting..."
	      << std::endl;
    delete [] buffer;
    infile.close();
    outfile.close();
    return -1;
  }
}

#include "ADCFile.hh"

ADCFile::ADCFile()
{
  SetPath("");
}

ADCFile::ADCFile(std::string path)
{
  SetPath(path);
}

ADCFile::~ADCFile()
{;}

#include "TrigFile.hh"

TrigFile::TrigFile()
{
  SetPath("");
}

TrigFile::TrigFile(std::string path)
{
  SetPath(path);
}

TrigFile::~TrigFile()
{;}

#include "SACSimpleClusterization.hh"
#include <math.h>

Int_t SACSimpleClusterization::IsSeedNeig(Int_t seedID, Int_t cellID) {

  Int_t dChIDmax=fClusterDcellMax;
  //uses cellID to find neig cells wrt seed of the cluster
  Int_t IsNeig=-1;
  Int_t SeedRow=seedID/10;
  Int_t SeedCol=seedID%10;

  Int_t CellRow=cellID/10;
  Int_t CellCol=cellID%10;
  //excludes the seed cell 
  if( abs(SeedCol-CellCol)<=dChIDmax && abs(SeedRow-CellRow)<=dChIDmax ) 
    IsNeig = (Int_t)std::min(abs(SeedCol-CellCol),abs(SeedRow-CellRow)); 
  //  std::cout<<"seedID "<<seedID<<" cellID "<<cellID<<" Is Neig "<<IsNeig<<std::endl;
  return IsNeig;
}


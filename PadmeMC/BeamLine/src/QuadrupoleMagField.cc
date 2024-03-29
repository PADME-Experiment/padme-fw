//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: QuadrupoleMagField.cc 69786 2013-05-15 09:38:51Z gcosmo $
//
// -------------------------------------------------------------------

#include "QuadrupoleMagField.hh" //M Raggi 
#include "G4RotationMatrix.hh"
#include "G4SystemOfUnits.hh"

static G4RotationMatrix IdentityMatrix; 

QuadrupoleMagField::QuadrupoleMagField(G4double pGradient)
{
  G4cout<<" Creating quadrupole "<<G4endl;
  fGradient   = pGradient ;
  fOrigin     = G4ThreeVector( 0.0, 0.0, 0.0) ;
  fpMatrix    = &IdentityMatrix;
}


QuadrupoleMagField::QuadrupoleMagField(G4double pGradient, G4ThreeVector
					   pOrigin, G4RotationMatrix* pMatrix)
{
  fGradient    = pGradient ;
  fOrigin      = pOrigin ;
  fpMatrix     = pMatrix ;
}


QuadrupoleMagField::~QuadrupoleMagField()
{
}

//  Allow displaced origin and rotation 
//  Extensions by Bjorn Riese (GSI)

void QuadrupoleMagField::GetFieldValue( const G4double y[7],G4double B[3]) const  
{  
  G4ThreeVector r_global = G4ThreeVector(
					 y[0] - fOrigin.x(), 
					 y[1] - fOrigin.y(), 
					 y[2] - fOrigin.z());  
//  G4ThreeVector r_local = G4ThreeVector(
//					fpMatrix->colX() * r_global,
//					fpMatrix->colY() * r_global,
//					fpMatrix->colZ() * r_global);

 G4ThreeVector r_local = G4ThreeVector(
				       fpMatrix->rowX() * r_global,
				       fpMatrix->rowY() * r_global,
				       fpMatrix->rowZ() * r_global);

  G4ThreeVector B_local = G4ThreeVector(
					fGradient * r_local.y(),
					fGradient * r_local.x(),
					//fGradient * r_local.z(), //bad fix M. Raggi 03/2021
					0);
  
//  G4ThreeVector B_global = G4ThreeVector(
//					 fpMatrix->inverse().colX() * B_local,
//					 fpMatrix->inverse().colY() * B_local,
//					 fpMatrix->inverse().colZ() * B_local);  

  G4ThreeVector B_global = G4ThreeVector(
					 fpMatrix->inverse().rowX() * B_local,
					 fpMatrix->inverse().rowY() * B_local,
					 fpMatrix->inverse().rowZ() * B_local);  

  B[0] = B_global.x() ;
  B[1] = B_global.y() ;
  B[2] = B_global.z() ;

//  printf("B value           x: %f y: %f z: %f \n",B[0]/tesla,B[1]/tesla,B[2]/tesla);
//  printf("YYY value         %f %f %f \n",y[0],y[1],y[2]);
//  printf("orig value        %f %f %f \n",fOrigin.x(),fOrigin.y(),fOrigin.z());  
//  printf("XYZ global value  x:%f  y: %f z: %f \n",r_global.x(),r_global.y(),r_global.z());
//  printf("XYZ local value   %f %f %f \n",r_local.x(),r_local.y(),r_local.z());
//  
//  G4cout<<"matrix value  "<<fpMatrix->rowX()<<G4endl;
//  G4cout<<"matrix value  "<<fpMatrix->rowY()<<G4endl;
//  G4cout<<"matrix value  "<<fpMatrix->rowZ()<<G4endl;

  //  printf("fGradient Tesla/m %f \n",fGradient*m/tesla);
}

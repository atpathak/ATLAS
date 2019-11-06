#include "QFramework/TQImportLink.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

using std::cout;
using std::endl;

////////////////////////////////////////////////////////////////////////////////////////////////
//
// TQImportLink:
//
// < put your class description here >
//
////////////////////////////////////////////////////////////////////////////////////////////////

ClassImp(TQImportLink)


//__________________________________________________________________________________|___________

TQImportLink::TQImportLink() : TQLink() {
  // Default constructor of TQImportLink class:

  fImportPath.Clear();
}


//__________________________________________________________________________________|___________

TQImportLink::TQImportLink(TString name) : TQLink(name) {
  // Default constructor of TQImportLink class:

}


//__________________________________________________________________________________|___________

TQImportLink::TQImportLink(TString name, TString importPath) : TQLink(name) {
  // Default constructor of TQImportLink class:

  fImportPath = importPath;
}


//__________________________________________________________________________________|___________

TString TQImportLink::getImportPath() {

  return fImportPath;
}


//__________________________________________________________________________________|___________

TString TQImportLink::getDestAsString() {

  return getImportPath();
}


//__________________________________________________________________________________|___________

TQImportLink::~TQImportLink() {
  // Destructor of TQImportLink class:

}



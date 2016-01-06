/**
 * Gamma Combination
 * Author: Maximilian Schlupp, max.schlupp@cern.ch
 * Date: November 2013
 *
 **/

#ifndef PDF_GausPlusExp_h
#define PDF_GausPlusExp_h

#include "PDF_Generic_Abs.h"
#include "RooAddPdf.h"
 
using namespace RooFit;
using namespace std;
using namespace Utils;

class PDF_GausPlusExp : public PDF_Generic_Abs
{
public:
  PDF_GausPlusExp(RooWorkspace* w, OptParser* opt);
  RooFitResult* fit(bool fitToys = kTRUE);
  void          generateToys(int SeedShift = 0);
  inline void   setNToys(int nToys){toysToGenerate = nToys;};
  inline int    getNToys(){return toysToGenerate;};
  virtual void  randomizeConstraintObservables(bool useConstrPdf = true);

  ~PDF_GausPlusExp();
  protected: 
    int     toysToGenerate; //> number of toys to generate
  private:
    bool    drawFitsDebug;  //> for visualizing toys and fit results, only changeable in the code
};

#endif

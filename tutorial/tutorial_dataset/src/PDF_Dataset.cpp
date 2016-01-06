#include "PDF_GausPlusExp.h"
#include "RooExponential.h"

PDF_GausPlusExp::PDF_GausPlusExp(RooWorkspace* w, OptParser* opt)
: PDF_Generic_Abs(w,1,opt)
{
  name    = "GausPlusExp";
  title   = "GausPlusExp";
  data = (RooDataSet*)wspc->data("data"); //> set real Dataset 
  if(data){
    isDataSet = kTRUE;
    std::cout << "INFO in PDF_GausPlusExp::PDF_GausPlusExp -- Dataset initialized" << std::endl;
  }
  else{
    std::cout << "FATAL in PDF_GausPlusExp::PDF_GausPlusExp -- no Dataset found in worspace!" << std::endl;
    isDataSet = kFALSE;
  }
  this->setNToys(0);
  drawFitsDebug = kFALSE;
}
PDF_GausPlusExp::~PDF_GausPlusExp(){};

RooFitResult* PDF_GausPlusExp::fit(bool fitToys){
  if(this->notSetupToFit(fitToys)){
    std::cout << "FATAL in PDF_GausPlusExp::fit -- There is no PDF or (toy)data set to fit!" << std::endl;  
    return NULL;
  }
  // Turn off RooMsg
  RooMsgService::instance().setGlobalKillBelow(ERROR);
  RooMsgService::instance().setSilentMode(kTRUE);
  // Choose Dataset to fit to
  RooDataSet* dataToFit = (fitToys) ? this->toyObservables : this->data ;
  // fit pdf, there is no sense in using HESSE or MINOS
  RooFitResult* result  = pdf->fitTo( *dataToFit, RooFit::Save() 
                                      ,RooFit::Minos(kFALSE)
                                      ,RooFit::Hesse(kFALSE)
                                      ,RooFit::Strategy(3)
                                      ,RooFit::Minimizer("Minuit2","minimize")
                                      ); 

  RooMsgService::instance().setSilentMode(kFALSE);
  RooMsgService::instance().setGlobalKillBelow(INFO);
  this->fitStatus = result->status();
  return result;
};

void   PDF_GausPlusExp::generateToys(int SeedShift){
  TRandom3 rndm(0);
  if(this->getNToys()==0){
    std::cout << "FATAL in PDF_GausPlusExp::generateToys -- I am supposed to generate 0 Toys? Can't do that!" << std::endl;  
  }
  if(this->arg->debug){
    std::cout << "DEBUG in PDF_GausPlusExp::generateToys -- generate Toys with current pdf parameters" << std::endl;  
    observables->Print("v");
    parameters->Print("v");
  }
  
  int signal_events_to_be_generated = (int) (getWorkspace()->var("branchingRatio")->getVal() / getWorkspace()->var("norm_constant")->getVal());
  cout <<"bimm"<<getWorkspace()->var("branchingRatio")->getVal() / getWorkspace()->var("norm_constant")->getVal()<<" "<<signal_events_to_be_generated << endl;

  RooDataSet* toys = this->getWorkspace()->pdf("g")->generate(*observables
                                                  , signal_events_to_be_generated
                                                  , RooFit::Extended(true)
                                                  );

  toys->append(*(this->getWorkspace()->pdf("e")->generate(*observables
                             ,int(this->getWorkspace()->var("Nbkg")->getVal())
                             ,RooFit::Extended(true)
                             )));
  

  this->toyObservables  = toys; 
  this->isToyDataSet    = kTRUE;

};


void  PDF_GausPlusExp::randomizeConstraintObservables(bool useConstrPdf){
    // we do not have any global observables
}
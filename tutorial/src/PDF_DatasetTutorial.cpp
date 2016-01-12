#include "PDF_DatasetTutorial.h"
#include "RooExponential.h"

PDF_DatasetTutorial::PDF_DatasetTutorial(RooWorkspace* w)
: PDF_Datasets_Abs(w,1, NULL)
{
  name    = "PDF_DatasetTutorial";
  title   = "PDF_DatasetTutorial";
  data = (RooDataSet*)wspc->data("data"); //> set real Dataset 
  if(data){
    isDataSet = kTRUE;
    std::cout << "INFO in PDF_DatasetTutorial::PDF_DatasetTutorial -- Dataset initialized" << std::endl;
  }
  else{
    std::cout << "FATAL in PDF_DatasetTutorial::PDF_DatasetTutorial -- no Dataset with name 'data' found in workspace!" << std::endl;
    isDataSet = kFALSE;
  }
  
  drawFitsDebug  = kFALSE;
}
PDF_DatasetTutorial::~PDF_DatasetTutorial(){};

RooFitResult* PDF_DatasetTutorial::fit(bool fitToys){
  if(this->notSetupToFit(fitToys)){
    std::cout << "FATAL in PDF_DatasetTutorial::fit -- There is no PDF or (toy)data set to fit!" << std::endl;  
    return NULL;
  }
  // Turn off RooMsg
  RooMsgService::instance().setGlobalKillBelow(ERROR);
  RooMsgService::instance().setSilentMode(kTRUE);
  // Choose Dataset to fit to
  RooDataSet* dataToFit = (fitToys) ? this->toyObservables : this->data ;
  
  if(fitToys)   wspc->loadSnapshot(globalObsToySnapshotName);
  else          wspc->loadSnapshot(globalObsDataSnapshotName);

  RooFitResult* result  = pdf->fitTo( *dataToFit, RooFit::Save() 
                                      ,RooFit::ExternalConstraints(*this->getWorkspace()->set(constraintName))
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

void   PDF_DatasetTutorial::generateToys(int SeedShift) {
  TRandom3 rndm(0);

  double mean_signal_events_to_be_generated = getWorkspace()->var("branchingRatio")->getVal() / getWorkspace()->var("norm_constant")->getVal();
  double mean_background_events_to_be_generated = this->getWorkspace()->var("Nbkg")->getVal();

  std::default_random_engine generator(std::random_device{}());
  std::poisson_distribution<int> signal_poisson(mean_signal_events_to_be_generated);
  std::poisson_distribution<int> background_poisson(mean_background_events_to_be_generated);

  int sig_number = signal_poisson(generator);
  int bkg_number = background_poisson(generator);

  RooDataSet* toys = this->getWorkspace()->pdf("g")->generate(*observables, sig_number);
  toys->append(*(this->getWorkspace()->pdf("background_model")->generate(*observables,bkg_number)));

  this->toyObservables  = toys; 
  this->isToyDataSet    = kTRUE;
}

  void  PDF_DatasetTutorial::generateToysGlobalObservables(bool useConstrPdf , int SeedShift) {
    // \todo: generate the global observables!!!!
      
    wspc->saveSnapshot(globalObsToySnapshotName, *wspc->set(globalObsName));
}
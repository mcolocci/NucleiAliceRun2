void runAna(const Char_t *kCollectionFile="wnTest.xml", Long64_t nentries=TChain::kBigNumber) {
  
  // LHC10b and LHC10c : useVTX=kTRUE  useT0=kFALSE
  // LHC10d and LHC10e : useVTX=kFALSE useT0=kTRUE
  
  Int_t nmaxfile = 1;//limit on the number of ESD root files

  Bool_t kAOD = kFALSE;
  Bool_t grid = kFALSE;//TRUE;
  Bool_t connect = kFALSE;
  if(grid) connect = kTRUE;
    
  //for PID response and Physics Selection
  Bool_t isMC = kFALSE;
  //Uncomment only if recoPass changes
  /*Bool_t autoMCesd=kTRUE;
  Bool_t tuneOnData=kTRUE;
  if(isMC) tuneOnData=kTRUE;
  Int_t recoPass=2;
  Bool_t cachePID=kFALSE;
  TString detResponse="";
  Bool_t useTPCEtaCorrection = kTRUE;
  Bool_t useTPCMultiplicityCorrection = kTRUE;
  Int_t  recoDataPass = 2;
  */

  // include the path you need to compile and the library you need
  gSystem->SetIncludePath("-I. -I$ROOTSYS/include -I$ALICE_ROOT/include -I$ALICE_PHYSICS/include");   
  
  gSystem->Load("libANALYSIS");
  gSystem->Load("libANALYSISalice");
  gSystem->Load("libOADB");
    
  //for commited task:
  /*
  */
  
  //for local task
  gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/AddTaskPIDResponse.C");
  gROOT->LoadMacro("$ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C");
  gROOT->LoadMacro("$ALICE_PHYSICS/OADB/COMMON/MULTIPLICITY/macros/AddTaskMultSelection.C"); 
  gROOT->LoadMacro("AddAnalysisTaskNuclei.C");
  gROOT->LoadMacro("AliAnalysisTaskNuclei.cxx++g");
  
  // Connect to the GRID
  if(connect) TGrid::Connect("alien://");
   
  AliAnalysisManager *mgr = new AliAnalysisManager("testAnalysis");

  TChain* analysisChain;
  if(kAOD){
    AliAODInputHandler* aodH = new AliAODInputHandler;
    mgr->SetInputEventHandler(aodH);
    analysisChain = new TChain("aodTree");
  }
  else{
    AliVEventHandler* esdH = new AliESDInputHandler;
    mgr->SetInputEventHandler(esdH);  
    if(isMC){
      AliMCEventHandler* mcH = new AliMCEventHandler();
      mgr->SetMCtruthEventHandler(mcH);
    }
    analysisChain = new TChain("esdTree");
  }
  
  TAlienCollection *myCollection = NULL;
  if(grid){
    myCollection = (TAlienCollection *) TAlienCollection::Open(kCollectionFile);
    if (!myCollection) {
      Error("AliThisTask", Form("Cannot create an AliEn collection from %s", kCollectionFile));
      return;
    }
    myCollection->Reset();    
  }
  
  Int_t ifile=0;
      
  //read the list from a xml collection "wn.xml" (grid case)
  while(grid && myCollection->Next() && ifile < nmaxfile) {
    ifile++;
    char esdFile[255];
    snprintf(esdFile,255, "%s", myCollection->GetTURL(""));
    Info("AliTaskAlienForTOF", Form("Adding %s", esdFile));
    analysisChain->Add(esdFile);
  }
  //read the list from a local text file "lista" (local case)
  if(!grid){
    char nomefile[100];
    if(kAOD) FILE *fl = fopen("listaAOD","r");
    else FILE *fl = fopen("listaESD","r");
    while(fscanf(fl,"%s",nomefile) == 1 && ifile < nmaxfile){
      analysisChain->Add(nomefile);
      ifile++;
    }
    fclose(fl);
  }
  Info("Task", Form("CHAIN HAS %d ENTRIES", (Int_t)analysisChain->GetEntries()));
  
  //Uncomment only if recoPass changes
  //AddTaskPIDResponse(isMC,autoMCesd,tuneOnData,recoPass,cachePID,detResponse,useTPCEtaCorrection,useTPCMultiplicityCorrection,recoDataPass);
  AddTaskPIDResponse(isMC);
      
  AddTaskPhysicsSelection(isMC);//needed for ESDs

  AddTaskMultSelection(kFALSE);
  
  AddAnalysisTaskNuclei(isMC);
  
  // Enable debug printouts
  // mgr->SetDebugLevel(2);
  
  // Run analysis
  // AliLog::SetGlobalLogLevel(AliLog::kError);
  mgr->InitAnalysis();
  mgr->PrintStatus();
  mgr->StartAnalysis("startAnalysis", analysisChain);
  
  return;
}


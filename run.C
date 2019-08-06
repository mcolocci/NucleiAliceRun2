const Char_t *AliPhysicsVersion = "v5-09-24-01-1";

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

void run(TString pluginMode = "test", 
	 TString dataset = "test", 
	 TString runlist = "test")
{

  /* include path for ACLic */
  gSystem->AddIncludePath("-I$ROOTSYS/include");
  gSystem->AddIncludePath("-I$ALICE_ROOT/include");
  gSystem->AddIncludePath("-I$ALICE_PHYSICS/include");

  /* load libraries */
  gSystem->Load("libANALYSIS");
  gSystem->Load("libANALYSISalice");
  gSystem->Load("libOADB");

  //
  TString trainName = "PWGLF";
  //
  TString analysisTag = "Nuclei"; 
  analysisTag += "_"; 
  analysisTag += dataset; 
  analysisTag += "_"; 
  analysisTag += runlist;
  //  
  AliAnalysisGrid *alienHandler = CreateAlienHandler(pluginMode, dataset, runlist, analysisTag);
  //
  AliAnalysisManager *mgr  = new AliAnalysisManager("manager", "manager");
  mgr->SetDebugLevel(0);
  mgr->SetGridHandler(alienHandler);
  //
  AliESDInputHandler *inputHandler = new AliESDInputHandler("handler", "ESD handler");
  inputHandler->SetNeedField();
  mgr->SetInputEventHandler(inputHandler);

  /****************************************/
  /* ADD TASK                             */
  /****************************************/

  gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/AddTaskPIDResponse.C");
  gROOT->LoadMacro("$ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C");
  gROOT->LoadMacro("$ALICE_PHYSICS/OADB/COMMON/MULTIPLICITY/macros/AddTaskMultSelection.C");
  gROOT->LoadMacro("AddAnalysisTaskNuclei.C");
  gROOT->LoadMacro("AliAnalysisTaskNuclei.cxx++g");
  
  AddTaskPIDResponse();
  AddTaskPhysicsSelection(kFALSE);//no calib
  AddTaskMultSelection(kFALSE);//no MC
  AddAnalysisTaskNuclei(kFALSE);//no MC
  
  /****************************************/
  /* RUN ANALYSIS                         */
  /****************************************/

  if(!mgr->InitAnalysis()) return;
  //mgr->SetDebugLevel(2);
  mgr->PrintStatus();
  mgr->StartAnalysis("grid");
  return;

}

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

AliAnalysisGrid* CreateAlienHandler(TString pluginmode,
				    TString dataset,
				    TString runlist,
				    TString analysisTag)
{

    AliAnalysisAlien *plugin = new AliAnalysisAlien();
    plugin->SetRunMode(pluginmode.Data());
    plugin->SetAPIVersion("V1.1x");
    plugin->SetAliPhysicsVersion(AliPhysicsVersion);
    // config dataset
    ConfigDataset(plugin, dataset, runlist);
    //
    plugin->SetGridWorkingDir(analysisTag.Data());
    plugin->SetExecutable("Nuclei.sh");
    plugin->SetAnalysisMacro("Nuclei.C");    
    TString exe = analysisTag; exe += ".sh";
    TString mac = analysisTag; mac += ".C";
    // Declare alien output directory. Relative to working directory.
    plugin->SetGridOutputDir("output"); // In this case will be $HOME/work/output
    //
    plugin->AddIncludePath("-I. -I$ROOTSYS/include -I$ALICE_ROOT -I$ALICE_ROOT/include -I$ALICE_PHYSICS/include -I$ALICE_ROOT/STEER -I/$ALICE_ROOT/ANALYSIS -I/$ALICE_ROOT/ANALYSISalice");
    //
    //plugin->SetAdditionalLibs("$ALICE_ROOT/ANALYSIS/macros/AddTaskPIDResponse.C $ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C $ALICE_PHYSICS/OADB/COMMON/MULTIPLICITY/macros/AddTaskMultSelection.C AliAnalysisTaskNuclei.h AliAnalysisTaskNuclei.cxx AddAnalysisTaskNuclei.C libGui.so libProof.so libMinuit.so libANALYSIS.so libOADB.so libANALYSISalice.so libSTEERBase.so, libESD.so, libAOD.so, libANALYSISaliceBase.so, libCORRFW.so");
    plugin->SetAdditionalLibs("AliAnalysisTaskNuclei.h AliAnalysisTaskNuclei.cxx");
    plugin->SetAnalysisSource("AliAnalysisTaskNuclei.cxx");
    //
    plugin->SetKeepLogs(kTRUE);
    plugin->SetDefaultOutputs(kTRUE);
    plugin->SetMergeViaJDL(kTRUE);
    plugin->SetOneStageMerging(kFALSE);
    plugin->SetMaxMergeStages(1);
    //
    plugin->SetSplitMaxInputFileNumber(100);    
    plugin->SetNtestFiles(1);

    return plugin;
}

void ConfigDataset(AliAnalysisAlien *plugin,
		   TString dataset,
		   TString runlist)
{

  // configure dataset
  //
  if (dataset.EqualTo("test")) {
    plugin->SetRunPrefix("000");
    plugin->SetGridDataDir("/alice/data/2018/LHC18q");
    plugin->SetDataPattern("pass1/*/AliESDs.root");
  }

  // configure runlist
  //
  Int_t *_runlist = NULL;
  Int_t _nruns = 0;
  //
  Int_t runlist_test[] = {
    295585
  };
  //
  if (runlist.EqualTo("test")) {
    _runlist = runlist_test;
    _nruns = sizeof(runlist_test)/4;
  }
      
  // add the runs
  Int_t nruns = 0;
  for(Int_t irun = 0; irun < _nruns; irun++) {
    plugin->AddRunNumber(_runlist[irun]);
    nruns++;
  }
  printf(">>> ConfigDataset: added %d runs from runlist %s\n", nruns, runlist.Data()); 
  plugin->SetNrunsPerMaster(1);  
  plugin->SetOutputToRunNo(1);

  return;
  
}


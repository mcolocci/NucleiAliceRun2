AliAnalysisTaskSE *AddAnalysisTaskNuclei(Bool_t isMC=kFALSE){

  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  
  //for ESDs
  //AliPhysicsSelectionTask* physSelTask = AddTaskPhysicsSelection(isMC);

  //AliPPVsMultUtils *fAliPPVsMultUtils = new AliPPVsMultUtils();//for Run 1

  const Int_t Ntask=1;
  
  AliAnalysisTaskNuclei *task[Ntask];
  for(Int_t i=0;i<Ntask;i++) {
    task[i] = new AliAnalysisTaskNuclei("AliAnalysisTaskNuclei");
    mgr->AddTask(task[i]);
  }
  
  for(Int_t i=0;i<Ntask;i++) {
    task[i]->SetIsMC(isMC);
    //task[i]->SetPPVsMultUtilsObj(fAliPPVsMultUtils);
    task[i]->SetNTPCclustersMin();
    task[i]->SetNsigmaTPCcut();
    task[i]->SetDCAxyMax();
    task[i]->SetDCAzMax();
  }
  
  AliAnalysisDataContainer *cinput[Ntask];
  AliAnalysisDataContainer *cOutputL[Ntask];
  for(Int_t i=0;i<Ntask;i++) {
    Char_t nameIn[1000];
    Char_t nameOut[1000];
    
    snprintf(nameIn,1000,"cchain1_output");
    snprintf(nameOut,1000,"output");
    
    cinput[i] = mgr->CreateContainer(nameIn,TChain::Class(),AliAnalysisManager::kInputContainer);
    mgr->ConnectInput(task[i],0,mgr->GetCommonInputContainer());
    
    cOutputL[i] = mgr->CreateContainer(nameOut,TList::Class(), AliAnalysisManager::kOutputContainer, AliAnalysisManager::GetCommonFileName());
    mgr->ConnectOutput(task[i],1,cOutputL[i]);
  }
  
  return task[0];
}

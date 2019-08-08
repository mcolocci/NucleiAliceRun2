#include <TGrid.h>

void DoMerge(char* nomelista="lista", char* output="AnalysisResults.root"){
//  system("ls */AnalysisResults.root >lista");
 gSystem->Load("libVMC.so");
 gSystem->Load("libPhysics.so");
 gSystem->Load("libTree.so");
 gSystem->Load("libSTEERBase.so");
 gSystem->Load("libANALYSIS.so");
 gSystem->Load("libAOD.so");
 gSystem->Load("libESD.so");
 gSystem->Load("libANALYSISalice.so");
 gSystem->Load("libCORRFW.so");
 gSystem->Load("libNetx.so");

 FILE *f = fopen(nomelista,"r");
 
 TFileMerger m(kFALSE);
 m.OutputFile(output);
 
 Int_t i=0;
 char nome[100];
 while (fscanf(f,"%s",nome)==1) {
   m.AddFile(nome);
   i++;
 }
 if (i)
   m.Merge();

 return;
}


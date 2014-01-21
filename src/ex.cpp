#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <TCanvas.h>
#include <TTree.h>
#include <TFile.h>
#include <TH1F.h>

#include "DDASEvent.h"
#include "DetectorLibrary.hpp"
#include "Identifier.hpp"
#include "MapFile.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    //Load in the ROOT tree from the ddasdumper
    TFile file("/mnt/analysis/e13504/svp/rootFiles/test00.root");
    TTree *tr = (TTree*)file.Get("dchan");
    TBranch *br = tr->GetBranch("ddasevent");

    int numMods = 2;
    int numCh = numMods*16;

    //Initialize the Detector Library
    DetectorLibrary detLib(numCh);
    //Read the indicated map file and populate the Detector Library with 
    //the known detectors
    MapFile map(detLib, "src/map.txt");

    //Print the read in map file to make sure we got what we expected.
    detLib.PrintMap();

    vector<TH1F*> histos(numCh);

    //Module 0 Histograms
    TH1F *csI = new TH1F("csI", "Energy Spectrum for M0C0", 32768,0,32768);
    TH1F *bmOn0 = new TH1F("bmOn0", "Energy Spectrum for M0C4", 32768,0,32768);
    TH1F *bmOff0 = new TH1F("bmOff0", "Energy Spectrum for M0C5", 32768,0,32768);
    TH1F *plsr0 = new TH1F("plsr0", "Energy Spectrum for M0C6", 32768,0,32768);
    
    //Module 1 Histograms
    TH1F *ge  = new TH1F("ge", "Energy Spectrum for M1C0", 32768,0,32768);
    TH1F *bmOn1 = new TH1F("bmOn1", "Energy Spectrum for M1C4", 32768,0,32768);
    TH1F *bmOff1 = new TH1F("bmOff1", "Energy Spectrum for M1C5", 32768,0,32768);
    TH1F *plsr1 = new TH1F("plsr1", "Energy Spectrum for M1C6", 32768,0,32768);
    
    DDASEvent *event = new DDASEvent(); 
    br->SetAddress(&event);

    int numEvent = tr->GetEntries();
    int cutoff = 0;
    
    for (int i = 0; i < numEvent; i++) {
        br->GetEntry(i); 
        vector<ddaschannel*> evt = event->GetData();
        for(auto j : evt) {
            unsigned int sId  = j->GetSlotID();
            unsigned int chId = j->GetChannelID();
            unsigned int id = (sId-2)*16 + chId;
            unsigned int en = j->GetEnergy();
            unsigned int tLow = j->GetTimeLow();
            unsigned int tHigh = j->GetTimeHigh();
            
            if(id == 0)
                csI->Fill(en);
            if(id == 4)
                bmOn0->Fill(en);
            if(id == 5)
                bmOff0->Fill(en);
            if(id == 6)
                plsr0->Fill(en);
            if(id == 16)
                ge->Fill(en);
            if(id == 20)
                bmOn1->Fill(en);
            if(id == 21)
                bmOff1->Fill(en);
            if(id == 22)
                plsr1->Fill(en);
        }
    }

    TFile out("/mnt/analysis/e13504/svp/rootFiles/test00-sort.root", 
              "RECREATE");
    csI->Write();
    bmOn0->Write();
    bmOff0->Write();
    plsr0->Write();

    ge->Write();
    bmOn1->Write();
    bmOff1->Write();
    plsr1->Write(); 

    out.Write();
}

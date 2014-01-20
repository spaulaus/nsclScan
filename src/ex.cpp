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
    
    TFile file("/mnt/analysis/e13504/svp/rootFiles/test00.root");
    TFile out("/mnt/analysis/e13504/svp/rootFiles/test00-sort.root", "RECREATE");
    TTree *tr = (TTree*)file.Get("dchan");
    TBranch *br = tr->GetBranch("ddasevent");

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
        // if(cutoff == 1e1)
        //     break;
    }

    // //--------- Module 0 Histograms ---------
    // TCanvas* c0 = new TCanvas("c0","",0,0,700,500);
    // c0->cd();
    // csI->GetXaxis()->SetTitle("Raw Energy");
    // csI->GetYaxis()->SetTitle("Counts");
    // csI->SetAxisRange(0,6000,"X");
    // csI->SetAxisRange(0,35e3,"Y");
    // csI->Draw();
    // c0->SaveAs("pics/prelim/test00-m0c0.eps");
    
    // TCanvas* c1 = new TCanvas("c1","",0,0,700,500);
    // c1->cd();
    // bmOn0->GetXaxis()->SetTitle("Raw Energy");
    // bmOn0->GetYaxis()->SetTitle("Counts");
    // bmOn0->SetAxisRange(0,16532,"X");
    // bmOn0->Draw();
    // c1->SaveAs("pics/prelim/test00-m0c4.eps");

    // TCanvas* c2 = new TCanvas("c2","",0,0,700,500);
    // c2->cd();
    // bmOff0->GetXaxis()->SetTitle("Raw Energy");
    // bmOff0->GetYaxis()->SetTitle("Counts");
    // bmOff0->SetAxisRange(0,16532,"X");
    // bmOff0->Draw();
    // c2->SaveAs("pics/prelim/test00-m0c5.eps");

    // TCanvas* c3 = new TCanvas("c3","",0,0,700,500);
    // c3->cd();
    // plsr0->GetXaxis()->SetTitle("Raw Energy");
    // plsr0->GetYaxis()->SetTitle("Counts");
    // plsr0->SetAxisRange(0,16532,"X");
    // plsr0->Draw();
    // c3->SaveAs("pics/prelim/test00-m0c6.eps");
    
    // //---------- Module 1 Histograms ----------
    // TCanvas* c4 = new TCanvas("c4","",0,0,700,500);
    // c4->cd();
    // ge->GetXaxis()->SetTitle("Raw Energy");
    // ge->GetYaxis()->SetTitle("Counts");
    // ge->SetAxisRange(0,16532,"X");
    // ge->SetAxisRange(0,250,"Y");
    // ge->Draw();
    // c4->SaveAs("pics/prelim/test00-m1c0.eps");

    // TCanvas* c5 = new TCanvas("c5","",0,0,700,500);
    // c5->cd();
    // bmOn1->GetXaxis()->SetTitle("Raw Energy");
    // bmOn1->GetYaxis()->SetTitle("Counts");
    // bmOn1->SetAxisRange(0,16532,"X");
    // bmOn1->Draw();
    // c5->SaveAs("pics/prelim/test00-m1c4.eps");

    // TCanvas* c6 = new TCanvas("c6","",0,0,700,500);
    // c6->cd();
    // bmOff1->GetXaxis()->SetTitle("Raw Energy");
    // bmOff1->GetYaxis()->SetTitle("Counts");
    // bmOff1->SetAxisRange(0,16532,"X");
    // bmOff1->Draw();
    // c6->SaveAs("pics/prelim/test00-m1c5.eps");

    // TCanvas* c7 = new TCanvas("c7","",0,0,700,500);
    // c7->cd();
    // plsr1->GetXaxis()->SetTitle("Raw Energy");
    // plsr1->GetYaxis()->SetTitle("Counts");
    // plsr1->SetAxisRange(0,16532,"X");
    // plsr1->Draw();
    // c7->SaveAs("pics/prelim/test00-m1c6.eps");

    out.Write();
}

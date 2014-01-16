#include <iostream>
#include <vector>

#include <TCanvas.h>
#include <TTree.h>
#include <TFile.h>
#include <TH1F.h>

#include "DDASEvent.h"

using namespace std;

int main(int argc, char* argv[]) {
    TH1F *ge  = new TH1F("ge", "Energy Spectrum for Ge M1C0", 32768,0,32768);
    TH1F *csI = new TH1F("csI", "Energy Spectrum for CsI M0C0", 32768,0,32768);
    TFile file("/mnt/analysis/e13504/svp/rootFiles/test00.root");
    TFile out("/mnt/analysis/e13504/svp/rootFiles/test00-sort.root");
    TTree *tr = (TTree*)file.Get("dchan");
    TBranch *br = tr->GetBranch("ddasevent");

    tr->Print();

    DDASEvent *event = new DDASEvent(); 
    br->SetAddress(&event);

    int numEvent = tr->GetEntries();
    int numGe = 0;
    
    for (int i = 0; i < numEvent; i++) {
        br->GetEntry(i); 
        vector<ddaschannel*> evt = event->GetData();
        //cout << "Reading event Number " << i << endl
        //      << "------------------------------" << endl
        //      << "Boss, there are " << evt.size() 
        //      << " events in this bitch." << endl;
        for(auto j : evt) {
            unsigned int crId = j->GetCrateID();
            unsigned int sId  = j->GetSlotID();
            unsigned int chId = j->GetChannelID();
            unsigned int id = (sId-2)*16 + chId;
            unsigned int en = j->GetEnergy();
            unsigned int tLow = j->GetTimeLow();
            unsigned int tHigh = j->GetTimeHigh();
            unsigned int fCode = j->GetFinishCode();
            
            if(id == 16) {
                //cout << "The Ge count is " << numGe << endl;
                ge->Fill(en);
                numGe++;
            }
            if(id == 0)
                csI->Fill(en);
            
            // cout << crId << " " << sId << " " 
            //      << chId << " " << id << " " << en << " " 
            //      << tLow << " " << tHigh << " " << fCode << endl;
        }
        //cout << endl << endl;
        if(numGe == 1e6)
            break;
    }
    TCanvas* c = new TCanvas("c","",0,0,700,500);
    c->cd();
    ge->GetXaxis()->SetTitle("Raw Energy");
    ge->GetYaxis()->SetTitle("Counts");
    ge->SetAxisRange(0,16532,"X");
    ge->SetAxisRange(0,250,"Y");
    ge->Draw();
    c->SaveAs("test00-ge.eps");

    TCanvas* c1 = new TCanvas("c1","",0,0,700,500);
    c1->cd();
    csI->GetXaxis()->SetTitle("Raw Energy");
    csI->GetYaxis()->SetTitle("Counts");
    csI->SetAxisRange(0,6000,"X");
    csI->SetAxisRange(0,35e3,"Y");
    csI->Draw();
    c1->SaveAs("test00-csi.eps");
}

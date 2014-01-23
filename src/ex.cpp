/** \file ex.cpp
 *  \brief A code to process data from the ddasdumper program
 *
 *  \author S. V. Paulauskas
 *  \date 15 January 2014
 */
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <TCanvas.h>
#include <TTree.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>

#include "DDASEvent.h"
#include "DetectorLibrary.hpp"
#include "Identifier.hpp"
#include "MapFile.hpp"

using namespace std;

double CalcId(double &slot, double &ch) {
    return((slot-2)*16 + ch);
}

int main(int argc, char* argv[]) {
    int numMods = 2;
    int numCh = numMods*16;

    //Initialize the Detector Library
    DetectorLibrary detLib(numCh);

    //Read the indicated map file and populate the Detector Library with 
    //the known detectors
    MapFile map(detLib, "src/map.txt");
    
    set<int> usdIds = detLib.GetUsedIds();
    
    std::map<int, TH1D*> eHstgrm, tHstgrm;
    for(const auto &i : usdIds) {
        Identifier chInfo = detLib.at(i);

        stringstream eName, eTitle, etName, etTitle, tTitle, tName;
        eName << chInfo.GetType() << ":" << chInfo.GetSubtype() 
             << ":" << i << ":" << "RawEnergy";
        eTitle << "Energy Spectrum for M" << detLib.ModuleFromIndex(i) 
              << "C" << detLib.ChannelFromIndex(i);

        tName << chInfo.GetType() << ":" << chInfo.GetSubtype() 
             << ":" << i << ":" << "Rate";
        tTitle << "Rate Spectrum for M" << detLib.ModuleFromIndex(i) 
              << "C" << detLib.ChannelFromIndex(i);

        TH1D *eHist = new TH1D(eName.str().c_str(), eTitle.str().c_str(), 
                               32768, 0, 32768.);
        TH1D *tHist = new TH1D(tName.str().c_str(), tTitle.str().c_str(), 
                               5000, 0, 5000.);

        eHstgrm.insert(make_pair(i, eHist));
        tHstgrm.insert(make_pair(i, tHist));
    }

    TH2D *etHstgrm = new TH2D("csI:large:0:TimeEnergy", 
                              "Time vs Energy Spectrum for M0C0",
                              32000, 0, 32000., 5000, 0, 5000.);
    
    // vector<string> files = {"/mnt/analysis/e13504/svp/rootFiles/run145/run-0145-00.root",
    //                         "/mnt/analysis/e13504/svp/rootFiles/run145/run-0145-01.root",
    //                         "/mnt/analysis/e13504/svp/rootFiles/run145/run-0145-02.root"};
    vector<string> files = { "/mnt/analysis/e13504/svp/rootFiles/run145/run-0145-02.root"};
    string outFile = "/mnt/analysis/e13504/svp/rootFiles/run145/run-0145-summed.root";
    
    //initialize the first time
    double fTime = 0;
    
    for(const auto &it : files) {
        cout << "We are working on the following file, boss. " << endl
             << it << endl;

        //Load in the ROOT tree from the ddasdumper
        TFile file(it.c_str());
        TTree *tr = (TTree*)file.Get("dchan");
        TBranch *br = tr->GetBranch("ddasevent");
     
        DDASEvent *event = new DDASEvent(); 
        br->SetAddress(&event);
     
        int numEvent = tr->GetEntries();
        int cutoff = 0;
     
        for (int i = 0; i < numEvent; i++) {
            if(i == 1000) 
                break;
            br->GetEntry(i); 
            vector<ddaschannel*> evt = event->GetData();
            for(const auto &j : evt) {
                double slot  = j->GetSlotID();
                double chan = j->GetChannelID();
                double id = CalcId(slot, chan);
                double en = j->GetEnergy();
                double time = j->GetTime();

                //set the first time
                if(i == 0 && j == evt.at(0) && it == files.at(0))
                    fTime = time;
                
                //caluclate the time difference in seconds.
                double tdiff = (time - fTime)*10.e-9; 
                
            
                eHstgrm.at(id)->Fill(en);
                tHstgrm.at(id)->Fill(tdiff);
                if(id == 0)
                    etHstgrm->Fill(en,tdiff);

            }//for(const auto j : evt)

            // out.cd();
            // if(i % 100 == 0) {
            //     for(const auto &id : usdIds) {
            //         eHstgrm.at(id)->Write();
            //         tHstgrm.at(id)->Write();
            //     }
            //     etHstgrm->Write();
            //     out.Write();
            // } 
        }//for(int i = 0; i < numEvent
    }//for(auto it : files)
 
    //Save the histograms to a root file
    TFile out(outFile.c_str(), "UPDATE");
    for(const auto &id : usdIds) {
        eHstgrm.at(id)->Write();
        tHstgrm.at(id)->Write();
    }
    etHstgrm->Write();
    out.Write();
}

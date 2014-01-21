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
    
    set<int> usdIds = detLib.GetUsedIds();
    
    std::map<int, TH1D*> eHstgrm;
    for(const auto &i : usdIds) {
        Identifier chInfo = detLib.at(i);
        stringstream name, title;
        name << chInfo.GetType() << ":" << chInfo.GetSubtype() 
             << ":" << i;
        title << "Energy Spectrum for M" << detLib.ModuleFromIndex(i) 
              << "C" << detLib.ChannelFromIndex(i);

        TH1D *hist = new TH1D(name.str().c_str(), title.str().c_str(), 32768, 0, 32768);
        eHstgrm.insert(make_pair(i, hist));
    }

    vector<string> files = {"/mnt/analysis/e13504/svp/rootFiles/run145/run-0145-00.root",
                            "/mnt/analysis/e13504/svp/rootFiles/run145/run-0145-01.root",
                            "/mnt/analysis/e13504/svp/rootFiles/run145/run-0145-02.root"};
    string outFile = "/mnt/analysis/e13504/svp/rootFiles/run145/run-0145-summed.root";

    for(auto it : files) {
        cout << "We are working on the following file, boss. " << it << endl;

        //Load in the ROOT tree from the ddasdumper
        TFile file(it.c_str());
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
             
                eHstgrm.at(id)->Fill(en);
            }
        }
    }//for over files

    //Save the histograms to a root file
    TFile out(outFile.c_str(), "UPDATE");
    for(const auto &id : usdIds)
        eHstgrm.at(id)->Write();
    out.Write();
}

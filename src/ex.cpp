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
#include <TH3D.h>

// #include <RooAddPdf.h>
// #include <RooConstVar.h>
// #include <RooDataSet.h>
// #include <RooDouble.h>
// #include <RooFitResult.h>
// #include <RooFormulaVar.h>
// #include <RooPlot.h>
// #include <RooRealVar.h>

#include "DDASEvent.h"
#include "DetectorLibrary.hpp"
#include "Identifier.hpp"
#include "MapFile.hpp"

using namespace std;
//using namespace RooFit;

double CalcId(double &slot, double &ch) {
    return((slot-2)*16 + ch);
}

int main(int argc, char* argv[]) {
    //The input and output files
    vector<string> files = 
        {"/mnt/analysis/e13504/svp/rootFiles/run145/run-0145-00.root",
         "/mnt/analysis/e13504/svp/rootFiles/run145/run-0145-01.root",
         "/mnt/analysis/e13504/svp/rootFiles/run145/run-0145-02.root"};
    // vector<string> files = 
    //     {"/mnt/analysis/e13504/svp/rootFiles/run145/run-0145-02.root"};
    string outFile = 
        "/mnt/analysis/e13504/svp/rootFiles/run145/run-0145-summed.root";

    int numMods = 2;
    int numCh = numMods*16;
    double energyContraction = 4.;

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
                               8192, 0., 8192.);
        TH1D *tHist = new TH1D(tName.str().c_str(), tTitle.str().c_str(), 
                               8192, 0., 8192.);

        eHstgrm.insert(make_pair(i, eHist));
        tHstgrm.insert(make_pair(i, tHist));
    }

    TH1D *bHist = new TH1D("csI:large:0:dTmsOff", "Tdiff w BeamOff", 
                           1.2e5, 0., 12.);
    TH1D *oHist = new TH1D("csI:large:0:dTmsOn", "Tdiff w BeamOn", 
                           1.2e5, 0., 12.);

    TH2D *etHstgrm = new TH2D("csI:large:0:TimeEnergy", 
                              "Time vs Energy Spectrum for M0C0",
                              8192, 0., 8192., 1.2e5, 0., 12.);
    TH2D *gtHstgrm = new TH2D("ge:ignore:0:TimeEnergy", 
                              "Time vs Energy Spectrum for M1C0",
                              8192, 0., 8192., 1.2e5, 0., 12.);

    gtHstgrm->SetContour(100.);

    //Define the data set for fitting 
    // RooRealVar rootime("time","Decay time", 0.0, 0., 12000.);
    // RooDataSet data("data","Fitting dataset", rootime);

    //initialize the first time
    double firstTime = 0, onTime = 0, offTime = 0;
    
    for(const auto &it : files) {
        cout << "We are working on the following file, boss. " << endl
             << it << endl;

        //Load in the ROOT tree from the ddasdumper
        TFile inFile(it.c_str());
        TTree *tr = (TTree*)inFile.Get("dchan");
        TBranch *br = tr->GetBranch("ddasevent");
     
        DDASEvent *event = new DDASEvent(); 
        br->SetAddress(&event);
        int numEvent = tr->GetEntries();

        for (int i = 0; i < numEvent; i++) {
            // if(i == 1000)
            //     break;
            br->GetEntry(i); 
            vector<ddaschannel*> evt = event->GetData();
            for(const auto &j : evt) {
                double slot  = j->GetSlotID();
                double chan = j->GetChannelID();
                double id = CalcId(slot, chan);
                double en = j->GetEnergy() / energyContraction;
                double time = j->GetTime();

                //set the various times
                if(i == 0 && j == evt.at(0) && it == files.at(0))
                    firstTime = time;
                if(id == 5)
                    offTime = time;
                if(id == 4)
                    onTime = time;
    
                //caluclate the Run Time in seconds.
                double runTime = (time - firstTime)*10.e-9; 
                double timeBoff = (time - offTime)*10.e-9; 
                double timeBon = (time - onTime)*10.e-9; 

                //Stuff related to only the CsI
                if(id == 0) {
                    bHist->Fill(timeBoff);
                    oHist->Fill(timeBon);
                    etHstgrm->Fill(en,timeBon);
                    // rootime.setVal(timeBon);
                    // data.add(RooArgList(rootime));
                }
                if(id == 16) {
                    gtHstgrm->Fill(en,timeBon);
                }
                eHstgrm.at(id)->Fill(en);
                tHstgrm.at(id)->Fill(runTime);
            }//for(const auto j : evt)
        }//for(int i = 0; i < numEvent
        inFile.Close();
    }//for(auto it : files)
 
    //Save the histograms to a root file
    TFile outF(outFile.c_str(), "RECREATE");
    for(const auto &id : usdIds) {
        eHstgrm.at(id)->Write();
        tHstgrm.at(id)->Write();
    }
    bHist->Write();
    oHist->Write();
    etHstgrm->Write();
    gtHstgrm->Write();
    
    outF.Write();
}

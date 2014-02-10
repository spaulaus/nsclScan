/** \file ex.cpp
 *  \brief A code to process data from the ddasdumper program
 *
 *  \author S. V. Paulauskas
 *  \date 15 January 2014
 */

#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <TTree.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>

#include "DDASEvent.h"
#include "DetectorLibrary.hpp"
#include "Identifier.hpp"
#include "MapFile.hpp"

using namespace std;

double CalcId(const double &slot, const double &ch) {
    return((slot-2)*16 + ch);
}

double CalGe(const double &en) {
    double m = 0.17065;
    double b = 1.4821;
    double calEn = m*en+b;
    return(calEn);
}

int main(int argc, char* argv[]) {
	if(argc < 3) {
		cerr << "Usage : ./ex <input> <output>" << endl;
		exit(2);
	}

	set<string> files;
	for(int i = 1; i < argc-1; i++)
		files.insert(argv[i]);
	string outFile = argv[argc-1];

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
                               5000, 0., 5000.);

        eHstgrm.insert(make_pair(i, eHist));
        tHstgrm.insert(make_pair(i, tHist));
    }

    TH1D *hits = new TH1D("hitSpectrum", "Channel Hit Spectrum", numCh+1, 0.,
                              numCh+1);
    TH1D *mult = new TH1D("multiplicity", "Event Multiplicity", numCh+1, 0.,
                              numCh+1);

    TH1D *bHist = new TH1D("csI:large:0:dtOff", "Tdiff w BeamOff",
                           1.3e4, 0., 13.);
    TH1D *tHist1 = new TH1D("csI:large:0:cpms", "CPmS",
                            5.e6, 0., 5.e3);
    TH1D *oHist = new TH1D("csI:large:0:dtOn", "Tdiff w BeamOn",
                           1.3e4, 0., 13.);
    TH1D *geCal = new TH1D("ge:ignore:16:CalEn", "Calibrated Ge",
                           8192,0.,8192.);

    TH2D *etHstgrm = new TH2D("csI:large:0:TimeEnergy",
                              "Time vs Energy Spectrum for M0C0",
                              8192, 0., 8192., 1.3e4, 0., 13.);
    TH2D *gtHstgrm = new TH2D("ge:ignore:0:TimeEnergy",
                              "Time vs Cal Energy Spectrum for M1C0",
                              8192, 0., 8192., 1.3e4, 0., 13.);

    //initialize the first time
    double firstTime = 0, onTime = 0, offTime = 0;
    vector<double> csiE, csiT;

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

                mult->Fill(evt.size());
                hits->Fill(id);

                //set the various times
                if(i == 0 && j == evt.at(0) && it == *files.begin())
                    firstTime = time;
                if(id == 5)
                    offTime = time;
                if(id == 4) {
                    if((time - onTime)*10.e-9 > 13.) {
                        cerr << "Oh man, we missed a beam on!! I am going to"
                             << " junk all the stored stats till now. " << endl;
                        csiE.clear();
                        csiT.clear();
                    }
                    onTime = time;
                    for(unsigned int i = 0; i < csiE.size(); i++)
                        etHstgrm->Fill(csiE.at(i), csiT.at(i));
                    csiE.clear();
                    csiT.clear();
                }

                //caluclate the Run Time in seconds.
                double runTime = (time - firstTime)*10.e-9;
                double timeBoff = (time - offTime)*10.e-9;
                double timeBon = (time - onTime)*10.e-9;

                //Stuff related to only the CsI
                if(id == 0 && onTime != 0) {
                    bHist->Fill(timeBoff);
                    oHist->Fill(timeBon);
                    csiE.push_back(en);
                    csiT.push_back(timeBon);
                }
                if(id == 16 && onTime != 0) {
                    double calEn = CalGe(en*energyContraction);
                    gtHstgrm->Fill(calEn,timeBon);
                    geCal->Fill(calEn);
                }
                eHstgrm.at(id)->Fill(en);
                tHstgrm.at(id)->Fill(runTime);
                if(onTime != 0 && runTime < 1463 && runTime > 1450)
                    tHist1->Fill(runTime);
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
    hits->Write();
    mult->Write();
    bHist->Write();
    geCal->Write();
    oHist->Write();
    tHist1->Write();
    etHstgrm->Write();
    gtHstgrm->Write();
    outF.Write();
    cout << "Finishing up" << endl;
}

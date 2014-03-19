/** \file ex.cpp
 *  \brief A code to process data from the ddasdumper program
 *
 *  \author S. V. Paulauskas
 *  \date 15 January 2014
 */
#include <chrono>
#include <iostream>
#include <map>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <TTree.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>

#include "Calibrator.hpp"
#include "DDASEvent.h"
#include "DetectorLibrary.hpp"
#include "Identifier.hpp"
#include "MapFile.hpp"

using namespace std;

double CalcId(const double &slot, const double &ch) {
    return((slot-2)*16 + ch);
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
    
    //Initialize the rng for the energies. Pixie energies are ints 
    //performing calibrations on them can lead to artifacts w/o this.
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    mt19937_64 rng(seed);
    uniform_real_distribution<double> dist(0.0,1.0);

    int numMods = 2;
    int numCh = numMods*16;

    //Instance the Calibrator
    Calibrator cal;

    //Initialize the Detector Library
    DetectorLibrary detLib(numCh);

    //Read the indicated map file and populate the Detector Library with
    //the known detectors
    MapFile map(detLib, "src/map.txt");

    set<int> usdIds = detLib.GetUsedIds();

    //---------- BEGIN HISTOGRAM DEFINITIONS ----------
    std::map<int, TH1D*> ceHstgrm, eHstgrm, tHstgrm;
    for(const auto &i : usdIds) {
        Identifier chInfo = detLib.at(i);

        stringstream ceName, ceTitle, eName, eTitle, tTitle, tName;

        ceName << chInfo.GetType() << ":" << chInfo.GetSubtype()
               << ":" << i << ":" << "CalEnergy";
        ceTitle << "Calibrated Energy Spectrum for M" 
                << detLib.ModuleFromIndex(i) << "C" 
                << detLib.ChannelFromIndex(i);

        eName << chInfo.GetType() << ":" << chInfo.GetSubtype()
              << ":" << i << ":" << "RawEnergy";
        eTitle << "Energy Spectrum for M" << detLib.ModuleFromIndex(i)
               << "C" << detLib.ChannelFromIndex(i);
        
        tName << chInfo.GetType() << ":" << chInfo.GetSubtype()
              << ":" << i << ":" << "Rate";
        tTitle << "Rate Spectrum for M" << detLib.ModuleFromIndex(i)
               << "C" << detLib.ChannelFromIndex(i);
        
        TH1D *ceHist = new TH1D(ceName.str().c_str(), ceTitle.str().c_str(),
                                10000, 0., 5000.);
        TH1D *eHist  = new TH1D(eName.str().c_str(), eTitle.str().c_str(),
                                65536, 0., 32768);
        TH1D *tHist  = new TH1D(tName.str().c_str(), tTitle.str().c_str(),
                                65536, 0., 32768.);

        ceHstgrm.insert(make_pair(i,ceHist));
        eHstgrm.insert(make_pair(i, eHist));
        tHstgrm.insert(make_pair(i, tHist));
    }

    TH1D *hits = new TH1D("hitSpectrum", "Channel Hit Spectrum", numCh+1, 0.,
                          numCh+1);
    TH1D *mult = new TH1D("multiplicity", "Event Multiplicity", numCh+1, 0.,
                          numCh+1);
    TH1D *tdiff = new TH1D("tdiff", "Beam On/Off time diff", 5e3, 0.0, 5.);
    TH1D *cHist = new TH1D("ctdiff", "CsI Time Diff", 1e6, 0.0, 1e-4);
    TH1D *bHist = new TH1D("csI:large:0:dtOff", "Tdiff w BeamOff",
                           1.3e4, 0., 1.3e1);
    TH1D *tHist1 = new TH1D("csI:large:0:cpms", "CPmS",
                            5.e6, 0., 5.e3);
    TH1D *oHist = new TH1D("csI:large:0:dtOn", "Tdiff w BeamOn",
                           1.3e4, 0., 13.);
    
    TH2D *corrB = new TH2D("corr-b", "",
                           1e4, 0., 5.e3, 1e4, 0., 1e-3);
    TH2D *corrNb = new TH2D("corr-nb", "",
                            1e4, 0., 5.e3, 1e4, 0., 1e-3);
    TH2D *corre = new TH2D("corre", "",
                            1e4, 0., 5.e3, 1e4, 0., 5.e3);
    TH2D *etHstgrm = new TH2D("csI:large:0:TimeEnergy", "",
                              1.6e4, 0., 8.e3, 1.3e3, 0., 1.3e1);
    TH2D *cetHstgrm = new TH2D("csI:large:0:TimeCalEnergy", "",
                              1.6e4, 0., 8.e3, 1.3e3, 0., 1.3e1);
    TH2D *gtHstgrm = new TH2D("ge:ignore:0:TimeEnergy", "", 
                              1e4, 0., 5.e3, 1.3e4, 0., 13.);
    //---------- END HISTOGRAM DEFINITIONS ---------

    //initialize some of the times
    double firstTime = 0, onTime = 0, offTime = 0, csiTime = 0, 
        betaTime = 0, nonBetaTime = 0, betaEn = 0, nonBetaEn = 0;

    bool hasBetaTime = false, hasNonBetaTime = false;

    //How many cycles should we ignore for the missing data chunks?
    int numCycles = 4, cycleCount = 0;

    vector<double> csicE, csiE, csiT;

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
            //if(i == 1000)
            //    break;
            br->GetEntry(i);
            vector<ddaschannel*> evt = event->GetData();
            mult->Fill(evt.size());

            for(const auto &j : evt) {
                double slot  = j->GetSlotID();
                double chan = j->GetChannelID();
                double id = CalcId(slot, chan);
                double en = j->GetEnergy() + dist(rng);
                double time = j->GetTime();
                
                hits->Fill(id);

                //caluclate the Run Time in seconds.
                double runTime = (time - firstTime)*10.e-9;
                double timeBoff = (time - offTime)*10.e-9;
                double timeBon = (time - onTime)*10.e-9;
                
                //Continue through event if the id is not in the used list
                if(usdIds.find(id) == usdIds.end())
                    continue;
                
                //set the various times
                if(i == 0 && j == evt.at(0) && it == *files.begin())
                    firstTime = time;
                if(id == 4) {
                    if((time - onTime)*10.e-9 > 13.) {
                        cerr << "Oh man, we missed a beam on!! I am going to"
                             << " junk all the stored stats till now. " << endl;
                        csicE.clear();
                        csiE.clear();
                        csiT.clear();
                    }
                    onTime = time;
                    for(unsigned int i = 0; i < csiE.size(); i++) {
                        etHstgrm->Fill(csiE.at(i), csiT.at(i));
                        cetHstgrm->Fill(csicE.at(i), csiT.at(i));
                    }
                    csicE.clear();
                    csiE.clear();
                    csiT.clear();
                    
                    cycleCount++;
                }
                if(id == 5) {
                    tdiff->Fill((time-onTime)*10.e-9);
                    offTime = time;
                }
                
                if(id == 0) {
                    double calEn = cal.GetCsICal(en);
                    ceHstgrm.at(id)->Fill(calEn);
                    if(calEn < 3500 && calEn > 500 && !hasBetaTime) {
                        betaEn = calEn;
                        betaTime = time;
                        hasBetaTime = true;
                    }
                    if(calEn < 120 && calEn > 20 && hasBetaTime) {
                        nonBetaEn = calEn;
                        nonBetaTime = time;
                        hasNonBetaTime = true;
                    }

                    if(hasBetaTime && hasNonBetaTime) {
                        double corrTime = (nonBetaTime - betaTime)*10.e-9;
                        corrB->Fill(betaEn, corrTime);
                        corrNb->Fill(nonBetaEn, corrTime);
                        corre->Fill(betaEn, nonBetaEn);
                        hasBetaTime = hasNonBetaTime = false;
                        betaTime = nonBetaTime = 0;
                        betaEn = nonBetaEn = 0;
                    }

                    if(cycleCount > numCycles) {
                        if(onTime != 0) {
                            cHist->Fill((time - csiTime)*10.e-9);
                            csiTime = time;
                            
                            bHist->Fill(timeBoff);
                            oHist->Fill(timeBon);
                            csiE.push_back(en);
                            csicE.push_back(calEn);
                            csiT.push_back(timeBon);
                        }
                    }
                }//if(id == 0

                if(id == 16) {
                    double calEn = cal.GetGeCal(en);
                    ceHstgrm.at(id)->Fill(calEn);
                    if(onTime != 0 && cycleCount > numCycles)
                        gtHstgrm->Fill(calEn,timeBon);
                }
                
                eHstgrm.at(id)->Fill(en);
                tHstgrm.at(id)->Fill(runTime);
                tHist1->Fill(runTime);
            }//for(const auto j : evt)
        }//for(int i = 0; i < numEvent
        inFile.Close();
    }//for(auto it : files)

    //Save the histograms to a root file
    TFile outF(outFile.c_str(), "RECREATE");
    for(const auto &id : usdIds) {
        ceHstgrm.at(id)->Write();
        eHstgrm.at(id)->Write();
        tHstgrm.at(id)->Write();
    }
    //deadT->Write();
    corre->Write();
    corrB->Write();
    corrNb->Write();
    cHist->Write();
    hits->Write();
    mult->Write();
    bHist->Write();
    oHist->Write();
    tdiff->Write();
    tHist1->Write();
    cetHstgrm->Write();
    etHstgrm->Write();
    gtHstgrm->Write();
    outF.Write();
    cout << "Finishing up..." << endl;
}

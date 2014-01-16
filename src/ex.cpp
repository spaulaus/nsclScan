#include <iostream>
#include <vector>

#include <TTree.h>
#include <TFile.h>

#include "DDASEvent.h"

using namespace std;

int main(int argc, char* argv[]) {
    cout << "Testing this shit" << endl;

    TFile file("/mnt/analysis/e13504/svp/rootFiles/test00.root");
    TTree *tr = (TTree*)file.Get("dchan");
    TBranch *br = tr->FindBranch("ddasevent");

    DDASEvent *event = new DDASEvent(); 
    br->SetAddress(&event);

    int numEvent = tr->GetEntries();
    cout << numEvent << endl;

    for (int i = 0; i < numEvent; i++) {
        br->GetEntry(i); //Read complete accepted event in memory.
        vector<ddaschannel*> evt = event->GetData();
        if(evt.size() > 2) {
            cout << "Reading event Number " << i << endl
                 << "------------------------------" << endl;
            
            cout << "Boss, there are " << evt.size() << " events in this bitch."
                 << endl;
            for(auto j : evt) {
                unsigned int id = j->GetID();
                unsigned int sId  = j->GetSlotID();
                unsigned int crId = j->GetCrateID();
                unsigned int chId = j->GetChannelID();
                unsigned int en = j->GetEnergy();
                cout << id << " " << sId << " " << crId << " " 
                     << chId << " " << en << endl;
            }
            cout << endl << endl;
        }
    }
    //Delete ClonesArray and histogram objects
    event->Finish();
}

{
	gSystem->AddIncludePath("-I/mnt/analysis/e13504/svp/ddasdumper");
	gSystem->AddLinkedLibs(" -L/mnt/analysis/e13504/svp/ddasdumper");
	gSystem->Load("/mnt/analysis/e13504/svp/ddasdumper/libddaschannel.so");

	TFile inFile("/mnt/analysis/e13504/rootFiles/run223/run-0223-00.root");
	TTree *tr = (TTree*)inFile.Get("dchan");
	TBranch *br = tr->GetBranch("ddasevent");

	DDASEvent *event = new DDASEvent();
	br->SetAddress(&event);
	int numEvent = tr->GetEntries();

	for (int i = 0; i < numEvent; i++) {
		cout << "We are working on event number : " << i << endl;
		if(i == 1000)
			break;
		br->GetEntry(i);
		vector<ddaschannel*> evt = event->GetData();
		for(unsigned int j = 0; j < evt.size(); j++) {
			double slot  = evt.at(j)->GetSlotID();
			double chan = evt.at(j)->GetChannelID();
			double id = (slot-2)*16 + chan;
			double en = evt.at(j)->GetEnergy();
			double time = evt.at(j)->GetTime();
			cout << slot << " " << chan << " " << id << " " << en << " "
					<< time << endl;
		}
		cout << endl << endl;
	}
}

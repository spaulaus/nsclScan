{
    gROOT->Reset();
    
    TFile f("/mnt/analysis/e13504/svp/rootFiles/run145/run-0145-summed-bkp.root");
    TCanvas *c1 = new TCanvas("c1","c1",600,400);
    TCanvas *c2 = new TCanvas("c2","c2",600,400);

    TH2D *etCsi = (TH2D*)f.Get("csI:large:0:TimeEnergy;1");
    etCsi->SetAxisRange(0,2000,"X");

    c2->SetLogz();
    c1->SetLogz();
}

// TH1D *proj1 = (TH1D*)hist1->ProjectionX("proj1", 4000,11000);

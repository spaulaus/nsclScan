{
    gROOT->Reset();
    gROOT->SetStyle("Modern");
    gROOT->ForceStyle(kTRUE);
    //removes that statistics box
    gStyle->SetOptStat(kFALSE);
    gStyle->SetOptLogz();
    
    TFile f("/mnt/analysis/e13504/svp/rootFiles/run145/run-0145-summed-bkp.root");
    TCanvas *c1 = new TCanvas("c1","c1",600,400);
    c1->SetLogz();
    
    TH1D *h0 = (TH1D*)f.Get("csi:large:0:RawEnergy;1");
    h0->SetAxisRange(0,350e3,"Y");
    h0->SetAxisRange(0,2000,"X");
    h0->SetLineColor(kBlack);
    h0->Draw();
    c1->SaveAs("pics/energy/csi-run145.eps");

    h0 = (TH1D*)f.Get("csi:large:0:Rate;1");
    h0->SetAxisRange(0,4000,"X");
    h0->SetLineColor(kBlack);
    h0->Draw();
    c1->SaveAs("pics/rates/csi-run145.eps");

    h0 = (TH1D*)f.Get("csI:large:0:dtOff;1");
    h0->SetLineColor(kBlack);
    h0->Draw();
    c1->SaveAs("pics/cycles/csiOff-run145.eps");

    h0 = (TH1D*)f.Get("csI:large:0:dtOn;1");
    h0->SetLineColor(kBlack);
    h0->Draw();
    c1->SaveAs("pics/cycles/csiOn-run145.eps");

    //Stuff for the Ge
    h0 = (TH1D*)f.Get("ge:ignore:16:RawEnergy;1");
    h0->SetLineColor(kBlack);
    h0->SetAxisRange(0,2300,"Y");
    h0->Draw();
    c1->SaveAs("pics/energy/geRaw-run145.eps");
    
    h0 = (TH1D*)f.Get("ge:ignore:16:CalEn;1");
    h0->SetLineColor(kBlack);
    h0->SetAxisRange(0,2000,"X");
    h0->Draw();
    c1->SaveAs("pics/energy/geCal-run145.eps");

    h0 = (TH1D*)f.Get("ge:ignore:16:Rate;1");
    h0->SetAxisRange(0,4000,"X");
    h0->SetLineColor(kBlack);
    h0->Draw();
    c1->SaveAs("pics/rates/ge-run145.eps");
    
    //Logic and beam realted stuff
    gStyle->SetOptStat(kTRUE);
    
    h0 = (TH1D*)f.Get("logic:beamOn:4:Rate;1");
    h0->SetAxisRange(0,4000,"X");
    h0->SetLineColor(kBlack);
    h0->Draw();
    c1->SaveAs("pics/rates/bon0-run145.eps");

    h0 = (TH1D*)f.Get("logic:beamOff:5:Rate;1");
    h0->SetAxisRange(0,4000,"X");
    h0->SetLineColor(kBlack);
    h0->Draw();
    c1->SaveAs("pics/rates/boff0-run145.eps");

    h0 = (TH1D*)f.Get("pulser:ignore:6:Rate;1");
    h0->SetAxisRange(0,4000,"X");
    h0->SetLineColor(kBlack);
    h0->Draw();
    c1->SaveAs("pics/rates/plsr0-run145.eps");

    h0 = (TH1D*)f.Get("logic:beamOn:20:Rate;1");
    h0->SetAxisRange(0,4000,"X");
    h0->SetLineColor(kBlack);
    h0->Draw();
    c1->SaveAs("pics/rates/bon1-run145.eps");

    h0 = (TH1D*)f.Get("logic:beamOff:21:Rate;1");
    h0->SetAxisRange(0,4000,"X");
    h0->SetLineColor(kBlack);
    h0->Draw();
    c1->SaveAs("pics/rates/boff1-run145.eps");

    h0 = (TH1D*)f.Get("pulser:ignore:22:Rate;1");
    h0->SetAxisRange(0,4000,"X");
    h0->SetLineColor(kBlack);
    h0->Draw();
    c1->SaveAs("pics/rates/plsr1-run145.eps");

    //Plot the 2D Monsters
    TH2D *h2 = (TH2D*)f.Get("csI:large:0:TimeEnergy;1");
    h2->SetAxisRange(0,3000,"X");
    h2->Draw("COL");
    c1->SaveAs("pics/timeVsEnergy/csi-run145.eps");

    h2 = (TH2D*)f.Get("ge:ignore:0:TimeEnergy;1");
    c1->SetLogz(0);
    h2->Draw("COL");
    c1->SaveAs("pics/timeVsEnergy/ge-run145.eps");
}

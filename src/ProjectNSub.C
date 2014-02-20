//A simple macro to project and subtract the various regions
{
    TH2D *primHist = (TH2D*)f.Get("csI:large:0:TimeEnergy");

    //---------- The waiting period ---------
    TH1D *tw = (TH1D*)primHist->ProjectionX("tw",2500,4499);

    //---------- The various projections ---------
    TH1D *s1 = (TH1D*)primHist->ProjectionX("s1",4500,5499);
    TH1D *s2 = (TH1D*)primHist->ProjectionX("s2",5500,8499);
    TH1D *b2 = (TH1D*)primHist->ProjectionX("b2",8500,11499);
    TH1D *b1 = (TH1D*)primHist->ProjectionX("b1",11500,12499);

    TH1D *sub1 = new TH1D ("sub1", "", 1e4, 0., 5.e3);
    sub1->Add(s1,b1,1,-1);
    TH1D *sub2 = new TH1D ("sub2", "", 1e4, 0., 5.e3.);
    sub2->Add(s2,b2,1,-1);
    
    sub1->SetXTitle("Energy (keV)");
    sub1->SetYTitle("Counts/(0.5 keV)");
    sub1->SetAxisRange(1,4000,"X");
    sub1->SetLineColor(2000);
    sub1->Draw();
    sub2->SetLineColor(2018);
    sub2->Draw("same");

    TH1D *tot = new TH1D("tot","tot", 1e4, 0., 5.e3.);
    tot->Add(sub1,sub2,1,1);
        
    //---------- An even 2 section cut starting after the wait ---------
    TH1D *s3 = (TH1D*)primHist->ProjectionX("s3",4500,8499);
    TH1D *b3 = (TH1D*)primHist->ProjectionX("b3",8500,12499);

    TH1D *sub3 = new TH1D("sub3","sub3", 1e4, 0., 5.e3.);
    sub3->Add(s3,b3,1,-1);

    //---------- An even 2 section cut ignoring the wait ----------
    TH1D *s4 = (TH1D*)primHist->ProjectionX("s4",2500,7499);
    TH1D *b4 = (TH1D*)primHist->ProjectionX("b4",7500,12499);

    TH1D *sub4 = new TH1D("sub4","sub4", 1e4, 0., 5.e3.);
    sub3->Add(s4,b4,1,-1);

    //---------- Project every 500 ms ----------
    // c2->cd();
    // TH1D *p0 = (TH1D*)primHist->ProjectionX("p0",2500.,3500.);
    // p0->SetLineColor(1);
    // p0->Draw();
    // TH1D *p1 = (TH1D*)primHist->ProjectionX("p1",3500.,4500.);
    // p1->SetLineColor(2);
    // p1->Draw("same");
    // TH1D *p2 = (TH1D*)primHist->ProjectionX("p2",4500.,5500.);
    // p2->SetLineColor(3);
    // p2->Draw("same");
    // TH1D *p3 = (TH1D*)primHist->ProjectionX("p3",5500.,6500.);
    // p3->SetLineColor(4);
    // p3->Draw("same");
    // TH1D *p4 = (TH1D*)primHist->ProjectionX("p4",6500.,7500.);
    // p4->SetLineColor(5);
    // p4->Draw("same");
    // TH1D *p5 = (TH1D*)primHist->ProjectionX("p5",7500.,8500.);
    // p5->SetLineColor(6);
    // p5->Draw("same");
    // TH1D *p6 = (TH1D*)primHist->ProjectionX("p6",8500.,9500.);
    // p6->SetLineColor(7);
    // p6->Draw("same");
    // TH1D *p7 = (TH1D*)primHist->ProjectionX("p7",9500.,10500.);
    // p7->SetLineColor(8);
    // p7->Draw("same");
    // TH1D *p8 = (TH1D*)primHist->ProjectionX("p8",10500.,11500.);
    // p8->SetLineColor(9);
    // p8->Draw("same");
    // TH1D *p9 = (TH1D*)primHist->ProjectionX("p9",11500.,12500.);
    // p9->SetLineColor(2000);
    // p9->Draw("same");
}

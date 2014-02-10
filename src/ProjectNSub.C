//A simple macro to project and subtract the various regions
{
    TH2D *primHist = (TH2D*)f.Get("csI:large:0:TimeEnergy;1");

    TH1D *tw = (TH1D*)primHist->ProjectionX("tw",2500,4499);

    TH1D *s1 = (TH1D*)primHist->ProjectionX("s1",4500,5499);
    TH1D *s2 = (TH1D*)primHist->ProjectionX("s2",5500,8499);
    TH1D *b2 = (TH1D*)primHist->ProjectionX("b2",8500,11499);
    TH1D *b1 = (TH1D*)primHist->ProjectionX("b1",11500,12499);

    TH1D *s3 = (TH1D*)primHist->ProjectionX("s3",4500,8499);
    TH1D *b3 = (TH1D*)primHist->ProjectionX("b3",8500,12499);

    TH1D *sub1 = new TH1D ("sub1", "sub1", 8192, 0., 8192.);
    sub1->Add(s1,b1,1,-1);
    TH1D *sub2 = new TH1D ("sub2", "sub2", 8192, 0., 8192.);
    sub2->Add(s2,b2,1,-1);

    TH1D *sub3 = new TH1D("sub3","sub3", 8192, 0., 8192.);
    sub3->Add(s3,b3,1,-1);

    TH1D *tot = new TH1D("tot","tot", 8192, 0., 8192.);
    tot->Add(sub1,sub2,1,1);
    tot->SetAxisRange(1,1600,"X");
    tot->Draw();
}

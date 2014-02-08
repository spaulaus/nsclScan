//A simple macro to project and subtract the various regions
{
    TH2D *primHist = (TH2D*)f.Get("csI:large:0:TimeEnergy;1");

    TH1D *tw = (TH1D*)primHist->ProjectionX("tw",2500,4499);

    TH1D *s1 = (TH1D*)primHist->ProjectionX("s1",4500,6499);
    TH1D *b1 = (TH1D*)primHist->ProjectionX("s2",6500,8499);
    TH1D *s2 = (TH1D*)primHist->ProjectionX("b1",8500,10499);
    TH1D *b2 = (TH1D*)primHist->ProjectionX("b2",10500,12499);

    TH1D *s3 = (TH1D*)primHist->ProjectionX("s3",4500,8499);
    TH1D *b3 = (TH1D*)primHist->ProjectionX("b3",8500,12499);

    TH1D *sub1 = s1;
    sub1->Add(b1,-1);
    TH1D *sub2 = s2;
    sub2->Add(b2,-1);

    TH1D *sub3 = s3;
    sub3->Add(b3,-1);

    TH1D *tot = sub1;
    tot->Add(sub2,1);
    tot->SetAxisRange(1,1600,"X");
    tot->Draw();
}

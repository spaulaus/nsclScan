/* \file fitting.C
 * \brief Root macro for a Fit to the beta spectrum 
 *
 * The fitting function comes from the following reference
 * M. Morita, Theory of Beta Decay, Prog. Theoretical Phys., 1963
 *
 * The Fermi part comes from J. Kantele
 *
 * \author S. V. Paulauskas
 * \date 03-12-2014
 */
//---------- Specify the file to be worked on ----------
TFile file("data/test/test00.root");

//---------- Physics constants ----------
const double me = 510.998910; // keV/c/c;

//---------- The fitting region ----------
const double fLow = 200, fHigh = 3200;
const double fLow = 1000, fHigh = 7500;
double FermiFunc(const double &e) {
    const double z = 3;

    //Expects energy in kev
    double m[8][11] = {
        {1, .3269, -.12948, .019016, -.00095348, 0, .0272646, -.0004201, -9.5474E-6, 0, 0}, 
        {10, 2.08738, -.57928, .0404785, .00305364, -.000334, .11416, .043251, -.0033661, 0, 0}, 
        {20, 2.80848, -.404105, -.068659, .019067, -.0010833, .127189, .128762, -.010038, 0, 0}, 
        {30, 3.33244, -.300744, -.11193, .023452, -.0012114, .083368, .244539, -.019246, 0, 0}, 
        {50, 4.38334, -.319865, -.091311, .016894, -.0007709, .047726, .515291, -.0424, 0, 0}, 
        {70, 5.51184, -.38239, -.058665, .0107613, -.000491881, .515219, .738253, -.066362, 0, 0}, 
        {90, 6.76374, -.41807, -.042962, .0086018, -.00051951, 2.52482, .66674, -.076984, 0, 0}, 
        {100, 7.4614, -.437171, -.035496, .0077881, -.00057477, 4.4908, .42503, -.071491, 0, 0} };

    int g;
    if(e > 2000)
        g = 5.;
    else
        g = 0.;

    int idx;
    for(int i = 0; i < 8; i++) {
        if( (m[i+1][0] - z) >= 0) {
            idx = i;
            break;
        }
    }
    
    double y[8];
    for(int j = idx; j <= idx+1; j++) {
        y[j] = exp(m[j][1 + g] + 
                   m[j][2 + g] * log(e) + 
                   m[j][3 + g] * pow(log(e),2) + 
                   m[j][4 + g] * pow(log(e),3) + 
                   m[j][5 + g] * pow(log(e),4) );
    }
    double f = exp(log(y[idx]) + 
                   ( z - m[idx][0] ) * 
                   ( log(y[idx + 1]) - log(y[idx]) ) / 
                   ( m[idx + 1][0] - m[idx][0] ) );
    return(f);
}

//---------- FITTING FUNCTION ----------
Double_t FitFunc(Double_t *x, Double_t *par) {
    double A = par[0];
    double a = par[1];
    double Q = par[2];
    double E = x[0];
    double W = E + me;

    //----------- MOMENTUM AND TOTAL ENERGY OF ELECTRON -----------
    double p  = sqrt(E*E + 2 * me * E);

    //---------- SHAPE FACTOR ----------
    double shape = 1. + a * E;

    if(E > Q) 
        return(0.);
    else
        return(A * FermiFunc(E) * p * W * pow(Q - E, 2) * shape);
}

//---------- BEGIN THE MAIN FITTING ROUTINE ---------
int fitting(void) {
    //The 2d histogram for the projections
    TH2D *primHist = (TH2D*)file.Get("csI:large:0:TimeCalEnergy");
    
    //---------- The waiting period ---------
    TH1D *tw = (TH1D*)primHist->ProjectionX("tw",250,499);
    
    //---------- The various projections ---------
    TH1D *s1 = (TH1D*)primHist->ProjectionX("s1",500,599);
    TH1D *s2 = (TH1D*)primHist->ProjectionX("s2",600,874.9);
    TH1D *b2 = (TH1D*)primHist->ProjectionX("b2",875,1149.9);
    TH1D *b1 = (TH1D*)primHist->ProjectionX("b1",1150,1249.9);
    
    TH1D *sub1 = new TH1D ("sub1", "", 1.6e4, 0., 8.e3);
    sub1->Add(s1,b1,1,-1);
    TH1D *sub2 = new TH1D ("sub2", "", 1.6e4, 0., 8.e3);
    sub2->Add(s2,b2,1,-1);
    
    //---------- DEFINE FITTING FUNCTION AND DO THE FIT ----------
    TF1 *mf=new TF1("Func", FitFunc, fLow, fHigh, 3);
    
    double ampInit   = 3.0e-11;
    double c1Init    = 1e-4;
    double endptInit = 3505;

    mf->SetParameters(ampInit, c1Init, endptInit);
    mf->SetParLimits(1, 1e-19, 1e6);
    mf->SetParLimits(1, -10, 10);
    mf->SetParLimits(2, 0., 1e6);
    
    bool doFit = false;
    bool doOutput = false;
    if(doFit) {
        cout << endl << "Fit with MEN... *chuckle*" << endl;
        sub1->Fit("Func", "MEN", "", fLow, fHigh);
        
        //----------- PRINT RESULTS ----------
        cout << "***Results from last fit***"<<endl;
        cout << "Amplitude = " << mf->GetParameter(0) << " +- " 
             << mf->GetParError(0) << endl;
        cout << "C1 = " << mf->GetParameter(1) << " +- " 
             << mf->GetParError(1) << endl;
        cout << "End-Point = " << mf->GetParameter(2) << " +- " 
             << mf->GetParError(2) << endl;
        cout << "chi2= " << mf->GetChisquare() << endl;
        cout << "dof= " << mf->GetNDF() << endl;
        cout << "chi2/dof = " << mf->GetChisquare() / mf ->GetNDF() << endl;
        cout << "***************************" << endl << endl;

        if(doOutput) {
            ofstream out("/projects/e13504/Analysis/svp/data/fitstudy/cal-wF.dat",
                         ios::app);
            out << fLow << " " << fHigh << " " 
                << mf->GetParameter(0) << " " 
                << mf->GetParError(0) << " " 
                << mf->GetParameter(1) << " " 
                << mf->GetParError(1) << " " 
                << mf->GetParameter(2) << " " 
                << mf->GetParError(2) << " " 
                << mf->GetParameter(3) << " " 
                << mf->GetParError(3) << " " 
                << mf->GetChisquare() << " " 
                << mf->GetNDF() << " "
                << mf->GetChisquare()/mf->GetNDF() << endl;
            out.close();
        }
    }else
        cout << "Not performing the fit simply printing the function " 
             << "with the following values: " << endl << "A = " 
             << ampInit << endl << "a = " << c1Init << endl << "Q = "
             << endptInit << endl;
            
    
    //---------- DISPLAY SPECTRA -----------
    TCanvas *c8=new TCanvas("c8","Results-1");
    c8->Clear();
    c8->cd();
    
    sub1->SetAxisRange(fLow,fHigh,"X");
    sub1->Draw();
    mf->SetLineColor(2);
    mf->Draw("same");
    c8->Update();

    cout  <<  "*** THE END ***" << endl;
    //return(0);
    exit(0);
}


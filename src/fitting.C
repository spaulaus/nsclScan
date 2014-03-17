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

// Physics constants
const double me = 510.998910; // keV/c/c;

const double fLow = 500, fHigh = 4000;

// Energy calibration coefficients
//double Acal = 0.558823;
//double Bcal = -8.06648;

//---------- FITTING FUNCTION ----------
Double_t FitFunc(Double_t *x, Double_t *par) {
    double amp   = par[0];
    double c1    = par[1];
    double endPt = par[2];
    double betaE = x[0];

    //----------- MOMENTUM AND TOTAL ENERGY OF ELECTRON -----------
    double betaMom  = sqrt(betaE * betaE + 2. * me * betaE);
    double E = betaE + me;

    const double z = 3;
    double coeff[4][4] = { {-17.2, 7.9015, -2.54, 0.28482,}, 
                           {3.31368, -2.06273, 0.703822, -0.075039,}, 
                           {-0.364018, 0.387961, -0.142528, 0.016,},
                           {0.0278071, -0.026519, 0.0098854, -0.00113772} };
    double evalCoeff[4];
    for(int i = 0; i < 4; i++)
        evalCoeff[i] = coeff[i][0] + log(z) * coeff[i][1] + 
            coeff[i][2]*pow(log(z),2.) + coeff[i][3]*pow(log(z),3.);
    double logf = evalCoeff[0] + evalCoeff[1]*log(endPt) + 
        evalCoeff[2]*pow(log(endPt),2.) + evalCoeff[3]*pow(log(endPt),3.);
    double f = pow(10,logf);
    
    //---------- SHAPE FACTOR ----------
    double shape = 1. + c1 * E;

    if(E > endPt) 
        return(0.);
    else
        return(amp * (1/f) * betaMom * E * pow(endPt - E, 2) * shape);
}

//---------- BEGIN THE MAIN FITTING ROUTINE ---------
int fitting(void) {
    //The 2d histogram for the projections
    TH2D *primHist = (TH2D*)file.Get("csI:large:0:TimeEnergy");
    
    //---------- The waiting period ---------
    TH1D *tw = (TH1D*)primHist->ProjectionX("tw",2500,4999);
    
    //---------- The various projections ---------
    TH1D *s1 = (TH1D*)primHist->ProjectionX("s1",5000,5999);
    TH1D *s2 = (TH1D*)primHist->ProjectionX("s2",6000,8749);
    TH1D *b2 = (TH1D*)primHist->ProjectionX("b2",8750,11499);
    TH1D *b1 = (TH1D*)primHist->ProjectionX("b1",11500,12499);
    
    TH1D *sub1 = new TH1D ("sub1", "", 1e4, 0., 5.e3);
    sub1->Add(s1,b1,1,-1);
    TH1D *sub2 = new TH1D ("sub2", "", 1e4, 0., 5.e3);
    sub2->Add(s2,b2,1,-1);
    
    //---------- DEFINE FITTING FUNCTION AND DO THE FIT ----------
    TF1 *mf=new TF1("Func", FitFunc, fLow, fHigh, 3);
    
    double ampInit   = 2.27e-11;
    double c1Init    = -2.526e-4;
    double endptInit = 4483.94;

    mf->SetParameters(ampInit, c1Init, endptInit);
    mf->SetParLimits(1, 1e-19, 10);
    mf->SetParLimits(1, -10, 10);
    mf->SetParLimits(2, 0., 1e6);
    
    bool doFit = true;
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
    }else
        cout << "Not performing the fit simply printing the function" 
             << "with the following values: " << endl << "amp = " 
             << amp << endl << "c1 = " << c1Init << endl << " EndPoint = "
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
    return(0);
}


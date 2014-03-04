//Macro dkyFunk.C
//---------- Beta Spectrum ---------
Double_t BetaFunc(Double_t *x, Double_t *par)
{
    // Energy calibration coefficients
    double Acal = 1000.;
    double Bcal = 0.;

    // Physics constants
    const double me=0.510998910;
    const double amu=931.494028;		// in MeV

    Int_t z;
    Double_t c_bsim;
    z=sub2->GetXaxis()->FindFixBin(x[0]);
    c_bsim=sub2->GetBinContent(z);
    
    // Convert channel number to total energy
    Double_t Etot=x[0]/Acal + me;
    return( par[0]*c_bsim*(1. + par[1]*Etot) );
}

void LoadBetaFunc() {
    TF1 *BetaSpec = new TF1("BetaSpec", BetaFunc, 500, 4000., 2);
    BetaSpec->SetParameters(1.,1.);
    BetaSpec->SetParLimits(0,-10.,10.);
    BetaSpec->SetParLimits(1,-10.,10.);
}

//---------- A SINGLE DECAY EXPONENTIAL ----------
Double_t DkyExpFunc(Double_t *x, Double_t *par) {
    Double_t amp = par[0];
    Double_t tau = par[1];
    Double_t off = par[2];
    return(amp*exp(-x[0]/tau)+off);
}

void LoadDkyExpFunc() {
    TF1 *DkyExp = new TF1("DkyExp", DkyExpFunc, 0, 13, 3);
    DkyExp->SetParameters(1e3, 1.16281, 100.);
}


//---------- A GAUSSIAN FUNCTION ---------
Double_t GaussFunc(Double_t *x, Double_t *par) {
    const double pi=TMath::Pi();
    Double_t amp = par[0];
    Double_t sigma = par[1];
    Double_t mu = par[2];
    Double_t offset = par[3];
    return(((amp/(sigma*sqrt(2*pi))) * 
           exp(-(x[0]-mu)*(x[0]-mu)/(2*sigma*sigma)))+offset);
}

void LoadGaussFunc() {
    TF1 *Gauss = new TF1("Gauss", GaussFunc, -1e5, 1e5, 4);
    Gauss->SetParameters(1.e5, 40.63, 1175., 0.);
}


//---------- A GROW AND DECAY CURVE ----------
Double_t GrowDecayFunc(Double_t *x, Double_t *par) {
    Double_t amp = par[0];
    Double_t tau = par[1];
    Double_t x0  = par[2];
    if(x[0] < x0)
        return(amp*(1-exp(-x[0]/tau)));
    else
        return(amp*(1-exp(-x0/tau))*exp(-(x[0]-x0)/tau));
}

void LoadGrowDecayFunc(){
    TF1 *GrowDecay = new TF1("GrowDecay",GrowDecayFunc, 0, 2000.,3);
    GrowDecay->SetParameters(1e2,1100.,2.5);
}


//---------- Asymmetric Gaussian from Radware ----------
Double_t RadGaussFunc(Double_t *x, Double_t *par) {
}

void LoadRadGaussian() {
    TF1 *RadGaussian = new TF1("RadGaussian", RadGaussFunc, -1e5, 1e5, 4);
    RadGaussian->SetParameters(1.e5, 40.63, 1175., 0.);
}



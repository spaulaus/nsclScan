//Macro dkyFunk.C

Double_t SingleDkyExp(Double_t *x, Double_t *par) {
    Double_t amp = par[0];
    Double_t tau = par[1];
    Double_t off = par[2];
    return(amp*exp(-x[0]/tau)+off);
}

Double_t Gaussian(Double_t *x, Double_t *par) {
    const double pi=TMath::Pi();
    Double_t amp = par[0];
    Double_t sigma = par[1];
    Double_t mu = par[2];
    Double_t offset = par[3];
    return(((amp/(sigma*sqrt(2*pi))) * 
           exp(-(x[0]-mu)*(x[0]-mu)/(2*sigma*sigma)))+offset);
}

Double_t GrowDecayCurve(Double_t *x, Double_t *par) {
    Double_t amp = par[0];
    Double_t tau = par[1];
    Double_t x0  = par[2];
    if(x[0] < x0)
        return(amp*(1-exp(-x[0]/tau)));
    else
        return(amp*(1-exp(-x0/tau))*exp(-(x[0]-x0)/tau));
}

Double_t RadGaussian(Double_t *x, Double_t *par) {
    


}

void LoadSingleDkyExp() {
    TF1 *SingleDkyExp = new TF1("SingleDkyExp", SingleDkyExp, 0, 13, 3);
    SingleDecayExp->SetParameters(1e3, 1.16281, 100.);
}

void LoadGaussian() {
    TF1 *Gaussian = new TF1("Gaussian", Gaussian, -1e5, 1e5, 4);
    Gaussian->SetParameters(1.e5, 40.63, 1175., 0.);
}

void LoadRadGaussian() {
    TF1 *RadGaussian = new TF1("RadGaussian", RadGaussian, -1e5, 1e5, 4);
    RadGaussian->SetParameters(1.e5, 40.63, 1175., 0.);
}


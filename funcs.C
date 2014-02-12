//Macro dkyFunk.C

Double_t SingleDkyExp(Double_t *x, Double_t *par) {
    Double_t amp = par[0];
    Double_t tau = par[1];
    Double_t off = par[2];
    return(amp*exp(-x[0]/tau)+off);
}

void LoadSingleDkyExp() {
    TF1 *f1 = new TF1("f1", SingleDkyExp,0, 13, 3);
    f1->SetParameters(0,1e3);
    f1->SetParameters(1,1.16281);
    f1->SetParameters(2,100.);
}

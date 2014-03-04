//*******************************************************//
// Root macro for a Monte-Carlo fit of beta spectrum
// Author : O. Naviliat-Cunicic
//*******************************************************//
// Math constants
const double pi=TMath::Pi();

// Physics constants
const double me=0.510998910;
const double amu=931.494028; // in MeV

// Constants for the beta decay
inline double getQval(void) {
    double mHe6 = 6.018885883; // amu, see PRL 106 (2012) 052504
    double emHe6 = 0.000000057;
    double mLi6 = 6.0151228874; // amu, see PRA 82 (2010) 042513
    double emLi6 = 0.0000000016;
    return( (mHe6-mLi6)*amu );
}

const double Qb=getQval();
const double W=Qb+me;
const double FMAX=20.;	//Maximum Y-axis value for generating the beta spectrum

// Energy calibration coefficients
double Acal = 1000.;
double Bcal = 0.;

// Global variables
const int NSYST=11; // Number of different configurations for systematic effects
TH1F *hTbeta[3];
TH1F *hTbeta_diff[3];
TH1F *hTbeta_syst[NSYST];
const Char_t* C1_typ[]={"C1_exp","C1_sim","C1_fit"};

// Main driver for the 
// Monte-Carlo fit of the beta spectrum from He6 decay;
int syst_C1(void) {
    // Book histograms
    int nCHAN=4000;
    double cMIN=0.,cMAX=4000.;
    for(int ib=0; ib<3; ib++)
        hTbeta[ib] = new TH1F(Form("hTbeta_%s",C1_typ[ib]), 
                              Form("hTbeta_%s",C1_typ[ib]),
                              nCHAN,cMIN,cMAX);
    for(int ib=0; ib<3; ib++)
        hTbeta_diff[ib] = new TH1F(Form("hTbeta_diff_%s",C1_typ[ib]),
                                   Form("hTbeta_diff_%s",C1_typ[ib]),
                                   nCHAN,cMIN,cMAX);
    for(int is=0; is<NSYST; is++)
        hTbeta_syst[is] =new TH1F(Form("hTbeta_syst_%d",is),
                                  Form("hTbeta_syst_%d",is),
                                  nCHAN,cMIN,cMAX);
    // Parameters
    // Linear term in the shape factor
    double C1[2];
    C1[0]=0.01;  // Experimental
    C1[1]=0.;    // Simulation
    
    // Low energy thresholds (in MeV)
    double T_min[2];
    T_min[0]=0.200;
    T_min[1]=0.200;
    
    double E_rms_1MeV[2],E_rms_0MeV[2],syst[NSYST],delta_E_rms,rms;
    E_rms_1MeV[0]=0.04;
    E_rms_1MeV[1]=0.04;
    delta_E_rms=0.002;
    E_rms_0MeV[0]=0.01;
    E_rms_0MeV[1]=0.01;
    
    double MAXEVTS[2];
    MAXEVTS[0]=1.0e5;
    MAXEVTS[1]=5.0e6;
    int MODULO_print = MAXEVTS[0]/10.;
    
    // Calculate the size of a systematic effect to be implemented
    for(int is = 0; is < NSYST; is++) {
        // Response function
        //syst[is]=E_rms_1MeV[0]+delta_E_rms*(double(is)-5.);
        
        // Deviation from linearity
        syst[is]=double(is-NSYST/2.)/1000.;
    }
    
    // Kinematic variables
    double pe,Te,E;
    
    // The first loop generates the experimental spectrum; 
    for(int ib = 0; ib < 2; ib++) {
        // the second loop generates the spectrum for the Monte-Carlo fit
        for(int nevts = 1; nevts < MAXEVTS[ib]+1; nevts++) {	
            double ftir=FMAX;
            double fres=0.;
            
            // Generate kinematic variables following the beta spectrum
            while(ftir>fres) {
                // (this neglects the energy of the recoiling nucleus)
                ftir = gRandom->Rndm()*FMAX;
                Te = gRandom->Rndm()*Qb;
                
                pe = sqrt(Te*Te+2.*me*Te);
                E = Te + me;
                fres = pe * E * pow(Qb-Te,2.) * (1. + C1[ib]*E);
            } // got a new valid event
            
            // Tell the world where we are
            if( nevts % MODULO_print == 0 ) 
                cout << setw(8) << nevts << " generated events in loop " 
                     << ib+1 << endl;
            
            // Transform physical value to channel number
            // Energy calibration
            Double_t Te_chan;
            Te_chan=Acal*Te + Bcal;
            
            // Detector response function
            // rms=sqrt( Te_chan*E_rms_1MeV[ib]*E_rms_1MeV[ib] + 
            //           E_rms_0MeV[ib]*E_rms_0MeV[ib] );
            // Te_chan=Te_chan+gRandom->Gaus(0,rms);
            
            // Increment spectrum
            if(Te>T_min[ib]) hTbeta[ib]->Fill(Te_chan);
            
            // Induce systematic effect on the experimental 
            // spectrum and increment spectra
            if(ib==0) {
                for(int is=0; is<NSYST; is++) {
                    // Detector response function
                    // rms=sqrt( Te_chan*syst[is]*syst[is] + 
                    //           E_rms_0MeV[ib]*E_rms_0MeV[ib] );
                    // Te_chan=Te_chan+gRandom->Gaus(0,rms);
                    
                    // Linearity of calibration
                    Te_chan=Te*(1. + syst[is])*Acal + Bcal;
                    
                    // Increment spectra
                    if(Te>T_min[ib]) hTbeta_syst[is]->Fill(Te_chan);
                }	
            }//if(ib==0)
            
        }//for(int nevents = 1   End of loop over events
        cout << endl << "End of loop " << ib+1 << endl << endl;	
        
    } //for(int ib = 0;  End of loop over spectra
    
    cout << "Start fits..." << endl;
    
    //***********************************************************//
    // At this stage the spectra have been generated, now do fits//
    //***********************************************************//
    Double_t Fit_E(Double_t *, Double_t *);
    TF1 *mf=new TF1("Fit_E_f",Fit_E,cMIN,cMAX,2);
    
    mf->SetParameters(1.,1.);
    mf->SetParLimits(0,-10.,10.);
    mf->SetParLimits(1,-10.,10.);
    
    double E_fitmin,E_fitmax;
    E_fitmin=200;
    E_fitmax=2700;
    
    // Fit of the experimental spectrum
    cout << endl << "Fit with MEN..." << endl;
    hTbeta[0]->Fit("Fit_E_f","MEN","",E_fitmin,E_fitmax);
    cout << endl << "Fit with LMEN..." << endl;
    hTbeta[0]->Fit("Fit_E_f","LMEN","",E_fitmin,E_fitmax);
    
    double A_fit,err_A_fit,C1_fit,err_C1_fit;
    double chi2,dof;
    A_fit=mf->GetParameter(0);
    C1_fit=mf->GetParameter(1);
    err_A_fit=mf->GetParError(0);
    err_C1_fit=mf->GetParError(1);
    chi2=mf->GetChisquare();
    dof=mf->GetNDF();
    
    cout<< "***Results from last fit***"<<endl;
    cout<< "A_fit="<<A_fit<<" +- "<<err_A_fit<<endl;
    cout<< "C1_fit="<<C1_fit<<" +- "<<err_C1_fit<<endl;
    cout<< "C1_exp="<<C1[0]<<endl;
    cout<< "chi2="<<chi2<<endl;
    cout<< "dof="<<dof<<endl;
    cout<< "***************************"<<endl<<endl;
    
    // Calculate and fill histogram with the results from the fit
    int nbBins=hTbeta[0]->GetNbinsX();
    double binVal,Etot;
    for(int i=1; i<nbBins+1; i++) {
        // Convert channel number to toal energy
        Etot = hTbeta[1]->GetBinCenter(i)/1000. + me;
        binVal = A_fit*hTbeta[1]->GetBinContent(i)*(1.+C1_fit*Etot);
        hTbeta[2]->SetBinContent(i,binVal);
    }
    
    ////////Calcul des différences exp-sim
    hTbeta_diff[0]->Add(hTbeta[0],1.);
    hTbeta_diff[0]->Add(hTbeta[1],-A_fit);
    hTbeta_diff[1]->Add(hTbeta[1],A_fit);
    hTbeta_diff[1]->Add(hTbeta[1],-A_fit);
    hTbeta_diff[2]->Add(hTbeta[2],1.);
    hTbeta_diff[2]->Add(hTbeta[1],-A_fit);
    
    //////////Fit syste/////////////////////////////
    TF1 *mf_syst[NSYST];
    
    // Open output file for prints
    TString filename="systEff.dat";
    ofstream outfile(filename.Data());
    
    cout.flags(ios::left);
    cout << setw(11) << "Sys.effect" << setw(11) 
         << "C1_fit" << setw(11) 
         << "err_C1_fit" << setw(11) << "chi2" << setw(11) << "ndf" 
         << setw(11) << "chi2/ndf" << endl;
    outfile.flags(ios::left);
    outfile << setw(11) << "Sys.effect" << setw(11) << "C1_fit" << setw(11) 
            << "err_C1_fit" << setw(11) << "chi2" << setw(11) << "ndf" 
            << setw(11) << "chi2/ndf" << endl;
    
    for(int is = 0; is < NSYST; is++) {		
        mf_syst[is]=new TF1(Form("Fit_E_f_syst%d",is),Fit_E,0.,4000.,2);
        mf_syst[is]->SetParameters(1.,1.);
        
        mf_syst[is]->SetParLimits(0,-10.,10.);
        mf_syst[is]->SetParLimits(1,-10.,10.);
        
        hTbeta_syst[is]->Fit(Form("Fit_E_f_syst%d",is),"MENQ","",
                             E_fitmin,E_fitmax);
        //hTbeta_syst[is]->Fit("Fit_E_f_syst","LLMENQ","",E_fitmin,E_fitmax);
        
        A_fit=mf_syst[is]->GetParameter(0);
        C1_fit=mf_syst[is]->GetParameter(1);
        
        err_A_fit=mf_syst[is]->GetParError(0);
        err_C1_fit=mf_syst[is]->GetParError(1);
        
        chi2=mf_syst[is]->GetChisquare();
        dof=mf_syst[is]->GetNDF();
        
        cout.flags(ios::left);
        cout << setw(11) << syst[is] << setprecision(4) 
             << setw(11) << C1_fit 
             << setw(11) << err_C1_fit << setw(11) << chi2 << setw(11) << dof 
             << setw(11) << chi2/dof << endl;
        outfile.flags(ios::left);
        outfile << setw(11) << syst[is] << setprecision(4) << setw(11) 
                << C1_fit << setw(11) << err_C1_fit << setw(11) << chi2 
                << setw(11) << dof << setw(11) << chi2/dof << endl;
    }//for(int is = 0;
    
    // Display spectra
    TCanvas *c1=new TCanvas("c1","Results-1");
    
    c1->Clear();
    //c1->Divide(1,2);
    
    c1->cd(1);
    hTbeta[0]->Draw();
    hTbeta[2]->SetLineColor(2);
    hTbeta[2]->Draw("same");
    hTbeta[1]->SetLineColor(3);
    hTbeta[1]->Draw("same");
    
    /***
        c1->cd(2);
        hTbeta_diff[0]->Draw();
        hTbeta_diff[1]->SetLineColor(3);
        hTbeta_diff[1]->Draw("same");
        hTbeta_diff[2]->SetLineColor(2);
        hTbeta_diff[2]->Draw("same");
    ***/
    
    c1->Update();
    
    cout  <<  "*** THE END ***" << endl;
    return(0);
}//syst_C1(void)

//*******************************************************************************************//
// Fit function
Double_t Fit_E(Double_t *x, Double_t *par)
{
    Int_t z;
    Double_t c_bsim;
    z=hTbeta[1]->GetXaxis()->FindFixBin(x[0]);
    c_bsim=hTbeta[1]->GetBinContent(z);
    
    // Convert channel number to total energy
    Double_t Etot=x[0]/Acal + me;
    return( par[0]*c_bsim*(1. + par[1]*Etot) );
}
//*******************************************************************************************//

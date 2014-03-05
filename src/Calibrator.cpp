/** \file Calibrator.cpp
 *  \brief A class to calculate the calibrations of the detectors
 *  \author S. V. Paulauskas
 *  \date 18 February 2014
 */
#include "Calibrator.hpp"

double Calibrator::GetCsICal(const double &raw) {
    //Gain +850 V; energy contraction 1

    //Only use the 60co lines - reproduces the lines exactly (as it should)
    // double m = 0.565488;
    // double b = -23.1406;

    //use 60co and 132cs lines
    double m = 0.558823;
    double b = -8.06648;

    return(m*raw+b);
    //Gain +900 V - calibrated from bkg lines
    // double m = 3.19243;
    // double b = -21.8969;
    // double c = 0.00;

    //Gain +860 V
    // double m = 0.532437;
    // double b = 32.287;
    // double c = -1.32692e-7;
    // return(c*raw*raw+m*raw+b);
}


double Calibrator::GetGeCal(const double &raw) {
    //Energy contraction of 1
    double m = 0.0426626;
    double b = 1.4821;
    return(m*raw+b);
}

/** \file Calibrator.cpp
 *  \brief A class to calculate the calibrations of the detectors
 *  \author S. V. Paulauskas
 *  \date 18 February 2014
 */
#include "Calibrator.hpp"

double Calibrator::GetCsICal(const double &raw) {
    //Gain +850 V; energy contraction 1
    double m = 0.559311;
    double b = -9.4343;
    return(m*raw+b);
    //Gain +900 V - calibrated from bkg lines
    // double m = 3.19243;
    // double b = -21.8969;
    // double c = 0.00;

    // double m = 3.16165;
    // double b = -23.1168;
    // double c = 0.000127226;
    // return(c*raw*raw+m*raw+b);
}


double Calibrator::GetGeCal(const double &raw) {
    //Energy contraction of 1
    double m = 0.0426626;
    double b = 1.4821;
    return(m*raw+b);
}

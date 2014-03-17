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
}


double Calibrator::GetGeCal(const double &raw) {
    //Energy contraction of 1
    double m = 0.0426626;
    double b = 1.4821;
    return(m*raw+b);
}

/** \file Calibrator.cpp
 *  \brief A class to calculate the calibrations of the detectors
 *  \author S. V. Paulauskas
 *  \date 18 February 2014
 */
#include "Calibrator.hpp"

double Calibrator::GetCsICal(const double &raw) {
    double m = 2.23724;
    double b = -9.4343;
    return(m*raw+b);
}


double Calibrator::GetGeCal(const double &raw) {
    double m = 0.17065;
    double b = 1.4821;
    return(m*raw+b);
}

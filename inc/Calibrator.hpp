/** \file Calibrator.hpp
 *  \brief A class to calculate the calibrations of the detectors
 *  \author S. V. Paulauskas
 *  \date 18 February 2014
 */
#ifndef __CALIBRATOR_HPP__
#define __CALIBRATOR_HPP__

class Calibrator {
public:
    Calibrator(){};
    ~Calibrator(){};

    double GetGeCal(const double &raw);
    double GetCsICal(const double &raw);
};
#endif //__CALIBRATOR_HPP__

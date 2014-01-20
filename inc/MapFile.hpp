/** \file MapFile.hpp
 * \brief An independent way to read in the map file with an updated format
 *   from the InitMap() days
 *
 * This file is part of the pixie_scan program developed by 
 * the Experimental Nuclear Physics Group at the Unviersity of 
 * Tennessee at Knoxville. 
 * https://github.com/pixie16/pixie_scan
 *
 * I (SVP) have modified this code to work with analysis software
 * used in conjunction with the NSCL DAQ.
 *
 */
#ifndef __MAPFILE_HPP_
#define __MAPFILE_HPP_

#include <set>
#include <string>
#include <vector>
#include <map>

#include "DetectorLibrary.hpp"

/** 
 * A nice simple class which reads in the map file when declared
 *   and updates the external modchan vector accordingly
 */
class MapFile {
public:
    MapFile(DetectorLibrary &detLib,
            const std::string &filename = "map.txt");
    static const std::string defaultFile; ///< default map file
    
    operator bool() {return isRead;} ///< is the map file valid

private:
    bool HasWildcard(const std::string &str) const;
    bool isRead;

    void ProcessTokenList(const std::vector<std::string> &list, 
                          DetectorLibrary &detLib);
    void TokenizeString(const std::string &in, 
                        std::vector<std::string> &out) const;
    void TokenToVector(std::string token, std::vector<int> &list, 
                       int number) const;
    
};
#endif // __MAPFILE_HPP_

/*----------------------------------------------------------------------
 * Copyright (c) 2014, University of Tennessee Board of Trustees
 * All rights reserved.
 *
 * This code was developed as part of the research efforts of the 
 * Experimental Nuclear Physics Group for use with the Pixie-16
 * electronics. The work was supported by the following grants:
 *
 * DE-FG52-08NA28552
 *
 * The work is licensed under the GNU GPLv3
 *
 * Redistribution and use in source and binary forms, 
 * with or without modification, are permitted provided 
 * that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above 
 *     copyright notice, this list of conditions and the 
 *     following disclaimer.
 *   * Redistributions in binary form must reproduce the 
 *     above copyright notice, this list of conditions and the 
 *     following disclaimer in the documentation and/or other 
 *     materials provided with the distribution.
 *   * Neither the name of University of Tennessee nor the names of its
 *     contributors may be used to endorse or promote
 *     products derived from this software without 
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR 
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE.
 *----------------------------------------------------------------------*/

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

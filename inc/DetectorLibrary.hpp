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

/** \file DetectorLibrary.hpp
 * \brief An extension of the vector of the identifiers previously used.
 */
#ifndef __DETECTORLIBRARY_HPP_
#define __DETECTORLIBRARY_HPP_

#include <map>
#include <set>
#include <string>
#include <vector>

#include "Identifier.hpp"

class DetectorLibrary {
public:
    DetectorLibrary(){};
    DetectorLibrary(const int &size);
    ~DetectorLibrary() {};

    bool HasValue(int mod, int chan) const;
    bool HasValue(int index) const;

    Identifier at(int mod, int ch) const;
    Identifier at(int idx) const;
    
    int GetNextLocation(const Identifier &id) const;
    int GetNextLocation(const std::string &type, 
			const std::string &subtype) const;
    int GetIndex(int mod, int chan) const;
    int ModuleFromIndex(int index) const;
    int ChannelFromIndex(int index) const;

    const std::set<int> &GetLocations(const Identifier &id) const;
    const std::set<int> &GetLocations(const std::string &type, 
                                      const std::string &subtype) const;
    const std::set<int> &GetUsedIds(void) const;
    const std::set<std::string>& GetKnownDetectors(void); 
    const std::set<std::string>& GetUsedDetectors(void) const;


    unsigned int GetPhysicalModules() const {return numPhysicalModules;}
    unsigned int GetModules() const {return numModules;}

    void PrintMap(void) const;
    //void PrintUsedDetectors(RawEvent& rawev) const;
    void push_back(const Identifier &x);
    void Set(int index, const Identifier& value);
    void Set(int mod, int ch, const Identifier& value);

    typedef std::string mapkey_t;

private:
    mapkey_t MakeKey( const std::string &type, 
                      const std::string &subtype ) const;

    ///< collection of all used locations for a given type and subtype
    std::map< mapkey_t, std::set<int> > locations; 
    
    ///< dummy locations to return when map key does not exist
    static std::set<int> emptyLocations; 

    unsigned int numModules;
    unsigned int numPhysicalModules;
    
    std::set<int> usedIds_;
    std::set<std::string> knownDetectors;
    std::set<std::string> usedTypes;
    std::set<std::string> usedSubtypes;

    std::vector<Identifier> identifiers_;
};

#endif // __DETECTORLIBRARY_HPP_

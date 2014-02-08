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

/** \file DetectorLibrary.cpp
 *  \brief Some useful function for managing the list of channel identifiers
 *  \author David Miller
 */
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>

#include "DetectorLibrary.hpp"

using namespace std;

set<int> DetectorLibrary::emptyLocations;

DetectorLibrary::DetectorLibrary(const int &size) {
    identifiers_.resize(size);
    GetKnownDetectors();
}

Identifier DetectorLibrary::at(int idx) const
{
    return(identifiers_.at(idx));
}

Identifier DetectorLibrary::at(int mod, int ch) const
{
    return (identifiers_.at(GetIndex(mod,ch)));
}

void DetectorLibrary::push_back(const Identifier &x)
{
    mapkey_t key = MakeKey(x.GetType(), x.GetSubtype());

    locations[key].insert(x.GetLocation());
    identifiers_.push_back(x);
}

/**
 * return the list of locations for a particular identifier
 */
const set<int>& DetectorLibrary::GetLocations(const Identifier &id) const
{
    return GetLocations(id.GetType(), id.GetSubtype());
}

/**
 * return the list of locations for a particular type and subtype
 */
const set<int>& DetectorLibrary::GetLocations(const string &type, const string &subtype) const
{
    mapkey_t key = MakeKey(type, subtype);

    if (locations.count(key) > 0) {
	return locations.find(key)->second; 
    } else {
	return emptyLocations;
    }
}

/**
 * return the next undefined location for a particular identifer
 */
int DetectorLibrary::GetNextLocation(const Identifier &id) const
{
  return GetNextLocation(id.GetType(), id.GetSubtype());
}

/**
 * return the next undefined location for a particular type and subtype
 */
int DetectorLibrary::GetNextLocation(const string &type, 
				     const string &subtype) const
{
    mapkey_t key = MakeKey(type, subtype);

    if (locations.count(key) > 0) {
	return *(locations.find(key)->second.rbegin()) + 1;
    } else {
	return 0;
    }
}

int DetectorLibrary::GetIndex(int mod, int chan) const
{
    return (mod * 16 + chan);
}

bool DetectorLibrary::HasValue(int mod, int chan) const
{
    return HasValue(GetIndex(mod,chan));
}

bool DetectorLibrary::HasValue(int index) const
{
    return ((int)identifiers_.size() > index && 
          identifiers_.at(index).GetType() != "");
}

void DetectorLibrary::Set(int index, const Identifier& value)
{
    /// Search the list of known detectors; if the detector type 
    ///    is not matched, print out an error message and terminate
    if (knownDetectors.find(value.GetType()) == knownDetectors.end()) {
	cout << endl;
	cout << "The detector called '" << value.GetType() <<"'"<< endl
	     << "read in from the file 'map2.txt'" << endl
	     << "is unknown to this program!.  This is a" << endl
	     << "fatal error.  Program execution halted!" << endl
	     << "If you believe this detector should exist," << endl
	     << "please edit the 'getKnownDetectors'" << endl
	     << "function inside the 'DetectorLibrary.cpp' file" << endl
	     << endl;
	cout << "The currently known detectors include:" << endl;
	copy(knownDetectors.begin(), knownDetectors.end(), ostream_iterator<string>(cout, " "));
	cout << endl;
	exit(EXIT_FAILURE);
    }

    unsigned int module  = ModuleFromIndex(index);
    if (module >= numModules ) {
	numModules = module + 1;
	identifiers_.resize(numModules * 16);
	if (!value.HasTag("virtual")) {
	    numPhysicalModules = module + 1;
	}
    }
    
    string key;
    key = value.GetType() + ':' + value.GetSubtype();
    locations[key].insert(value.GetLocation());
    
    usedTypes.insert(value.GetType());
    usedSubtypes.insert(value.GetSubtype());

    usedIds_.insert(index);
    identifiers_.at(index) = value;
}

void DetectorLibrary::Set(int mod, int ch, const Identifier &value)
{
    Set(GetIndex(mod,ch), value);
}

/**
 *  Dump the map
 */
void DetectorLibrary::PrintMap(void) const
{
    cout << setw(4) << "MOD" 
	 << setw(4) << "CH";
    Identifier::PrintHeaders();

    for (size_t i=0; i < identifiers_.size(); i++) {
	cout << setw(4) << ModuleFromIndex(i)
	     << setw(4) << ChannelFromIndex(i);
	identifiers_.at(i).Print();
    }
}

/**
 * Print the list of detectors used and initialize the global raw event
 */
//void DetectorLibrary::PrintUsedDetectors(RawEvent& rawev) const
//{
    // Print the number of detectors and detector subtypes used in the analysis
    // cout << usedTypes.size() <<" detector types are used in this analysis " 
    //      << "and are named:" << endl << "  "; 
    // copy(usedTypes.begin(), usedTypes.end(), ostream_iterator<string>(cout, " "));
    // cout << endl;
    
    // cout << usedSubtypes.size() <<" detector subtypes are used in this "
    //      << "analysis and are named:" << endl << "  ";
    // copy(usedSubtypes.begin(), usedSubtypes.end(), ostream_iterator<string>(cout," "));
    // cout << endl;   

    // rawev.Init(usedTypes);
//}

/*!
  Retrieves a vector containing all detector types for which an analysis
  routine has been defined making it possible to declare this detector type
  in the map.txt file.  The currently known detector types are in detectorStrings
*/
const set<string>& DetectorLibrary::GetKnownDetectors(void)
{
    knownDetectors = {"dssd_front", "dssd_back", "idssd_front", 
                      "position", "timeclass", "ge", "si", 
                      "scint", "mcp", "mtc", "generic", "ssd", 
                      "vandleSmall", "vandleBig", "tvandle", 
                      "pulser", "logic", "ion_chamber", 
                      "ignore", "valid", "csi", "nai", "tdc"};
  
    // this is a list of the detectors that are known to this program.
    cout << "DetectorLibrary: constructing the list of known detectors "
         << endl;

    return knownDetectors;
}

/**
 * Retrieves the detector types used in the current analysis
 */
const set<string>& DetectorLibrary::GetUsedDetectors(void) const
{
    return(usedTypes);
}

const set<int>& DetectorLibrary::GetUsedIds(void) const {
    return(usedIds_);
}

/**
 * Convert an index number into which module the detector resides in
 */
int DetectorLibrary::ModuleFromIndex(int index) const
{
    return int(index / 16);
}

/**
 * Convert an index number into which channel the detector resides in
 */
int DetectorLibrary::ChannelFromIndex(int index) const
{
    return (index % 16);
}

/**
 * Make a unique map key for a give detector type and subtype
 */
DetectorLibrary::mapkey_t DetectorLibrary::MakeKey(const string &type, const string &subtype) const
{
    return (type + ':' + subtype);
}

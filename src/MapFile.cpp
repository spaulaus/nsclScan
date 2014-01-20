/** \file MapFile.cpp
 * \brief Implementation to read in a new format map file
 * \author David Miller
 * \date 02 April 2012
 * This file is part of the pixie_scan program developed by 
 * the Experimental Nuclear Physics Group at the Unviersity of 
 * Tennessee at Knoxville. 
 * https://github.com/pixie16/pixie_scan
 *
 * I (SVP) have modified this code to work with analysis software
 * used in conjunction with the NSCL DAQ.
 *
 * \modified S. V. Paulauskas
 * \date 20 January 2014
 */

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

#include "Identifier.hpp"
#include "MapFile.hpp"

using namespace std;

/**
 * Contains the description of each channel in the analysis.  The
 * description is read in from map.txt and includes the detector type and 
 * subtype, the damm spectrum number, and physical location
 */

/**
 * Read in a map file 
 * This occurs in the constructor so to preempt scanor fortran routines;
 *   this allows us to define the operation of the program dependent
 *   on what detector types show up in the map
 */
MapFile::MapFile(DetectorLibrary &detLib, const string &filename) { 
    string mapFileName = filename;
    cout << "MapFile: loading map file " << mapFileName << endl;

    ifstream in(mapFileName.c_str());
     
    if (!in.good()) {
        cerr << "Could not open map file " << mapFileName << endl;
    }
    
    const size_t maxConfigLineLength = 100;
    char line[maxConfigLineLength];
     
    vector<string> tokenList;
    vector< vector<string> > normalLines;
    vector< vector<string> > wildcardLines;

    while (!in.eof()) {
	tokenList.clear();
	in.getline(line, maxConfigLineLength);
	if (in.fail())
	    break;
	
	// Allowing for comments for any line not starting with a number or a star
	if (!isdigit(line[0]) && line[0] != '*')
	    continue;

	TokenizeString(string(line), tokenList);
	if (tokenList.size() < 3) {
	    cerr << "Too few tokens in map file line: " << endl;
	    cerr << line << endl;
	    continue;
	}
	// separate lines based on whether or not they contain wildcards
	//   wildcards are * or ranges (i.e. 1-4) or even/odd for channels given by 'e' / 'o'
	if ( HasWildcard(tokenList.at(0)) || HasWildcard(tokenList.at(1)) ) {
	    wildcardLines.push_back(tokenList);
	} else {
	    normalLines.push_back(tokenList);
	}
    }

    // now we parse each tokenized line, processing wildcard lines afterwards
    for (vector< vector<string> >::iterator it = normalLines.begin();
	 it != normalLines.end(); it++) {
        ProcessTokenList(*it, detLib);
    }
    for (vector< vector<string> >::iterator it = wildcardLines.begin();
	 it != wildcardLines.end(); it++) {
        ProcessTokenList(*it, detLib);
    }

    isRead = true;
}

/**
 *  Tokenize a line from the map file, breaking into words separated by whitespace
 */
void MapFile::TokenizeString(const string &in, vector<string> &out) const
{
    string delimiters(" \t");

    // skip delimiters at beginning.
    string::size_type lastPos = in.find_first_not_of(delimiters, 0);
    
    // find first "non-delimiter".
    string::size_type pos = in.find_first_of(delimiters, lastPos);
    
    while (pos != string::npos || lastPos != string::npos) {
	// found a token, add it to the vector
	out.push_back(in.substr(lastPos, pos - lastPos));
	
	// skip delimiters.  Note the "not_of"
	lastPos = in.find_first_not_of(delimiters, pos);
	
	// find next "non-delimiter"
	pos = in.find_first_of(delimiters, lastPos);
    }
}

/**
 * Process a tokenized line of the mapfile and fill the global identifier array
 */
void MapFile::ProcessTokenList(const vector<string> &tokenList,
                               DetectorLibrary &detLib)
{
    Identifier id;
    
    vector<int> moduleList;
    vector<int> channelList;

    vector<string>::const_iterator tokenIt = tokenList.begin();

    // first token corresponds to module list
    TokenToVector(*tokenIt++, moduleList, detLib.GetModules() );
    // second token corresponds to channel list
    TokenToVector(*tokenIt++, channelList, 16);
    // third token corresponds to detector type

    string type(*tokenIt++);
    string subtype;

    // fourth token can provide the subytpe
    //   if token is a whole number, assume it as a location and skip
    if (tokenIt != tokenList.end() && 
        tokenIt->find_first_not_of("1234567890") != string::npos) {
        // synonym for "Same as type"
        if ( *tokenIt == "--" ) {
            subtype = type;
        } else { 
            subtype = *tokenIt;
        }
	tokenIt++;
    } else {
        subtype = type;
    }

    id.SetType(type);
    id.SetSubtype(subtype);

    // fifth token gives the starting location number
    //   otherwise we just find the smallest location in the map for the type/subtype
    int startingLocation;
    if (tokenIt != tokenList.end() &&
	tokenIt->find_first_not_of("1234567890") == string::npos ) {
        stringstream(*tokenIt) >> startingLocation;
        tokenIt++;
    } else {
        startingLocation = detLib.GetNextLocation(type, subtype);
    }

    // process additional identifiers as key=integer or toggling boolean flag to true (1)
    for (;tokenIt != tokenList.end(); tokenIt++) {
	string::size_type equalPos = tokenIt->find_first_of('=');

	if (equalPos == string::npos) {
	    id.AddTag(*tokenIt, Identifier::TagValue(1) );
	} else {
	    string key   = tokenIt->substr(0,equalPos);
	    Identifier::TagValue value;

	    stringstream(tokenIt->substr(equalPos+1)) >> value;
	    id.AddTag(tokenIt->substr(0,equalPos), value);
	}       
    }

    for (vector<int>::iterator modIt = moduleList.begin();
	 modIt != moduleList.end(); modIt++) {
        for (vector<int>::iterator chanIt = channelList.begin();
             chanIt != channelList.end(); chanIt++) {
            // check if this channel has already been defined
            if ( detLib.HasValue(*modIt, *chanIt) ) {
                // if this is a wildcard line, just continue
                if (HasWildcard(tokenList.at(0)) ||
                    HasWildcard(tokenList.at(1))) {
                    continue;
                }
                cerr << "Identifier for " << type << " in module " << *modIt
                     << " : channel " << *chanIt 
                     << " is initialized more than once in the map file"
                     << endl;
                
                exit(EXIT_FAILURE);
            }
            
            id.SetLocation(startingLocation);
            detLib.Set(*modIt, *chanIt, id);

            startingLocation++;
        }
    }
}

/**
 * Test if a string has a wildcard 
 */
bool MapFile::HasWildcard(const string &str) const
{
    return (str.find_first_of("*-eo",0) != string::npos);
}

/**
 * Convert a token into a string 
 */
void MapFile::TokenToVector(string token, vector<int> &list, int number) const
{
    if (!HasWildcard(token)) {
	// this is a single number
	int num;

	stringstream(token) >> num;
	list.push_back(num);
	return;
    }

    // first check for even / odd
    bool hasEven = false;
    bool hasOdd  = false;
    string::size_type pos;

    pos = token.find_first_of('e');
    if (pos != string::npos) {
	hasEven = true;
	token.erase(pos, 1);
    }

    pos = token.find_first_of('o');
    if (pos != string::npos) {
	hasOdd = true;
	token.erase(pos, 1);
    }

    // this also properly handles the "*" wildcard to do all modules/channels
    int low = 0;
    int high = number-1;
    
    // next look for a range indicator "-"
    pos = token.find_first_of('-');
    if (pos != string::npos) {
	string leftToken(token, 0, pos);
	string rightToken(token, pos+1);
	
	stringstream(leftToken) >> low;
	stringstream(rightToken) >> high;
	
	// extend the range if the number is not high enough
	number = max(high+1, number);
    }

    // finally parse the complete list and fill the list accordingly
    for (int i=low; i <= high; i++) {
	if ( hasOdd && ( i % 2 ) == 0 )
	    continue;
	if ( hasEven && ( i % 2 ) != 0 ) 
	    continue;
	list.push_back(i);
    }
}



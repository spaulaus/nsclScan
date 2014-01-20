/** \file Identifier.cpp
 * \brief Handles Indentification of a Channel
 * \author Sean N. Liddick
 * \date 02 July 2007
 *
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

#include <iomanip>
#include <iostream>

#include "Identifier.hpp"

using namespace std;

/** 
 * Return the value of a tag 
 */
Identifier::TagValue Identifier::GetTag(const string &s) const
{
    map<string, TagValue>::const_iterator it = tag.find(s);
    
    if (it == tag.end()) {
        return TagValue();
    }
    return it->second;
}

/**
 * Identifier constructor
 *
 * The dammid and detector location variable are set to -1
 * and the detector type and sub type are both set to ""
 * when an identifier object is created.
 */
Identifier::Identifier(){
    Zero();
}

/**
 * Identifier zero
 *
 * The dammid and detector location variable are reset to -1
 * and the detector type and sub type are both reset to ""
 * when an identifier object is zeroed.
 */
void Identifier::Zero()
{
    location = -1;
    type     = "";
    subtype  = "";

    tag.clear();
}

/**
 * Print column headings to aid with print 
 */
void Identifier::PrintHeaders(void) 
{
    cout << setw(10) << "Type"
	 << setw(10) << "Subtype"
	 << setw(4)  << "Loc"
	 << "    TAGS" << endl;	 
}

/**
 * Print the info containing in the identifier with trailing newline
 */
void Identifier::Print(void) const
{
    cout << setw(10) << type << " "
	 << setw(10) << subtype << " " 
	 << setw(4)  << location << " " 
	 << "    ";
    for (map<string, TagValue>::const_iterator it = tag.begin();
	 it != tag.end(); it++) {
	if (it != tag.begin())
	    cout << ", ";
	cout << it->first << "=" << it->second;
    }
    cout << endl;
}


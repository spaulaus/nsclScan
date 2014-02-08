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


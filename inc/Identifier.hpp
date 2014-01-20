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
#ifndef __IDENTIFIER_HPP_
#define __IDENTIFIER_HPP_

#include <sstream>
#include <string>
#include <map>

/**
 * \brief Channel identification

 * All parameters needed to uniquely specify the detector connected to a
 * specific channel are set here.  A vector of identifiers for all channels is
 * created on initialization in InitID().  Each event that is created in
 * ScanList() in PixieStd.cpp has its channel identifier attached to it in
 * the variable chanID inside RawEvent
 *
 * Identifier is a class that will contain basic channel information that
 * will not change including the damm spectrum number where the raw energies
 * will be plotted, the detector type and subtype, and the detector's physical
 * location (strip number, detector location, ...)
 */
class Identifier {
public:
    Identifier();
    ~Identifier(){};

    typedef int TagValue;

    void SetType(const std::string &a)    {type = a;}     /**< Set the detector type */
    void SetSubtype(const std::string &a) {subtype = a;}  /**< Set the detector subtype */
    void SetLocation(int a)   {location = a;} /**< Set the detector location */
    
    const std::string& GetType() const    {return type;}     /**< Get the detector type */
    const std::string& GetSubtype() const {return subtype;}  /**< Get the detector subtype */
    int GetLocation() const               {return location;} /**< Get the detector location */
    
    void AddTag(const std::string &s, TagValue n) {tag[s] = n;} /**< Insert a tag */
    bool HasTag(const std::string &s) const {return (tag.count(s) > 0);} /**< True if the tag s has been inserted */
    TagValue GetTag(const std::string &s) const; 

    void Zero();
    static void PrintHeaders(void);
    void Print(void) const;
    
    bool operator==(const Identifier &x) const {
	return (type == x.type &&
		subtype == x.subtype &&
		location == x.location);
    } /**< Compare this identifier with another */
    bool operator!=(const Identifier &x) const {
	return !operator==(x);
    }

private:
    std::string type;      /**< Specifies the detector type */
    std::string subtype;   /**< Specifies the detector sub type */
    int location;          /**< Specifies the real world location of the channel.
  			        For the DSSD this variable is the strip number */
    std::map<std::string, TagValue> tag;  /**< A list of tags associated with the identifer */ 
};
#endif //__IDENTIFIER_HPP_

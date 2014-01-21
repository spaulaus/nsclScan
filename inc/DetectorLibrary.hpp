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

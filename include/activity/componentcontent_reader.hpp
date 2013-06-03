/**
 * @file    compononentactivity_reader.hpp
 *
 * @description File reader for object serialization and read of a file using the boost-dev libraries in C++. The reader should serialize an activity from txt using a header for its structure.
 * @standard 	Preferred standard is C++11
 *
 * @author Marc Wiedemann
 * @date   2013
 *
 */

 /*!
 Software Structure
 1 Requirements
 Our goal is to put the activity module to action by writing and in this header reading from string file.
 We use the string file to supervise if errors occur and to have a flexible format for the transaction system and knowledge base, later a bytestream is an option when everything is positively tested.
 As input we need the activity format and transport
 In this first version the activity is hardcoded as a component with for the SIOX system relevant information pieces: name, HWid, SWid,Iid.
 To get the knowledge of the total information load of the activity we examine the activity.hpp in ../Implementation/include/monitoring/datatypes/. That format has to be adapted to our needs.
 These headers and the ones in ../Implementation/include/activity interact.

 2 UseCases
 Datatypes "activities" coming from the ActivityMultiplexer being written with componentcontent_writer to a text file can be read by this library module
 for use within modules of the knowledgepath as optimization basis.

 3 Design und Text
 We use the file serializiation as a functioning prototype with readable files resembling the transaction system of the siox system design.

 4 Testimplementation
  This is the first implementation of the SIOX component to string file transformation.
  Testing will be possible when the Implementation in cpp is finished.
  1 Testcase : Reading two activities from string file and sending them with SIOX communication to the knowledge module. Which one?
 */


#ifndef __componentcontent_reader
#define __componentcontent_reader

#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>
// datatype for activity
#include <activity.hpp>
// Byte stream
#include <iostream>
// String stream
#include <sstream>
#include <string>
#include <tuple>

// necessary for activity's namespace
using namespace monitoring;


std::stringstream ssm;
std::tuple<std::string, double, int, int, std::string> tple;

class information_type
{
public:
  component() {}
  component(const std::string &name, const std::string &HWid, const std::string &SWid, const std::string &Iid) : name_ (name), HWid_ (HWid), SWid_ (SWid), Iid_ (Iid) {}
  std::string name() const {return name_; }
  std::string HWid() const {return HWid_; }
  std::string SWid() const {return SWid_; }
  std::string Iid() const {return Iid_; }


private:
  friend class boost::serializiation::access;

  template  <typename Archive>
  friend void serialize(Archive &ar, component &cp, const unsigned int version)

  std::string name_;
  std::string HWid_;
  std::string SWid_;
  std::string Iid_;
};

template <typename Archive>
void serialize(Archive &ar, component &cp, const unsigned int version
){
  ar & cp.name;
  ar & cp.HWid;
  ar & cp.SWid;
  ar & cp.Iid;

}



void componentcontent_reader()
{
  boost::archive::text_iarchive ia(ssm);
  component *cp;
  ia >> cp;
	std::cout << cp->name() << std::endl;
	delete cp;
}


/*int main()
{
  componentcontent_reader()
}
*/

#endif /* __componentcontent_reader */
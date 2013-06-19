/**
 * @file    componentcontent_writer.hpp
 *
 * @description File writer for object serialization and write to file using the boost-dev libraries in C++
 * @standard 	Preferred standard is C++11
 *
 * @author Marc Wiedemann
 * @date   2013
 *
 */

/*!
 Software Structure
 
 1 Requirements
 Our goal is to put the activity module to action by writing and in this header writing to a string file.
 We use the string file to supervise if errors occur and to have a flexible format for the transaction system and knowledge base, later a bytestream is an option when everything is positively tested.
 As input we need the activity format and transport.
 In this first version the activity is hardcoded as a component with for the SIOX system relevant information pieces: name, HWid, SWid,Iid.
 To get the knowledge of the total information load of the activity we examine the activity.hpp in ../Implementation/include/monitoring/datatypes/. That format has to be adapted to our needs.
 These headers and the ones in ../Implementation/include/activity interact.
 
 Resilient:
 Second step: Write two iostreams one to fast Ram based storage (nvram or ram based database) and one to archive quality Transaction system database
 
 2 UseCases
 Datatypes "activities" coming from the ActivityMultiplexer being written with componentcontent_writer to a text file can be read by this library module
 for use within modules of the knowledgepath as optimization basis.
 
 3 Design und Text
 We use the file serializiation as a functioning prototype with readable files resembling the transaction system of the siox system design.
 
 4 Testimplementation
  This is the first implementation of the SIOX component to string file transformation.
  Testing will be possible when the Implementation in cpp is finished.
  1 Testcase : Writing two activities to one or two? string file(s) and sending them with SIOX communication to the knowledge module. Which one?
 
 */



#ifndef __componentcontent_writer
#define __componentcontent_writer

#include <__componentcontent_writer>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/string.hpp>
// datatype for activity
#include "Activity.h"
// Byte stream
#include <iostream>
// String stream
#include <sstream>
#include <string>
#include <tuple>

std::stringstream ssm;
std::tuple<std::string, double, int, int, std::string> tple;

class information_type
{
public:
  component() {};
  component(const std::string &name, const std::string &HWid, const std::string &SWid, const std::string &Iid) : name_ (name), HWid_ (HWid), SWid_ (SWid), Iid_ (Iid) {};
  std::string name() const {return name_; };
  std::string HWid() const {return HWid_; };
  std::string SWid() const {return SWid_; };
  std::string Iid() const {return Iid_; };


private:
  friend class boost::serializiation::access;

  template  <typename Archive>;
  friend void serialize(Archive &ar, component &cp, const unsigned int version);

  std::string name_;
  std::string HWid_;
  std::string SWid_;
  std::string Iid_;
};

template <typename Archive>;
void serialize(Archive &ar, component &cp, const unsigned int version;
){
};


class ComponentActivityWriter
{

public:

  /**
   * Register listener in notify list
   *
   * @param pointer ComponentActivityWriter_Listener
   * @return  Status  int
   */
  virtual void register_listener(ComponentActivityWriter_Listener * listener, bool async);

  /**
   * Unregister listener from notify list
   *
   * @param pointer ComponentActivityWriter_Listener
   * @return  Status  int
   */
  virtual void unregister_listener(ComponentActivityWriter_Listener * listener);

};


void componentcontent_writer()
{
};



#endif /* __componentcontent_writer */
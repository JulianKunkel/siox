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

#ifndef __componentcontent_writer
#define __componentcontent_writer

#include <__componentcontent_writer>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/string.hpp>
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



void componentcontent_writer()
{
};



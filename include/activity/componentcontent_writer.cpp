/**
 * @file    file_Writer.c
 *
 * @description File writer for object serialization and write to file using the boost-dev libraries in C++
 * @standard 	Preferred standard is C++11
 *
 * @author Marc Wiedemann
 * @date   2013
 *
 */


#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/string.hpp>
// Byte stream
#include <iostream>
// String stream
#include <sstream>
#include <string>

std::stringstream ssm;

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
void serialize(Archive &ar, component &cp, const unsigned int version)
{
  ar & cp.name;
  ar & cp.HWid;
  ar & cp.SWid;
  ar & cp.Iid;

}



void componentcontent_writer()
{
  boost::archive::text_oarchive oa(ssm);
  component cp("Write_collective", "cnode 2-4","MPI-2","All");
  oa << cp;
  boost::archive::text_oarchive oa2(ssm);
  component cp2("Open","cnode 18","POSIX","Address")
  oa2 << cp2;
}


int main()
{
	componentcontent_writer()
}
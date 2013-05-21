/**
 * @file    activity.hpp
 *
 * @description This activity is the structure to be read and written to txt using this header.
 * @standard    Preferred standard is C++11
 *
 * @author Michaela Zimmer, Marc Wiedemann
 * @date   2013
 *
 */

/*!
 This is a container class using the low-level activity that has ended and is complete

 We have four getter methods in the following areas:
 1 ID's (Aid, Cid (Componentid), Name, SWid, HWid, Iid)
 2 Attributes (Metrics)
 3 Remote Calls (from other nodes)
 4 Parents (Paid) with special case "Remote ?"

 */

#infdef __SIOX_ACTIVITY_H
#define __SIOX_ACTIVITY_H

/*!
 We define classes in the namespace monitoring. Is it accessible from other namespaces?
 The activity should be public to have other multiplexer, componentcontent_reader and componentcontent_writer access possibilities.
 */


namespace monitoring {

class get_Activity
{
public:
  
  const Activity& operate_info() const // const getter
  {
    return m_info;
  }

  Activity& operate_info() /*! variable getter with operate = get = set */
  {
    return m_info;
  }
  ~get_Activity();
  
private:

  Activity m_info;
};

/*!
 State of activity
 For the state of the activity we use the enum instruction to combine the definitions of constants and variables 
 For example usage in the cpp: ActivityStatus actstat=initialized;
 */

enum ActivityStatus {
    empty,
    initialized,
    started,
    stopped,
    finished
};


class Activity
{
// or public?
private:
    long aid;
    long paid;
    long unid;
    long time_start;
    long time_stop;
    int status;
    char[] comment;

public:
  Activity(void);
  Activity(Component component, Timestamp t_start = NULL, char[] comment="");
  ~Activity(void);

  Component() {};
  Component(const std::string &name, const std::string &HWid, const std::string &SWid, const std::string &Iid) : name_ (name), HWid_ (HWid), SWid_ (SWid), Iid_ (Iid) {};
  std::string name() const {return name_; };
  std::string HWid() const {return HWid_; };
  std::string SWid() const {return SWid_; };
  std::string Iid() const {return Iid_; };
  ~Component(void);

  SWid() {};
  SWid(const std::string layer_name, array<unsigned char,4> version)
  ~SWid(void);


    //List<Attribute> attributes;

/*!
 the activity consists of component name and ids, start and stop and end after reporting
 */

/*!
 Five Steps for code development in general
 Think of and write Use Cases
 Observe and write Requirements
 Declare functions their behavior and relations between them
 Write documentation for the developer and user
 See what's possible and implement it.
 */

/*!
 Usecase 1 : Individual POSIX file write of multifile to a storage target SAS controller which has a LUN
  In this case Component.HWid := HWid(LUN) = some kind of controller bus id = pci@0000:00:1f.2.
  The controller has a Component.name which is a string "SAS Controller Model".
  The Component.SWid is the software layer composition "POSIX-Kernel-PFS-Block" or any of those individually.
 */

/*!
 Usecase 2 : MPI Write using etypes - going through ROMIO or OMPIO the through ADIO layer to kernel and to generic_Write
 How is in this case the component.HWid discovered? Component.SWid would be the string "MPI-OMPIO-ADIO" or any of those individually.
 */

}


#endif // __SIOX_ACTIVITY_H
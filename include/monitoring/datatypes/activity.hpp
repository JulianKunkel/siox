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

#infdef __SIOX_ACTIVITY_H
#define __SIOX_ACTIVITY_H

/*!
 We define classes in the namespace monitoring. Is it accessible from other namespaces?
 The activity should be public to have other multiplexer, componentcontent_reader and componentcontent_writer access possibilities.
 */


namespace monitoring {
    

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

    //List<Attribute> attributes;

/*!
 the activity consists of component name and ids, start and stop and end after reporting
 */

public:
  Activity(void);
  Activity(Component component, Timestamp t_start = NULL, char[] comment="");
  ~Activity(void);

  void start(Timestamp t=NULL);
  void stop(Timestamp t=NULL);
  void end(void);

  Component() {};
  Component(const std::string &name, const std::string &HWid, const std::string &SWid, const std::string &Iid) : name_ (name), HWid_ (HWid), SWid_ (SWid), Iid_ (Iid) {};
  std::string name() const {return name_; };
  std::string HWid() const {return HWid_; };
  std::string SWid() const {return SWid_; };
  std::string Iid() const {return Iid_; };

}
#endif // __SIOX_ACTIVITY_H
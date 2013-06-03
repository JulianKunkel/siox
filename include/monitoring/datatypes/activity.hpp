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
 Five Steps for code development in general
 Think of and write Use Cases
 Observe and write Requirements
 Declare functions their behavior and relations between them
 Write documentation for the developer and user
 See what's possible and implement it.
 */

/**
 * USE CASES
 * =========
 * 1 - Containerklasse, die eine zu einer beliebigen beendeten Aktivität gehörigen
 * vom Low-Level-Interface aufgezeichneten Eigenschaften kapselt.
 * NICHT das Arbeitsobjekt innerhalb der HW-Node, sondern das Objekt, das im
 * Monitoringsystem archiviert wird.
 * 2 - Objekte werden mit einer anderen Klasse ("ActivityBuilder") erstellt.
 * Diese weist auch die IDs zu (und ermittelt sie, wo nötig).
 * 3 - Individual POSIX file write of multifile to a storage target SAS controller
 * which has a LUN
 * In this case Component.HWid := HWid(LUN) = some kind of controller
 * bus id = pci@0000:00:1f.2.
 * The controller has a Component.name which is a string "SAS Controller Model".
 * The Component.SWid is the software layer composition "POSIX-Kernel-PFS-Block"
 * or any of those individually.
 * 4 - MPI Write using etypes - going through ROMIO or OMPIO the through ADIO layer
 * to kernel and to generic_Write
 * How is in this case the component.HWid discovered?
 * Component.SWid would be the string "MPI-OMPIO-ADIO" or any of those individually.
 */

/**
 * REQUIREMENTS (and the use cases they are based on)
 * ============
 * 1 - Every activity needs to acommodate an arbitrary number of attributes, metrics
 * and remote calls. (1)
 * 2 - Once set, the members are to be immutable. (1)
 * 3 - serializable to byte streams or to text files first human readable
 * using protocol buffers or boost
 */

/**
 * DESIGN CONSIDERATIONS
 * =====================
 * 1 - We may need to provide a member to hold the activity's "opcode".
 */

/**
 * OPEN ISSUES
 * ===========
 * 1 - Add member vars, initialisation and read access for attributes, metrics and remote calls.
 */


#infdef __SIOX_ACTIVITY_H
#define __SIOX_ACTIVITY_H

/*!
 We define classes in the namespace monitoring. Is it accessible from other namespaces?
 The activity should be public to have other multiplexer, componentcontent_reader and componentcontent_writer access possibilities.
 */


using std::string;


namespace monitoring {


typedef Timestamp uint64_t;


class Activity
{
private:
    uint64_t aid;
    uint64_t paid;
    uint64_t cid;
    Timestamp time_start;
    Timestamp time_stop;
    int status;
    string comment;
    
    // List<Attribute> attributes;
    // List<Metric> metrics; // List<Observable> ?
    // List<RemoteCall> remotecalls;

public:
  Activity(void);
  // Activity(Component component, Timestamp t_start = NULL, string comment="");
  ~Activity(void);

  /** Supply a dummy object for testing until ActivityBuilder exists.
   * Call like this:
   *      Activity dummyActivity = "Activity::getDummy()"
   */
  static Activity getDummy();

  // Getter methods
  uint64_t get_aid();
  uint64_t get_paid();
  uint64_t get_unid();
  Timestamp get_time_start();
  Timestamp get_time_stop();
  string get_comment();


};

}


#endif // __SIOX_ACTIVITY_H
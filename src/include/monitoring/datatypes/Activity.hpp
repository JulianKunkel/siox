/** @file Activity.hpp
 *
 * This activity is the structure to be read and written to
 * txt using this header.
 *
 * @author Julian Kunkel, Michaela Zimmer, Marc Wiedemann
 * @date   2013
 *
 */

/**
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
 * 1 -
 */


#ifndef SIOX_ACTIVITY_H
#define SIOX_ACTIVITY_H

/*!
 The activity should be public to have other multiplexer, componentcontent_reader and componentcontent_writer access possibilities.
 */

#include <string>
#include <vector>

#include <memory>

#include <monitoring/datatypes/ids.hpp>

using namespace std;

namespace monitoring {
// The instance identifier such as "Port 4711" is relevant for matching of remote calls
// See @TODO

	//@serializable
	struct RemoteCall{
		RemoteCallIdentifier target;
		vector<Attribute> attributes;
	};

	//@serializable
	class Activity {
		public:
			// The ontology provides a single ID for each ActivityType of a component, e.g. POSIX "open()"
			UniqueComponentActivityID ucaid_;

			Timestamp time_start_;
			Timestamp time_stop_;

			ActivityID aid_;

			vector<ActivityID> parentArray_;
			vector<RemoteCall> remoteCallsArray_;
			vector<Attribute> attributeArray_;

			// If we are caused by a remote, we have to identify it.
			RemoteCallIdentifier * remoteInvoker_; // NULL if none

			// interface specific error value.
			ActivityError errorValue_;

			Activity( UniqueComponentActivityID ucaid, Timestamp start_t, Timestamp end_t, ActivityID aid, const vector<ActivityID> & parentArray, const vector<Attribute> & attributeArray, const vector<RemoteCall> & remoteCallsArray, RemoteCallIdentifier * remoteInvoker, ActivityError errorValue )
				:
				ucaid_( ucaid ), time_start_( start_t ), time_stop_( end_t ),
				aid_( aid ), parentArray_( std::move( parentArray ) ),
				remoteCallsArray_( std::move( remoteCallsArray ) ),
				attributeArray_( std:: move( attributeArray ) ),
				remoteInvoker_( remoteInvoker ), errorValue_( errorValue ) {}

			Activity() : remoteInvoker_(nullptr), errorValue_(0) {}

			void print() {
				/// @todo: Really need reflection - have a look at Boost :-)
				unsigned i;
				cout << endl << "Activity = " << this << endl;
				cout << "t_start = " << time_start_ << endl;
				cout << "t_stop  = " << time_stop_ << endl;
				cout << "ActivityID = " << aid_ << endl;
				cout << "ActivityID.id = " << aid_.id << endl;
				cout << "ActivityID.thread = " << aid_.thread << endl;
				cout << "ActivityID.ComponentID.num = " << aid_.cid.id << endl;
				cout << "ActivityID.ComponentID.ProcessID.NodeID = " << aid_.cid.pid.nid << endl;
				cout << "ActivityID.ComponentID.ProcessID.pid    = " << aid_.cid.pid.pid << endl;
				cout << "ActivityID.ComponentID.ProcessID.time   = " << aid_.cid.pid.time << endl;
				cout << "Attributes (" << attributeArray_.size() << " items):" << endl;
				for( i = 0; i < attributeArray_.size(); i++ ) {
					cout << "(" << i << ")" << endl;
					cout << "\tid    = " << attributeArray_[i].id << endl;
					cout << "\tvalue = " << attributeArray_[i].value << endl;
				}
				cout << "Parents " << parentArray_.size() << endl;
				for( i = 0; i < parentArray_.size(); i++ ) {
					cout << "\t" <<  parentArray_[i] << endl;
				}
				cout << "RemoteCalls (" << remoteCallsArray_.size() << " items):" << endl;
			}

			inline Timestamp duration() const{
				return time_stop_ - time_start_;
			}

			inline double durationInS() const{
				return duration() * 0.001 * 0.001 * 0.001;
			}

			inline Timestamp time_start() const {
				return time_start_;
			}

			inline Timestamp time_stop() const {
				return time_stop_;
			}


			inline UniqueComponentActivityID ucaid() const {
				return ucaid_;
			}

			inline ActivityID aid() const {
				return aid_;
			}

			inline const vector<ActivityID>& parentArray() const {
				return parentArray_;
			}

			inline const vector<RemoteCall>& remoteCallsArray() const {
				return remoteCallsArray_;
			}

			inline const vector<Attribute>& attributeArray() const {
				return attributeArray_;
			}

                        inline void replaceAttribute(const Attribute & replace){
                           for(auto itr=attributeArray_.begin(); itr != attributeArray_.end(); itr++) {
                               if( itr->id == replace.id ){
                                  *itr = replace;
                                  return;
                              }
                           }
                           attributeArray_.push_back(replace);
                        }

			inline const RemoteCallIdentifier * remoteInvoker() const {
				return remoteInvoker_;
			}

			inline ActivityError errorValue() const {
				return errorValue_;
			}

			inline const AttributeValue * findAttribute(OntologyAttributeID aID) const{
				for(auto itr = attributeArray_.begin(); itr != attributeArray_.end(); itr++){
					if (itr->id == aID){
						return & itr->value;
					}
				}
				return nullptr;
			}

	};

}



#endif // SIOX_ACTIVITY_H

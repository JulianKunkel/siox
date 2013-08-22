#include <iostream>
#include <list>

#include <monitoring/datatypes/Activity.hpp>
#include <monitoring/activity_multiplexer/ActivityMultiplexer.hpp>
#include <core/module/ModuleLoader.hpp>

using namespace std;
using namespace monitoring;


class AnomalyPlugin{
private:
	enum token_type{
		DISCARD_ACTIVITY, 
		SHORT_READ
	};

	// Filter funktion, generiert Tokens von Activites basierend auf name und Attributen der Aktivität ...
	token_type filter_activity(Activity * activity){
		// TODO IMPLEMENT ME
		return SHORT_READ;
	}

public:
	AnomalyPlugin(ActivityMultiplexer * multiplexer);	

	// Diese Funktion wird aufgerufen, wenn im synchronen Pfad eine Aktivität vorliegt.
	void processSync(Activity * activity){
		cout << activity->name() << " SYNC" << endl;
		token_type t = filter_activity(activity);
		if (t == DISCARD_ACTIVITY) { // keine Veränderung des eigenen Zustands nötig
			return;
		}
		cout << "Aktivitätstoken: " << t << endl;
		// Verändere den eigenen Zustand basierend auf dem Token ...
		// TODO
	}

	void processAsync(Activity * activity){
		cout << activity->name() << " ASYNC" << endl;
		// Dieser Pfad wird erst für das zweite Plugin relevant, ist zu testzwecken aber mal eingetragen.
	}
};


// Diese Klassen werden für dein Plugin genau einmal registriert, hier sollte im Moment keine Änderung nötig sein.
// Bitte implementieren.
// Siehe UML Diagramme unter doc/*.svg
class ActivityListenerSync : public ActivityMultiplexerListenerSync
{
public:
	ActivityListenerSync(AnomalyPlugin * ap){
		parentPlugin = ap;
	}
	void Notify(Activity * activity);
private:
	AnomalyPlugin * parentPlugin;
};

class ActivityListenerAsync : public ActivityMultiplexerListenerAsync
{
public:
	void Notify(Activity * activity);
	void Reset(int dropped);

	ActivityListenerAsync(AnomalyPlugin * ap){
		parentPlugin = ap;
	}
private:
	AnomalyPlugin * parentPlugin;
};


void ActivityListenerSync::Notify(Activity * activity)
{
	parentPlugin->processSync(activity);
}


void ActivityListenerAsync::Notify(Activity * activity)
{
	parentPlugin->processAsync(activity);
}


void ActivityListenerAsync::Reset(int dropped)
{
}


// Diese Funktion evtl. anpassen um Plugin Eigenschaften zu laden etc...
AnomalyPlugin::AnomalyPlugin(ActivityMultiplexer * multiplexer){
	ActivityMultiplexerListener * l1 = new ActivityListenerSync(this);
	ActivityMultiplexerListener * l2 = new ActivityListenerAsync(this);

	multiplexer->registerListener(l1);
	multiplexer->registerListener(l2);

	// TODO load settings from config file etc...
}



int main(int argc, char const *argv[]){
	ActivityMultiplexer * m1 = core::module_create_instance<ActivityMultiplexer>("", "ActivityMultiplexerAsyncThreaded", "monitoring_activitymultiplexer");

	// wir registrieren das Plugin (normal geschieht das automatisch)
	AnomalyPlugin * ap = new AnomalyPlugin(m1);

	// Werden nun Aktivitäten eingegeben, so werden diese über Notify an das Plugin übergeben. Bspw:
	// string name, uint64_t start_t, uint64_t end_t, list<string> * attributes

	// Hier mal Testcode, beliebig erweitern!!!
	// TODO weitere testaktivitäten loggen...
	ComponentID cid = {.pid = {2,3,4}, .uuid= {1,2}};
	auto * parentArray = new vector<ComponentID>{{.pid = {1,2,3}, .uuid= {2,2}}};
	auto * attributeArray = new vector<Attribute>{{.id=111, .value = "myData"}, {.id=3, . value = (uint64_t) 4711}};
	auto * remoteCallsArray = new vector<RemoteCall>();
	// Cast the real activity to the serializable object class wrapper
	Activity * activity = new Activity("test", 3, 5, cid, parentArray, attributeArray, remoteCallsArray, NULL, 0);

	m1->Log(activity);
	m1->Log(activity);
	m1->Log(activity);


	m1->shutdown();

	return 0;
}




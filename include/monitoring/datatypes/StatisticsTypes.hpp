/**
 * @file    StatisticsTypes.hpp
 *
 * A (software) component for datatypes used in statistics.
 *
 * @author Julian Kunkel, Marc Wiedemann
 * @date   2013
 *
 */


#ifndef DATATYPES_STATISTICS_H
#define DATATYPES_STATISTICS_H

#include <boost/variant.hpp>
#include <iostream>

#include <string>
#include <vector>
#include <utility>
#include <map>

using namespace std;

namespace monitoring{

/*
Statistical Attributes 
- Startzeit und Endzeit = Intervall
- "Metric identifier" in Ontologie: "throughput/network/[MIN/MAX/AVG]" 
--- STORAGE_TYPE
--- SI_Unit
- Value
- HWID

Was machen wir mit multipler Hardware? z.B. Ethernet + IB? Beides network. Associated Info ist dann "NetworkType"
=> throughput/network/filesystem, throughput/network/management

Semantische Plugins?

Können abgeleitete Metriken durch Abonnement von bestehenden Metriken erzeugt werden?
Wäre möglich, dann aber evtl. Ursprungsmetriken beim Forwarding ignorieren.
!!!! => MetrikPlugin registriert sich bei MetrikKomponente !!!!
=> Jede Metrik/Plugin kann in der Konfig fürs Forwarding deaktiviert werden.


- Typ: gauge / inkrementell etc. => Wir verwenden: delta value für delta time

-- Alle Metriken werden dahingehend konvertiert => Alles Hilfsprogramme / Wrapper => SIOX_Extensions!!!
- Selbiges für Speichertypen

Randbedingungen:
1) Beim forwarding / verlorener Statistiken problematisch

Es gibt keine Softwarestatistiken? Früher bei PVFS?
Was machen wir mit GPFS-Statistiken? Die sind erstmal nur globale Statistiken für die viele Writes und ähnliche. Einzelstatistiken sind schwer zu bekommen, evtl über DMAPI. Andriy fragen.
z.B. Request Count...
Es gibt eine Shared Memory Region in der die Werte abgelegt werden müssen.
Registrierung beim Daemon für eine bestimmte Metrik (Gauge / Incremental).
=> Global pro Knoten nur genau eine.



StatisticsSeries, for data effective storage
*/

typedef boost::variant<int64_t, uint64_t, int32_t, uint32_t, float, double> StatisticsValue;

enum StatisticsEntity{
	NETWORK,
	INPUT_OUTPUT,
	CPU,
	MEMORY,
	OS,
	HARDWARE_SPECIFIC,
	SOFTWARE_SPECIFIC
};

enum StatisticsScope{
	GLOBAL,
	NODE,
	DEVICE,
	COMPONENT
};

enum StatisticsIntervall{
	HUNDRED_MILLISECONDS,
	SECOND,
	TEN_SECONDS,
	MINUTE,
	TEN_MINUTES
};

enum StatisticsReduceOperator{
	MIN,
	MAX,
	COUNT,
	AVERAGE,
	SUM
};


class StatisticsDescription{
public:
	StatisticsEntity entity;
	StatisticsScope  scope;

	string metrics;

	vector<pair<string,string> > topology;
	//1)map<string,string> PAIR;
	//1)map<string,PAIR> topology;
	//1)topology MapofMapObject;

	// Less overhead than vector<pair<string,string>>:
	typedef map<string,map<string,string>> MapofMap;
	MapofMap MetricMapObject;


	string si_unit;

	/* TODO StatisticsDescription(XX){} @Julian Marc: What is the aim here ???
	 IS this the user readable description as in the StatisticPlugins?
	 Side note: Found out that vector pair as two more unnecessary copies than MapofMap */
};

}

#endif


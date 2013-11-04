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

#include <iostream>

#include <string>
#include <vector>
#include <utility>
#include <map>

#include <core/datatypes/VariableDatatype.hpp>
#include <monitoring/datatypes/GenericTypes.hpp>
#include <monitoring/ontology/OntologyDatatypes.hpp>

using namespace std;

namespace monitoring {

	/*
	Statistical Attributes
	- Startzeit und Endzeit = Interval
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

	typedef VariableDatatype StatisticsValue;

	enum StatisticsEntity {
		NETWORK,
		INPUT_OUTPUT,
		CPU,
		MEMORY,
		OS,
		HARDWARE_SPECIFIC,
		SOFTWARE_SPECIFIC
	};

	enum StatisticsScope {
		GLOBAL,
		NODE,
		DEVICE,
		COMPONENT
	};

	typedef enum StatisticsInterval {
		HUNDRED_MILLISECONDS,
		SECOND,
		TEN_SECONDS,
		MINUTE,
		TEN_MINUTES,
		INTERVALLS_NUMBER
	} StatisticsInterval;

	enum StatisticsReduceOperator {
		MIN,
		MAX,
		COUNT,
		AVERAGE,
		SUM
	};

	//@serializable
	class StatisticsDescription {
		public:
			OntologyAttributeID ontologyId;
			vector<pair<string, string> > topology;	//TODO: Create a service that can map this to an ID for faster comparison.

			StatisticsDescription(const OntologyAttributeID & attribute, const vector<pair<string, string> > & topology) :
				ontologyId(attribute),
				topology(topology)
			{};

			StatisticsDescription(){};

			bool operator==(StatisticsDescription const & what){
				return this->ontologyId == what.ontologyId && this->topology == what.topology;
			}
	};

	//@serializable
	class StatisticsDescriptionAndValue {
		public:
			StatisticsDescription desc;
			StatisticsValue value;
			Timestamp time;

			StatisticsDescriptionAndValue(StatisticsDescription desc, const StatisticsValue & value, Timestamp time) : desc(desc), value(value), time(time){};
			
			StatisticsDescriptionAndValue(){};
	};

}

#endif


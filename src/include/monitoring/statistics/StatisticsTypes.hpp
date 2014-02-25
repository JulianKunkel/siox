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
#include <monitoring/datatypes/Topology.hpp>
#include <workarounds.hpp>

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

	// enum StatisticsEntity {
	// 	NETWORK,
	// 	INPUT_OUTPUT,
	// 	CPU,
	// 	MEMORY,
	// 	OS,
	// 	HARDWARE_SPECIFIC,
	// 	SOFTWARE_SPECIFIC
	// };

	// enum StatisticsScope {
	// 	GLOBAL,
	// 	NODE,
	// 	DEVICE,
	// 	COMPONENT
	// };

	typedef enum StatisticsInterval {
		HUNDRED_MILLISECONDS = 0,
		SECOND,
		TEN_SECONDS,
		MINUTE,
		TEN_MINUTES,
		INTERVALLS_NUMBER
	} StatisticsInterval;
	ADD_ENUM_OPERATORS(StatisticsInterval)	//overloads arithmetic operators to work with this enum

	enum StatisticsReduceOperator {
		MIN = 0,
		MAX,
//		COUNT,	XXX: What is this reduction op supposed to do? Always return the latest value?
		AVERAGE,
		SUM,
		STATISTICS_REDUCE_OPERATOR_COUNT
	};
	ADD_ENUM_OPERATORS(StatisticsReduceOperator)	//overloads arithmetic operators to work with this enum

	//@serializable
	class StatisticsDescription {
		public:
			OntologyAttributeID ontologyId;
			TopologyObjectId topologyId;

			StatisticsDescription(const OntologyAttributeID & attribute, TopologyObjectId topologyId) :
				ontologyId(attribute),
				topologyId(topologyId)
			{};

			StatisticsDescription(){};

			bool operator==(StatisticsDescription const & what){
				return this->ontologyId == what.ontologyId && this->topologyId == what.topologyId;
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


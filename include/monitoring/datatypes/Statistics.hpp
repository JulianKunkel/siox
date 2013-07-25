/**
 * @file    Statistics.hpp
 *
 * @description A (software) component for datatypes used in statistics.
 * @standard    Preferred standard is C++11
 *
 * @author Marc Wiedemann
 * @date   2013
 *
 */


#ifndef DATATYPES_STATISTICS_H
#define DATATYPES_STATISTICS_H

#include "boost/variant.hpp"
#include <iostream>

/*
Statistical Attributes 
- Startzeit und Endzeit = Intervall
- "Metric identifier" in Ontologie: "throughput/network/[MIN/MAX/AVG]" 
--- STORAGE_TYPE
--- SI_Unit
- Value
- HWID

Was machen wir mit multipler Hardware? z.B. Ethernet + IB? Beides network Associated Info ist dann "NetworkType"
=> throughput/network/filesystem, throughput/network/management

Semantische Plugins?

Können abgeleitete Metriken durch Abonnement von bestehenden Metriken erzeugt werden?
Wäre möglich, dann aber evtl. Ursprungsmetriken beim Forwarding ignorieren.
!!!! => MetrikPlugin registriert sich bei MetrikKomponente !!!!
=> Jede Metrik/Plugin kann in der Konfig fürs Forwarding deaktiviert werden.


- type / gauge / incrementelle etc. => By default: delta value für delta time

-- Alle Metriken werden dahingehend konvertiert => Alles Hilfsprogramme / Wrapper => SIOX_Extensions!!!
- Selbiges für Speichertypen

Randbedingungen:
1) Beim forwarding / verlorener Statistiken problematisch

Es gibt keine Softwarestatistiken? Früher bei PVFS?
Was machen wir mit GPFS-Statistiken?
z.B. Request Count...
Es gibt eine Shared Memory Region in der die Werte abgelegt werden müssen.
Registrierung beim Daemon für eine bestimmte Metrik (Gauge / Incremental).
=> Global pro Knoten nur genau eine.



StatisticsSeries, for data effective storage
*/

typedef boost::variant<int64_t, uint64_t, int32_t, uint32_t, std::string, float, double> StatisticsValue;

typedef boost::variant<String, string, string, string, string, string, uint32_t> MetricAttributes;

#endif

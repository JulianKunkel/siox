#ifndef DATATYPES_STATISTICS_H
#define DATATYPES_STATISTICS_H

#include "boost/variant.hpp"
#include <iostream>

/*
Statistics
- startime
- entime
- metric identifier in ontology: "throughput/network/[MIN/MAX/AVG]" 
--- STORAGE_TYPE
--- Base Unit
- Value
- HWID

Was machen wir mit multiplen Hardware? z.B. Ethernet + IB?
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

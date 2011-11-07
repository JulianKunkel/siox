/**
 * Description:
 * @author 
 * @year 
 */

#ifndef siox_LL_H
#define siox_LL_H

typedef struct siox_unid_t * siox_unid;

enum siox_value_type{
	SIOX_TYPE_INTEGER,
	SIOX_TYPE_LONG,
	SIOX_TYPE_STRING
};

/**
 * Meldet den Knoten als Teilnehmer an siox an und teilt ihm eine frische UNID zu.
 *
 * SIOX nutzt die übergeben Daten, um einen neuen IOPm-Knoten zu erzeugen und – wenn möglich – in ggf.
 * existierende Graphen einzubinden. Die Parameter stehen für InstanzID (je nach Knotenart, z.B. die ProzeßID
 * eines Threads oder der IP-Adresse und Port eines Servercaches), SoftwareID (z.B. „MPI“ oder „POSIX“) und
 * HardwareID (z.B. „Blizzard Node 5“ oder die MacID einer NAS-Festplatte). Der Knoten kann SWID und sogar HWID
 * bereitstellen. Andernfalls bestimmt SIOX sie selbst.
 * 
 * Wo SIOX die nötigen Daten selbst herausfinden kann, darf statt des Parameters NULL stehen.
 */
siox_unid siox_register_node(const char * hwid, const char * swid, const char * iid);

/**

 */
void siox_unregister_node(siox_unid unid)

/**
 * Meldet weitere relevante Attribute des Knotens an siox.
 *
 * Beispiel: Ein Cache, der seine Kapazität angibt.
 * Capacity: 10000
 */
void siox_register_attribute(siox_unid unid, const char * key, enum siox_value_type type, void * value);


/**
 * Meldet eine Kante zu einem potentiellen Kindesknoten im IOPm-Graphen bei siox an.
 *
 * Ruft ein Knoten später siox_register() auf, versucht ihn siox anhand dieser Informationen in den vorhandenen
 * Graphen einzupflegen.
 */
void siox_register_edge(siox_unid unid, const char * child_swid)

/**
 * Meldet die Fähigkeit, eine bestimmte Deskriptorübersetzung auszuführen, bei siox an.
 */
siox_descriptor_map_id siox_register_descriptor_map(siox_unid unid, const char * source_descriptor_label,  enum siox_value_type source_type, const char * taget_descriptor_label,  enum siox_value_type target_type)

/**
 * Der Knoten beendet – soweit es siox betrifft – seine Arbeit und stößt damit abschließende Speicher- und
 * Analysearbeiten an.
 */
//void siox_create_descriptor(siox_unid unid, Deskriptortyp, Deskriptor)


/**
 * Meldet das Erzeugen eines neuen Deskriptors an siox.
 *
 * Die Funktion ist für jeden Deskriptor, der nicht von einem anderen abgeleitet oder von einem anderen Knoten
 * empfangen wird, aufzurufen.
 *
 * Diese Funktion ist optional.
 *
 * Beispiel: Ein Dateiname, den der Client durch eine Benutzereingabe erhalten hat.
 */
void siox_send_descriptor(siox_unid unid, const char * child_swid, Deskriptortyp, Deskriptor);

/**
 * Meldet die Übergabe eines Deskriptors an einen Kindesknoten.
 *
 * Beispiel: Ein Knoten fordert das Lesen aus einer Datei an, die über ihr Dateihandle identifiziert wird.
 */
void siox_receive_descriptor(siox_unid unid, Deskriptortyp, Deskriptor);


/**
 * Meldet die Ableitung eines Deskriptors aus einem anderen.
 *
 * Beispiel: Einem Dateinamen wird ein Dateihandle zugeordnet.
 */
void siox_map_descriptor(siox_unid unid, siox_descriptor_map_id mapID, void * src, void * target);

/**
 * Meldet, daß ein Deskriptor in einem Knoten nicht mehr genutzt wird.
 *
 * SIOX kennzeichnet alle Verknüpfungen zwischen ihm und verbundenen Deskriptoren in der Datenbank als inaktiv.
 *
 * Diese Funktion ist optional.
 *
 * Beispiel: Ein Dateihandle wird beim Schließen seiner Datei freigegeben.
 */
void siox_release_descriptor(siox_unid unid, Deskriptortyp, Deskriptor)

/**
 * Meldet den Start einer Aktivität und erhält eine Activity ID (AID) dafür.
 *
 * SIOX speichert den zugehörigen Zeitstempel und wieviele Aktivitäten der Knoten zur Zeit offenhält.
 */
AID siox_start_activity(siox_unid unid);

/**
 * Meldet den Abschluß einer Aktivität.
 *
 * SIOX speichert auch hier den zugehörigen Zeitstempel.
 *
 * Sollte eine Wiederaufnahme der Aktivität mittels siox_start_activity() möglich sein?
 */
void siox_stop_activity(AID);

/**
 * Meldet im Laufe der Aktivität gemessene Leistungsdaten und den Deskriptor, dem sie kausal zuzuordnen sind.
 *
 * Beispiel: Das Senden einer Anzahl Bytes an einen übergeordneten Knoten, die über ein Dateihandle angefordert
 * wurden.
 *
 * Woher weiß die Komponente, ob – und wenn ja, auf was – wir die Leistungsdaten beschränken wollen?
 */
void siox_report_activity(AID, Deskriptortyp, Deskriptor, Meßgröße, Wert [, Details])

/**
 * Meldet das Ende der Berichtsphase für die Aktivität und schließt sie für siox.
 *
 * Sind außer den Zeitstempeln keine weiteren Leistungsdaten zu melden, kann die Funktion auch direkt statt
 * siox_stop_activity() aufgerufen werden.
 */
void siox_end_activity(AID);

/**
 * Meldet gemessene Leistungsdaten, die keinem einzelnen Deskriptor kausal zugeordnet werden können.
 *
 * Beispiel: Die maximale Prozessorauslastung der vom Knoten repräsentierten Komponente während der letzten
 * Sekunde.
 *
 * Wann wird was gemeldet? Woher weiß die Komponente, ob – und wenn ja, auf was – wir die Leistungsdaten
 * beschränken wollen?
 */
void siox_report(siox_unid unid, Meßgröße, Wert [, Details])


#endif

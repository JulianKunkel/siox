/**
 * @file    siox-ll.h
 *          Headerdatei für das SIOX-Low-Level-Interface
 *
 * @authors Michaela Zimmer, Julian Kunkel & Marc Wiedemann
 * @date    2011
 *          GNU Public License
 */


#ifndef siox_LL_H
#define siox_LL_H

#include "../ontology/ontology.h"


/**
 * Die <em>Unique Node ID</em>.
 *
 * Identifiziert einen Knoten im IOPm-Graphen.
 */
typedef struct siox_unid_t * siox_unid;


/**
 * Die <em>Activity ID</em>.
 *
 * Identifiziert eine Aktivität.
 */
typedef struct siox_aid_t * siox_aid;


/**
 * Die <em>Descriptor Map ID</em>.
 *
 * Identifiziert eine Descriptor Map, d.h., ein Paar von zwei Deskriptortypen,
 * wobei der zweite aus dem ersten abgeleitet werden kann.
 */
typedef struct siox_dmid_t * siox_dmid;


/**
 * Ein Indikator für den Typ eines folgenden SIOX-Parameters.
 *
 * Gibt in einigen Funktionen an, von welchem Typ ein folgender übergebener Wert sein wird.
 */
enum siox_value_type{
    SIOX_TYPE_INTEGER,
    SIOX_TYPE_LONG,
    SIOX_TYPE_FLOAT,
    SIOX_TYPE_STRING
};




/**
 * Meldet den Knoten als Teilnehmer an SIOX an und teilt ihm eine frische @em UNID zu.
 *
 * SIOX nutzt die übergeben Daten, um einen neuen IOPm-Knoten zu erzeugen und – wenn möglich – in ggf.
 * existierende Graphen einzubinden.
 *
 * @param[in]   hwid    Die @em HardwareID des meldenden Knoten, z.B. „Blizzard Node 5“ oder die MacID
 *                      einer NAS-Festplatte.
 * @param[in]   swid    Die @em SoftwareID des meldenden Knoten, z.B. „MPI“ oder „POSIX“.
 * @param[in]   iid     Die @em InstanzID des meldenden Knoten (je nach Knotenart, z.B. die ProzeßID
 *                      eines Threads oder IP-Adresse und Port eines Servercaches)
 *
 * @return  Eine frische @em UNID, die SIOX dem Knoten zuteilt und welche dieser im künftigen
 *          Umgang mit SIOX verwenden muß.
 *
 * @note Wo SIOX die nötigen Daten selbst herausfinden kann, darf statt des Parameters @c NULL stehen.
 */
siox_unid siox_register_node(const char * hwid, const char * swid, const char * iid);


/**
 * Meldet den Knoten bei SIOX ab.
 *
 * @param[in]   unid    Die @em UNID, die SIOX dem Knoten zugeteilt hatte.
 */
void siox_unregister_node(siox_unid unid);


/**
 * Meldet weitere relevante Attribute des Knoten an SIOX.
 *
 * @em Beispiel:    Ein Cache, der seine Kapazität mit 10.000 Bytes angibt.
 *
 * @param[in]   unid        Die @em UNID des Knoten.
 * @param[in]   key         Eine textuelle Beschreibung des Attributs (hier z.B.: "Capacity/Bytes").
 * @param[in]   value_type  Der Typ des folgenden Wertes als siox_value_type.
 * @param[in]   value       Ein Zeiger auf den tatsächlichen Wert des Attributs (hier z.B.: 10.000).
 */
void siox_register_attribute(siox_unid unid, const char * key, enum siox_value_type value_type, void * value);


/**
 * Meldet eine Kante zu einem potentiellen Kindesknoten im IOPm-Graphen bei SIOX an.
 *
 * Ruft ein Knoten später siox_register() auf, versucht ihn SIOX anhand dieser Informationen
 * in den vorhandenen Graphen einzupflegen.
 *
 * @param[in]   unid        Die @em UNID des Knoten.
 * @param[in]   child_swid  Die @em SoftwareID des potentiellen Kindesknoten, z.B. "HDF5".
 */
void siox_register_edge(siox_unid unid, const char * child_swid);


/**
 * Meldet die Fähigkeit, eine bestimmte Deskriptorübersetzung auszuführen, bei SIOX an.
 *
 * Das System kann aus den Möglichkeiten eines Knotens Schlüsse auf den Verlauf des
 * Datenpfades und die Aktivitäten darauf ziehen.
 *
 * @param[in]   unid            Die @em UNID des Knoten.
 * @param[in]   source_dtid     Die @em DTID des Quelldeskriptors.
 * @param[in]   target_dtid     Die @em DTID des Zieldeskriptors.
 *
 * @return      Eine <em> DeskriptorMapID (DMID)</em>, die der Knoten angeben muß, wenn er SIOX mit
                siox_map_descriptor() eine Anwendung dieser Abbildung meldet.
 */
siox_dmid siox_register_descriptor_map(siox_unid unid, siox_dtid source_dtid, siox_dtid target_dtid);


/**
 * Meldet das Erzeugen eines neuen Deskriptors an SIOX.
 *
 * Die Funktion ist für jeden Deskriptor, der nicht von einem anderen abgeleitet oder von einem anderen Knoten
 * empfangen wird, aufzurufen.
 *
 * @em Beispiel:    Ein Dateiname, den der Client durch eine Benutzereingabe erhalten hat.
 *
 * @param[in]   unid            Die @em UNID des Knoten.
 * @param[in]   dtid            Die @em DTID des folgenden Deskriptors
 * @param[in]   descriptor      Der eigentliche Deskriptor, z.B. "siox.c" oder "17".
 *
 * @note    Diese Funktion ist optional.
 */
void siox_create_descriptor(siox_unid unid, siox_dtid dtid, const char * descriptor);


/**
 * Meldet die Übergabe eines Deskriptors an einen Kindesknoten.
 *
 * @em Beispiel:    Ein Knoten fordert das Lesen aus einer Datei an,
 *                  die über ihr Dateihandle identifiziert wird.
 *
 * @param[in]   unid        Die @em UNID des Knoten.
 * @param[in]   child_swid  Die @em SoftWareID des empfangenden Kindesknoten, z.B. "HDF5".
 * @param[in]   dtid        Die @em DTID des Deskriptors.
 * @param[in]   descriptor  Der eigentliche Deskriptor, z.B. "17726".
 */
void siox_send_descriptor(siox_unid unid, const char * child_swid, siox_dtid dtid, const char * descriptor);


/**
 * Meldet den Empfang eines Deskriptors von einem Elternknoten.
 *
 * @em Beispiel:    Ein Knoten wird angewiesen, aus einer Datei zu lesen,
 *                  die über ihre Inode identifiziert wird.
 *
 * @param[in]   unid        Die @em UNID des Knoten.
 * @param[in]   dtid        Die @em DTID des Deskriptors.
 * @param[in]   descriptor  Der eigentliche Deskriptor, z.B. "53718332".
 */
void siox_receive_descriptor(siox_unid unid, siox_dtid dtid, const char * descriptor);


/**
 * Meldet die Ableitung eines Deskriptors aus einem anderen.
 *
 * @em Beispiel:    Einem Dateinamen wird ein Dateihandle zugeordnet.
 *
 * @param[in]   unid                Die @em UNID des Knoten.
 * @param[in]   dmid                Die @em DMID der Abbildung aus siox_register_descriptor_map().
 * @param[in]   source_descriptor   Der Ursprungsdeskriptor, z.B. "siox.c".
 * @param[in]   target_descriptor   Der Zieldeskriptor, z.B. "17".
 */
void siox_map_descriptor(siox_unid unid, siox_dmid dmid, const char * source_descriptor, const char * target_descriptor);


/**
 * Meldet, daß ein Deskriptor in einem Knoten nicht mehr genutzt wird.
 *
 * SIOX kennzeichnet alle Verknüpfungen zwischen ihm und verbundenen Deskriptoren
 * in der Datenbank als inaktiv.
 *
 * @em Beispiel:    Ein Dateihandle wird beim Schließen seiner Datei freigegeben.
 *
 * @param[in]   unid        Die @em UNID des Knoten.
 * @param[in]   dtid        Die @em DTID des Deskriptors.
 * @param[in]   descriptor  Der eigentliche Deskriptor, z.B. "51773".
 *
 * @note    Diese Funktion ist optional.
 */
void siox_release_descriptor(siox_unid unid, siox_dtid dtid, const char * descriptor);


/**
 * Meldet den Start einer Aktivität und erhält eine <em>Activity ID (AID)</em> dafür.
 *
 * SIOX speichert den zugehörigen Zeitstempel, wieviele Aktivitäten der Knoten zur Zeit offenhält
 * und alle Leistungsdaten, die für diese Aktivität künftig zugeordnet werden werden.
 *
 * @param[in]   unid    Die @em UNID des Knoten.
 * @param[in]   comment Ein Kommentar, der die Aktivität beschreibt, z.B. "Resultate von Lauf 17 speichern".
 *
 * @return      Eine frische @em AID, welche der Knoten für alle künftigen Meldungen,
 *              die diese Aktivität betreffen, angeben muß.
 */
siox_aid siox_start_activity(siox_unid unid, const char * comment);


/**
 * Meldet den Abschluß einer Aktivität.
 *
 * SIOX speichert auch hier den zugehörigen Zeitstempel, wartet jedoch noch auf mögliche
 * Meldungen von Leistungsdaten, die der Aktivität per siox_report_activity() zuzuordenen sind.
 *
 * @param[in]   aid Die @em AID der Aktivität.
 *
 * @todo    Sollte eine Wiederaufnahme der Aktivität mittels siox_restart_activity(aid) möglich sein?
 */
void siox_stop_activity(siox_aid aid);


/**
 * Meldet im Laufe der Aktivität gemessene Leistungsdaten und den Deskriptor, dem sie kausal zuzuordnen sind.
 *
 * @em Beispiel:    Das Senden einer Anzahl Bytes an einen übergeordneten Knoten,
 *                  die über ein Dateihandle angefordert wurden.
 *
 * @param[in]   aid             Die @em AID der Aktivität.
 * @param[in]   descriptor_type Die @em DTID des Deskriptors.
 * @param[in]   descriptor      Der eigentliche Deskriptor, z.B. "51773".
 * @param[in]   mid             Die @em MID der Meßgröße.
 * @param[in]   value_type      Der Typ des folgenden Wertes als siox_value_type.
 * @param[in]   value           Ein Zeiger auf den tatsächlichen Wert des Meßwerts (hier z.B.: 10.363).
 * @param[in]   details         Raum für Anmerkungen, weitere Spezifikationen oder Details.
 *
 * @todo    Woher weiß die Komponente, ob – und wenn ja, auf was – wir die Leistungsdaten beschränken wollen?
 */
void siox_report_activity(siox_aid aid, siox_dtid descriptor_type, const char * descriptor, siox_mid mid, enum siox_value_type value_type, void * value, const char * details);


/**
 * Meldet das Ende der Berichtsphase für die Aktivität und schließt sie für SIOX.
 *
 * @param[in]   aid Die @em AID der Aktivität.
 *
 * @note    Sind außer den Zeitstempeln keine weiteren Leistungsdaten zu melden,
 *          kann die Funktion auch direkt statt siox_stop_activity() aufgerufen werden.
 */
void siox_end_activity(siox_aid aid);


/**
 * Meldet gemessene Leistungsdaten, die keinem einzelnen Deskriptor kausal zugeordnet werden können.
 *
 * @em Beispiel:    Die maximale Prozessorauslastung der vom Knoten repräsentierten Komponente
 *                  während der letzten Sekunde.
 *
 * @param[in]   unid            Die @em UNID des Knoten.
 * @param[in]   mid             Die @em MID der Meßgröße.
 * @param[in]   value_type      Der Typ des folgenden Wertes als siox_value_type.
 * @param[in]   value           Ein Zeiger auf den tatsächlichen Wert des Meßwerts (hier z.B.: 73,17).
 * @param[in]   details         Raum für Anmerkungen, weitere Spezifikationen oder Details.
 *
 * @todo    Wann wird was gemeldet? Woher weiß die Komponente, ob – und wenn ja, auf was – wir
 *          die Leistungsdaten beschränken wollen?
 */
void siox_report(siox_unid unid,  siox_mid  mid, enum siox_value_type value_type, void * value, const char * details);


#endif

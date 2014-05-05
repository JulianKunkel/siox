--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET lock_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

--
-- Name: siox; Type: COMMENT; Schema: -; Owner: siox
--

COMMENT ON DATABASE siox IS 'SIOX Transaction System DB';


--
-- Name: activity; Type: SCHEMA; Schema: -; Owner: siox
--

CREATE SCHEMA activity;


ALTER SCHEMA activity OWNER TO siox;

--
-- Name: SCHEMA activity; Type: COMMENT; Schema: -; Owner: siox
--

COMMENT ON SCHEMA activity IS 'Activities and Remote Calls';


--
-- Name: statistics; Type: SCHEMA; Schema: -; Owner: siox
--

CREATE SCHEMA statistics;


ALTER SCHEMA statistics OWNER TO siox;

--
-- Name: topology; Type: SCHEMA; Schema: -; Owner: siox
--

CREATE SCHEMA topology;


ALTER SCHEMA topology OWNER TO siox;

--
-- Name: plpgsql; Type: EXTENSION; Schema: -; Owner: 
--

CREATE EXTENSION IF NOT EXISTS plpgsql WITH SCHEMA pg_catalog;


--
-- Name: EXTENSION plpgsql; Type: COMMENT; Schema: -; Owner: 
--

COMMENT ON EXTENSION plpgsql IS 'PL/pgSQL procedural language';


SET search_path = public, pg_catalog;

--
-- Name: keyval_pair; Type: TYPE; Schema: public; Owner: siox
--

CREATE TYPE keyval_pair AS (
	key text,
	val text
);


ALTER TYPE public.keyval_pair OWNER TO siox;

SET search_path = activity, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: activities; Type: TABLE; Schema: activity; Owner: siox; Tablespace: 
--

CREATE TABLE activities (
    unique_id bigint NOT NULL,
    ucaid integer NOT NULL,
    time_start bigint NOT NULL,
    time_stop bigint,
    error_value integer,
    remote_calls bigint[],
    attributes character varying(1024)
);


ALTER TABLE activity.activities OWNER TO siox;

--
-- Name: TABLE activities; Type: COMMENT; Schema: activity; Owner: siox
--

COMMENT ON TABLE activities IS 'Activities';


--
-- Name: activity_ids; Type: TABLE; Schema: activity; Owner: siox; Tablespace: 
--

CREATE TABLE activity_ids (
    unique_id bigint NOT NULL,
    id integer NOT NULL,
    thread_id integer NOT NULL,
    cid_pid_nid integer NOT NULL,
    cid_pid_pid integer NOT NULL,
    cid_pid_time integer NOT NULL,
    cid_id integer NOT NULL
);


ALTER TABLE activity.activity_ids OWNER TO siox;

--
-- Name: TABLE activity_ids; Type: COMMENT; Schema: activity; Owner: siox
--

COMMENT ON TABLE activity_ids IS 'ActivityIDs';


--
-- Name: activity; Type: VIEW; Schema: activity; Owner: siox
--

CREATE VIEW activity AS
 SELECT activity_ids.unique_id,
    activities.ucaid,
    activities.time_start,
    activities.time_stop,
    activities.error_value,
    activities.remote_calls,
    activities.attributes,
    activity_ids.id,
    activity_ids.thread_id,
    activity_ids.cid_pid_nid,
    activity_ids.cid_pid_pid,
    activity_ids.cid_pid_time,
    activity_ids.cid_id
   FROM (activities
   RIGHT JOIN activity_ids USING (unique_id));


ALTER TABLE activity.activity OWNER TO siox;

--
-- Name: get_activity_by_aid(integer, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: activity; Owner: siox
--

CREATE FUNCTION get_activity_by_aid(id integer, thread_id integer, cpn integer, cpp integer, cpt integer, cid integer) RETURNS activity
    LANGUAGE plpgsql
    AS $$DECLARE act activity.activity%ROWTYPE;
BEGIN
SELECT * INTO act 
FROM activity.activity AS a
WHERE a.id = get_activity_by_aid.id AND a.thread_id = get_activity_by_aid.thread_id AND a.cid_pid_nid = get_activity_by_aid.cpn AND a.cid_pid_pid = get_activity_by_aid.cpp AND a.cid_pid_time = get_activity_by_aid.cpt AND a.cid_id = get_activity_by_aid.cid LIMIT 1;
RETURN act;
END
$$;


ALTER FUNCTION activity.get_activity_by_aid(id integer, thread_id integer, cpn integer, cpp integer, cpt integer, cid integer) OWNER TO siox;

--
-- Name: FUNCTION get_activity_by_aid(id integer, thread_id integer, cpn integer, cpp integer, cpt integer, cid integer); Type: COMMENT; Schema: activity; Owner: siox
--

COMMENT ON FUNCTION get_activity_by_aid(id integer, thread_id integer, cpn integer, cpp integer, cpt integer, cid integer) IS 'Returns activity given its ActivityID.';


SET search_path = topology, pg_catalog;

--
-- Name: relation; Type: TABLE; Schema: topology; Owner: siox; Tablespace: 
--

CREATE TABLE relation (
    parentobjectid integer,
    childname text,
    childobjectid integer,
    relationtypeid integer
);


ALTER TABLE topology.relation OWNER TO siox;

--
-- Name: TABLE relation; Type: COMMENT; Schema: topology; Owner: siox
--

COMMENT ON TABLE relation IS 'Relation';


SET search_path = activity, pg_catalog;

--
-- Name: activity_detail; Type: VIEW; Schema: activity; Owner: siox
--

CREATE VIEW activity_detail AS
 SELECT a.unique_id,
    a.ucaid,
    a.time_start,
    a.time_stop,
    a.error_value,
    a.remote_calls,
    a.attributes,
    a.id,
    a.thread_id,
    a.cid_pid_nid,
    a.cid_pid_pid,
    a.cid_pid_time,
    a.cid_id,
    t.parentobjectid,
    t.childname,
    t.childobjectid,
    t.relationtypeid
   FROM activity a,
    topology.relation t
  WHERE (t.childobjectid = a.ucaid);


ALTER TABLE activity.activity_detail OWNER TO siox;

--
-- Name: get_activity_by_uid(bigint); Type: FUNCTION; Schema: activity; Owner: siox
--

CREATE FUNCTION get_activity_by_uid(uid bigint) RETURNS activity_detail
    LANGUAGE plpgsql
    AS $$ 
DECLARE act activity.activity_detail%ROWTYPE;
BEGIN
SELECT * INTO act 
FROM activity.activity_detail AS a
WHERE a.unique_id = get_activity_by_uid.uid LIMIT 1;
RETURN act;
END
$$;


ALTER FUNCTION activity.get_activity_by_uid(uid bigint) OWNER TO siox;

--
-- Name: FUNCTION get_activity_by_uid(uid bigint); Type: COMMENT; Schema: activity; Owner: siox
--

COMMENT ON FUNCTION get_activity_by_uid(uid bigint) IS 'Returns activity given its Unique-ID.';


--
-- Name: parents; Type: TABLE; Schema: activity; Owner: siox; Tablespace: 
--

CREATE TABLE parents (
    child_id bigint NOT NULL,
    parent_id bigint NOT NULL
);


ALTER TABLE activity.parents OWNER TO siox;

--
-- Name: TABLE parents; Type: COMMENT; Schema: activity; Owner: siox
--

COMMENT ON TABLE parents IS 'Parent activities.';


--
-- Name: COLUMN parents.child_id; Type: COMMENT; Schema: activity; Owner: siox
--

COMMENT ON COLUMN parents.child_id IS 'Foreign key to activity_id.unique_id';


--
-- Name: COLUMN parents.parent_id; Type: COMMENT; Schema: activity; Owner: siox
--

COMMENT ON COLUMN parents.parent_id IS 'Foreign key to activity_id.unique_id';


--
-- Name: child; Type: VIEW; Schema: activity; Owner: siox
--

CREATE VIEW child AS
 SELECT a.unique_id,
    a.ucaid,
    a.time_start,
    a.time_stop,
    a.error_value,
    a.remote_calls,
    a.attributes,
    a.id,
    a.thread_id,
    a.cid_pid_nid,
    a.cid_pid_pid,
    a.cid_pid_time,
    a.cid_id,
    p.child_id,
    p.parent_id
   FROM (activity a
   RIGHT JOIN parents p ON ((a.unique_id = p.parent_id)));


ALTER TABLE activity.child OWNER TO siox;

--
-- Name: get_activity_children(bigint); Type: FUNCTION; Schema: activity; Owner: siox
--

CREATE FUNCTION get_activity_children(activity_unique_id bigint) RETURNS SETOF child
    LANGUAGE plpgsql
    AS $$BEGIN
RETURN QUERY SELECT * FROM activity.child WHERE activity.child.parent_id = get_activity_children.activity_unique_id;
END
$$;


ALTER FUNCTION activity.get_activity_children(activity_unique_id bigint) OWNER TO siox;

--
-- Name: FUNCTION get_activity_children(activity_unique_id bigint); Type: COMMENT; Schema: activity; Owner: siox
--

COMMENT ON FUNCTION get_activity_children(activity_unique_id bigint) IS 'Returns the children for an activity given the activity unique ID.';


--
-- Name: activity_list; Type: VIEW; Schema: activity; Owner: siox
--

CREATE VIEW activity_list AS
 SELECT a.unique_id,
    t.childname AS name,
    a.time_start,
    a.time_stop,
    a.error_value,
    b.cid_pid_nid,
    b.cid_pid_pid,
    b.cid_pid_time
   FROM activities a,
    activity_ids b,
    topology.relation t
  WHERE ((a.ucaid = t.childobjectid) AND (a.unique_id = b.unique_id));


ALTER TABLE activity.activity_list OWNER TO siox;

--
-- Name: get_activity_list(integer, integer, integer, integer, integer); Type: FUNCTION; Schema: activity; Owner: siox
--

CREATE FUNCTION get_activity_list(pid_nid integer, pid_pid integer, pid_time integer, page_size integer, page_offset integer) RETURNS SETOF activity_list
    LANGUAGE plpgsql
    AS $$
BEGIN
RETURN QUERY SELECT * FROM activity.activity_list WHERE cid_pid_nid = get_activity_list.pid_nid AND cid_pid_pid = get_activity_list.pid_pid AND cid_pid_time = get_activity_list.pid_time ORDER BY time_start ASC LIMIT get_activity_list.page_size OFFSET get_activity_list.page_offset;
END
$$;


ALTER FUNCTION activity.get_activity_list(pid_nid integer, pid_pid integer, pid_time integer, page_size integer, page_offset integer) OWNER TO siox;

--
-- Name: FUNCTION get_activity_list(pid_nid integer, pid_pid integer, pid_time integer, page_size integer, page_offset integer); Type: COMMENT; Schema: activity; Owner: siox
--

COMMENT ON FUNCTION get_activity_list(pid_nid integer, pid_pid integer, pid_time integer, page_size integer, page_offset integer) IS 'Returns all activitities sorted by time_start between page_size and page_size + page_offset.';


--
-- Name: parent; Type: VIEW; Schema: activity; Owner: siox
--

CREATE VIEW parent AS
 SELECT a.unique_id,
    a.ucaid,
    a.time_start,
    a.time_stop,
    a.error_value,
    a.remote_calls,
    a.attributes,
    a.id,
    a.thread_id,
    a.cid_pid_nid,
    a.cid_pid_pid,
    a.cid_pid_time,
    a.cid_id,
    p.child_id,
    p.parent_id
   FROM (activity a
   RIGHT JOIN parents p ON ((a.unique_id = p.child_id)));


ALTER TABLE activity.parent OWNER TO siox;

--
-- Name: get_activity_parents(bigint); Type: FUNCTION; Schema: activity; Owner: siox
--

CREATE FUNCTION get_activity_parents(activity_unique_id bigint) RETURNS SETOF parent
    LANGUAGE plpgsql
    AS $$BEGIN
RETURN QUERY SELECT * FROM activity.parent WHERE activity.parent.child_id = get_activity_parents.activity_unique_id;
END
$$;


ALTER FUNCTION activity.get_activity_parents(activity_unique_id bigint) OWNER TO siox;

--
-- Name: FUNCTION get_activity_parents(activity_unique_id bigint); Type: COMMENT; Schema: activity; Owner: siox
--

COMMENT ON FUNCTION get_activity_parents(activity_unique_id bigint) IS 'Returns the parents for an activity given the activity unique ID.';


--
-- Name: get_activity_uid(integer, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: activity; Owner: siox
--

CREATE FUNCTION get_activity_uid(id integer, thread_id integer, cpn integer, cpp integer, cpt integer, cid integer) RETURNS bigint
    LANGUAGE plpgsql
    AS $$DECLARE uid activity.activity.unique_id%TYPE;
BEGIN
SELECT unique_id INTO uid 
FROM activity.activity AS a
WHERE a.id = get_activity_uid.id AND a.thread_id = get_activity_uid.thread_id AND a.cid_pid_nid = get_activity_uid.cpn AND a.cid_pid_pid = get_activity_uid.cpp AND a.cid_pid_time = get_activity_uid.cpt AND a.cid_id = get_activity_uid.cid LIMIT 1;
RETURN uid;
END
$$;


ALTER FUNCTION activity.get_activity_uid(id integer, thread_id integer, cpn integer, cpp integer, cpt integer, cid integer) OWNER TO siox;

--
-- Name: FUNCTION get_activity_uid(id integer, thread_id integer, cpn integer, cpp integer, cpt integer, cid integer); Type: COMMENT; Schema: activity; Owner: siox
--

COMMENT ON FUNCTION get_activity_uid(id integer, thread_id integer, cpn integer, cpp integer, cpt integer, cid integer) IS 'Returns Unique-ID given its ActivityID.';


--
-- Name: remote_call_ids; Type: TABLE; Schema: activity; Owner: siox; Tablespace: 
--

CREATE TABLE remote_call_ids (
    unique_id bigint NOT NULL,
    nid integer,
    uuid integer,
    instance integer,
    activity_uid bigint
);


ALTER TABLE activity.remote_call_ids OWNER TO siox;

--
-- Name: TABLE remote_call_ids; Type: COMMENT; Schema: activity; Owner: siox
--

COMMENT ON TABLE remote_call_ids IS 'RemoteCallIdentifiers';


--
-- Name: remote_calls; Type: TABLE; Schema: activity; Owner: siox; Tablespace: 
--

CREATE TABLE remote_calls (
    unique_id bigint NOT NULL,
    attributes character varying(255)
);


ALTER TABLE activity.remote_calls OWNER TO siox;

--
-- Name: TABLE remote_calls; Type: COMMENT; Schema: activity; Owner: siox
--

COMMENT ON TABLE remote_calls IS 'RemoteCalls';


--
-- Name: remote_call; Type: VIEW; Schema: activity; Owner: siox
--

CREATE VIEW remote_call AS
 SELECT remote_call_ids.unique_id,
    remote_calls.attributes,
    remote_call_ids.nid,
    remote_call_ids.uuid,
    remote_call_ids.instance,
    remote_call_ids.activity_uid
   FROM (remote_calls
   RIGHT JOIN remote_call_ids USING (unique_id));


ALTER TABLE activity.remote_call OWNER TO siox;

--
-- Name: get_remote_call_by_activity_uid(bigint); Type: FUNCTION; Schema: activity; Owner: siox
--

CREATE FUNCTION get_remote_call_by_activity_uid(unique_id bigint) RETURNS SETOF remote_call
    LANGUAGE plpgsql
    AS $$BEGIN
RETURN QUERY SELECT * FROM activity.remote_call AS r WHERE r.activity_uid = get_remote_call_by_activity_uid.unique_id;
END
$$;


ALTER FUNCTION activity.get_remote_call_by_activity_uid(unique_id bigint) OWNER TO siox;

--
-- Name: FUNCTION get_remote_call_by_activity_uid(unique_id bigint); Type: COMMENT; Schema: activity; Owner: siox
--

COMMENT ON FUNCTION get_remote_call_by_activity_uid(unique_id bigint) IS 'Returns remote calls given its activity unique ID.';


--
-- Name: get_remote_call_by_rcid(integer, integer, integer, bigint); Type: FUNCTION; Schema: activity; Owner: siox
--

CREATE FUNCTION get_remote_call_by_rcid(nid integer, uuid integer, inst integer, act_uid bigint) RETURNS remote_call
    LANGUAGE plpgsql
    AS $$DECLARE rc activity.remote_call%ROWTYPE;
BEGIN
SELECT * INTO rc 
FROM activity.remote_call AS r
WHERE r.nid = get_remote_call_by_rcid.nid AND r.uuid = get_remote_call_by_rcid.uuid AND r.instance = get_remote_call_by_rcid.inst AND r.activity_uid = get_remote_call_by_rcid.act_uid LIMIT 1;
RETURN rc;
END
$$;


ALTER FUNCTION activity.get_remote_call_by_rcid(nid integer, uuid integer, inst integer, act_uid bigint) OWNER TO siox;

--
-- Name: FUNCTION get_remote_call_by_rcid(nid integer, uuid integer, inst integer, act_uid bigint); Type: COMMENT; Schema: activity; Owner: siox
--

COMMENT ON FUNCTION get_remote_call_by_rcid(nid integer, uuid integer, inst integer, act_uid bigint) IS 'Returns remote call given its RemoteCallID.';


--
-- Name: get_remote_call_by_uid(bigint); Type: FUNCTION; Schema: activity; Owner: siox
--

CREATE FUNCTION get_remote_call_by_uid(uid bigint) RETURNS remote_call
    LANGUAGE plpgsql
    AS $$ 
DECLARE rc activity.remote_call%ROWTYPE;
BEGIN
SELECT * INTO rc
FROM activity.remote_call AS r
WHERE r.unique_id = get_remote_call_by_uid.uid LIMIT 1;
RETURN rc;
END
$$;


ALTER FUNCTION activity.get_remote_call_by_uid(uid bigint) OWNER TO siox;

--
-- Name: FUNCTION get_remote_call_by_uid(uid bigint); Type: COMMENT; Schema: activity; Owner: siox
--

COMMENT ON FUNCTION get_remote_call_by_uid(uid bigint) IS 'Returns remote call given its Unique-ID.';


--
-- Name: get_remote_call_uid(integer, integer, integer, bigint); Type: FUNCTION; Schema: activity; Owner: siox
--

CREATE FUNCTION get_remote_call_uid(nid integer, uuid integer, inst integer, act_uid bigint) RETURNS bigint
    LANGUAGE plpgsql
    AS $$DECLARE uid activity.remote_calls.unique_id%TYPE;
BEGIN
SELECT unique_id INTO uid 
FROM activity.remote_calls AS r
WHERE r.nid = get_remote_call_uid.nid AND r.uuid = get_remote_call_uid.uuid AND r.instance = get_remote_call_uid.instance AND r.activity_uid = get_remote_call_uid.act_uid LIMIT 1;
RETURN uid;
END
$$;


ALTER FUNCTION activity.get_remote_call_uid(nid integer, uuid integer, inst integer, act_uid bigint) OWNER TO siox;

--
-- Name: FUNCTION get_remote_call_uid(nid integer, uuid integer, inst integer, act_uid bigint); Type: COMMENT; Schema: activity; Owner: siox
--

COMMENT ON FUNCTION get_remote_call_uid(nid integer, uuid integer, inst integer, act_uid bigint) IS 'Returns Unique-ID given its RemoteCallID.';


--
-- Name: reset_all(); Type: FUNCTION; Schema: activity; Owner: siox
--

CREATE FUNCTION reset_all() RETURNS void
    LANGUAGE sql
    AS $$TRUNCATE activity.activities, activity.activity_ids, activity.parents, activity.remote_calls, activity.remote_call_ids;
ALTER SEQUENCE activity.activity_id_unique_id_seq RESTART WITH 1;
ALTER SEQUENCE activity.remote_call_id_unique_id_seq RESTART WITH 1;
$$;


ALTER FUNCTION activity.reset_all() OWNER TO siox;

--
-- Name: FUNCTION reset_all(); Type: COMMENT; Schema: activity; Owner: siox
--

COMMENT ON FUNCTION reset_all() IS 'Truncates all tables in the schema activities and resets auto-incremental keys to 1.';


SET search_path = public, pg_catalog;

--
-- Name: convert_to_integer(text); Type: FUNCTION; Schema: public; Owner: siox
--

CREATE FUNCTION convert_to_integer(v_input text) RETURNS integer
    LANGUAGE plpgsql
    AS $$
DECLARE v_int_value INTEGER DEFAULT NULL;
BEGIN
    BEGIN
        v_int_value := v_input::INTEGER;
    EXCEPTION WHEN OTHERS THEN
        RAISE NOTICE 'Invalid integer value: "%".  Returning NULL.', v_input;
        RETURN NULL;
    END;
RETURN v_int_value;
END;
$$;


ALTER FUNCTION public.convert_to_integer(v_input text) OWNER TO siox;

--
-- Name: get_all_cmds(); Type: FUNCTION; Schema: public; Owner: siox
--

CREATE FUNCTION get_all_cmds() RETURNS SETOF text
    LANGUAGE plpgsql
    AS $$BEGIN
RETURN QUERY 
SELECT * FROM commands;
END
$$;


ALTER FUNCTION public.get_all_cmds() OWNER TO siox;

--
-- Name: get_all_nodes(); Type: FUNCTION; Schema: public; Owner: siox
--

CREATE FUNCTION get_all_nodes() RETURNS SETOF text
    LANGUAGE plpgsql
    AS $$BEGIN
RETURN QUERY 
SELECT * FROM nodes;
END

$$;


ALTER FUNCTION public.get_all_nodes() OWNER TO siox;

--
-- Name: get_all_users(); Type: FUNCTION; Schema: public; Owner: siox
--

CREATE FUNCTION get_all_users() RETURNS SETOF text
    LANGUAGE plpgsql
    AS $$BEGIN
RETURN QUERY 
SELECT * FROM users;
END
$$;


ALTER FUNCTION public.get_all_users() OWNER TO siox;

--
-- Name: get_program(integer); Type: FUNCTION; Schema: public; Owner: siox
--

CREATE FUNCTION get_program(pid integer) RETURNS SETOF topology.relation
    LANGUAGE plpgsql
    AS $$
BEGIN
RETURN QUERY SELECT * FROM topology.relation WHERE relationtypeid = (SELECT id FROM topology.type WHERE name='ProcessID') AND childobjectid = get_program.pid;
END
$$;


ALTER FUNCTION public.get_program(pid integer) OWNER TO siox;

--
-- Name: FUNCTION get_program(pid integer); Type: COMMENT; Schema: public; Owner: siox
--

COMMENT ON FUNCTION get_program(pid integer) IS 'Returns a program given its ID.';


--
-- Name: get_program_list(); Type: FUNCTION; Schema: public; Owner: siox
--

CREATE FUNCTION get_program_list() RETURNS SETOF topology.relation
    LANGUAGE plpgsql
    AS $$
BEGIN
RETURN QUERY SELECT * FROM topology.relation WHERE relationtypeid = (SELECT id FROM topology.type WHERE name='ProcessID');
END
$$;


ALTER FUNCTION public.get_program_list() OWNER TO siox;

--
-- Name: get_program_list(integer, integer); Type: FUNCTION; Schema: public; Owner: siox
--

CREATE FUNCTION get_program_list(page_size integer, page_offset integer) RETURNS SETOF topology.relation
    LANGUAGE plpgsql
    AS $$
BEGIN
RETURN QUERY SELECT * FROM topology.relation WHERE relationtypeid = (SELECT id FROM topology.type WHERE name='ProcessID') LIMIT get_program_list.page_size OFFSET get_program_list.page_offset;
END
$$;


ALTER FUNCTION public.get_program_list(page_size integer, page_offset integer) OWNER TO siox;

--
-- Name: reset_all_tables(); Type: FUNCTION; Schema: public; Owner: siox
--

CREATE FUNCTION reset_all_tables() RETURNS void
    LANGUAGE sql
    AS $$SELECT * FROM activity.reset_all();
SELECT * FROM topology.reset_all();
SELECT * FROM statistics.reset_all();$$;


ALTER FUNCTION public.reset_all_tables() OWNER TO siox;

--
-- Name: FUNCTION reset_all_tables(); Type: COMMENT; Schema: public; Owner: siox
--

COMMENT ON FUNCTION reset_all_tables() IS 'Truncate all tables and resets all sequences in the SIOX database.';


SET search_path = statistics, pg_catalog;

--
-- Name: reset_all(); Type: FUNCTION; Schema: statistics; Owner: siox
--

CREATE FUNCTION reset_all() RETURNS void
    LANGUAGE sql
    AS $$TRUNCATE statistics.stats;
ALTER SEQUENCE statistics.stats_stat_id_seq RESTART WITH 1;

$$;


ALTER FUNCTION statistics.reset_all() OWNER TO siox;

SET search_path = topology, pg_catalog;

--
-- Name: get_attribute_by_id(integer); Type: FUNCTION; Schema: topology; Owner: siox
--

CREATE FUNCTION get_attribute_by_id(id integer) RETURNS SETOF relation
    LANGUAGE plpgsql
    AS $$BEGIN
RETURN QUERY SELECT * FROM topology.relation WHERE childobjectid = get_attribute_by_id.id;
END
$$;


ALTER FUNCTION topology.get_attribute_by_id(id integer) OWNER TO siox;

--
-- Name: FUNCTION get_attribute_by_id(id integer); Type: COMMENT; Schema: topology; Owner: siox
--

COMMENT ON FUNCTION get_attribute_by_id(id integer) IS 'Returns attribute given its numeric ID.';


--
-- Name: attribute; Type: TABLE; Schema: topology; Owner: siox; Tablespace: 
--

CREATE TABLE attribute (
    id integer NOT NULL,
    name text,
    domaintypeid integer,
    datatype integer
);


ALTER TABLE topology.attribute OWNER TO siox;

--
-- Name: TABLE attribute; Type: COMMENT; Schema: topology; Owner: siox
--

COMMENT ON TABLE attribute IS 'Attribute';


--
-- Name: get_attribute_by_name(integer, text); Type: FUNCTION; Schema: topology; Owner: siox
--

CREATE FUNCTION get_attribute_by_name(type_id integer, name text) RETURNS SETOF attribute
    LANGUAGE plpgsql
    AS $$BEGIN
RETURN QUERY SELECT * FROM topology.attribute WHERE name = get_attribute_by_name.name AND domainTypeId = get_attribute_by_name.type_id;
END
$$;


ALTER FUNCTION topology.get_attribute_by_name(type_id integer, name text) OWNER TO siox;

--
-- Name: FUNCTION get_attribute_by_name(type_id integer, name text); Type: COMMENT; Schema: topology; Owner: siox
--

COMMENT ON FUNCTION get_attribute_by_name(type_id integer, name text) IS 'Returns attribute given name and domain ID.';


--
-- Name: get_attribute_by_object(integer, integer); Type: FUNCTION; Schema: topology; Owner: siox
--

CREATE FUNCTION get_attribute_by_object(obj_id integer, attr_id integer) RETURNS SETOF attribute
    LANGUAGE plpgsql
    AS $$BEGIN
RETURN QUERY SELECT * FROM topology.attribute WHERE objectId = get_attribute_by_object.obj_id AND attributeId = get_attribute_by_object.attr_id;
END
$$;


ALTER FUNCTION topology.get_attribute_by_object(obj_id integer, attr_id integer) OWNER TO siox;

--
-- Name: FUNCTION get_attribute_by_object(obj_id integer, attr_id integer); Type: COMMENT; Schema: topology; Owner: siox
--

COMMENT ON FUNCTION get_attribute_by_object(obj_id integer, attr_id integer) IS 'Returns attribute given an object id and attribute-id.';


--
-- Name: get_attributes_by_proc_number(integer); Type: FUNCTION; Schema: topology; Owner: siox
--

CREATE FUNCTION get_attributes_by_proc_number(p integer) RETURNS SETOF public.keyval_pair
    LANGUAGE plpgsql
    AS $$BEGIN
RETURN QUERY 
SELECT b.childname AS attribute, d.value AS value 
FROM topology.relation AS a, topology.relation AS b, topology.value AS d 
WHERE 	a.parentobjectid = get_attributes_by_proc_number.p  
AND 	b.childobjectid = a.childname::integer 
AND 	d.objectid = a.childobjectid;
END
$$;


ALTER FUNCTION topology.get_attributes_by_proc_number(p integer) OWNER TO siox;

--
-- Name: value; Type: TABLE; Schema: topology; Owner: siox; Tablespace: 
--

CREATE TABLE value (
    objectid integer,
    attributeid integer,
    type smallint,
    value text
);


ALTER TABLE topology.value OWNER TO siox;

--
-- Name: TABLE value; Type: COMMENT; Schema: topology; Owner: siox
--

COMMENT ON TABLE value IS 'Value';


--
-- Name: get_node_by_id(integer); Type: FUNCTION; Schema: topology; Owner: siox
--

CREATE FUNCTION get_node_by_id(nid integer) RETURNS value
    LANGUAGE plpgsql
    AS $$
DECLARE node topology.value%ROWTYPE;
BEGIN
SELECT * INTO node FROM topology.value WHERE objectid = get_node_by_id.nid;
RETURN node;
END
$$;


ALTER FUNCTION topology.get_node_by_id(nid integer) OWNER TO siox;

--
-- Name: FUNCTION get_node_by_id(nid integer); Type: COMMENT; Schema: topology; Owner: siox
--

COMMENT ON FUNCTION get_node_by_id(nid integer) IS 'Returns the node information given its numeric ID.';


--
-- Name: object; Type: TABLE; Schema: topology; Owner: siox; Tablespace: 
--

CREATE TABLE object (
    id integer NOT NULL,
    typeid integer
);


ALTER TABLE topology.object OWNER TO siox;

--
-- Name: TABLE object; Type: COMMENT; Schema: topology; Owner: siox
--

COMMENT ON TABLE object IS 'Object';


--
-- Name: get_object_by_id(integer); Type: FUNCTION; Schema: topology; Owner: siox
--

CREATE FUNCTION get_object_by_id(id integer) RETURNS SETOF object
    LANGUAGE plpgsql
    AS $$BEGIN
RETURN QUERY SELECT * FROM topology.object AS o WHERE o.id = get_object_by_id.id;
END
$$;


ALTER FUNCTION topology.get_object_by_id(id integer) OWNER TO siox;

--
-- Name: FUNCTION get_object_by_id(id integer); Type: COMMENT; Schema: topology; Owner: siox
--

COMMENT ON FUNCTION get_object_by_id(id integer) IS 'Returns object given its ID.';


--
-- Name: get_relation(integer, integer, text); Type: FUNCTION; Schema: topology; Owner: siox
--

CREATE FUNCTION get_relation(obj_id integer, type_id integer, name text) RETURNS SETOF relation
    LANGUAGE plpgsql
    AS $$BEGIN
RETURN QUERY SELECT * FROM topology.relation WHERE childName = get_relation.name AND parentObjectId = get_relation.obj_id AND relationType = get_relation.type_id;
END
$$;


ALTER FUNCTION topology.get_relation(obj_id integer, type_id integer, name text) OWNER TO siox;

--
-- Name: FUNCTION get_relation(obj_id integer, type_id integer, name text); Type: COMMENT; Schema: topology; Owner: siox
--

COMMENT ON FUNCTION get_relation(obj_id integer, type_id integer, name text) IS 'Returns relation.';


--
-- Name: get_statistics(); Type: FUNCTION; Schema: topology; Owner: siox
--

CREATE FUNCTION get_statistics() RETURNS SETOF relation
    LANGUAGE plpgsql
    AS $$
BEGIN
RETURN QUERY SELECT * FROM topology.relation WHERE parentobjectid=(SELECT childobjectid FROM topology.relation WHERE childname='statistics');
END
$$;


ALTER FUNCTION topology.get_statistics() OWNER TO siox;

--
-- Name: type; Type: TABLE; Schema: topology; Owner: siox; Tablespace: 
--

CREATE TABLE type (
    id integer NOT NULL,
    name text
);


ALTER TABLE topology.type OWNER TO siox;

--
-- Name: TABLE type; Type: COMMENT; Schema: topology; Owner: siox
--

COMMENT ON TABLE type IS 'Type';


--
-- Name: get_type_by_id(integer); Type: FUNCTION; Schema: topology; Owner: siox
--

CREATE FUNCTION get_type_by_id(id integer) RETURNS SETOF type
    LANGUAGE plpgsql
    AS $$BEGIN
RETURN QUERY SELECT * FROM topology.type AS t WHERE t.id = get_type_by_id.id;
END
$$;


ALTER FUNCTION topology.get_type_by_id(id integer) OWNER TO siox;

--
-- Name: FUNCTION get_type_by_id(id integer); Type: COMMENT; Schema: topology; Owner: siox
--

COMMENT ON FUNCTION get_type_by_id(id integer) IS 'Returns type given its ID.';


--
-- Name: get_type_by_name(text); Type: FUNCTION; Schema: topology; Owner: siox
--

CREATE FUNCTION get_type_by_name(name text) RETURNS SETOF type
    LANGUAGE plpgsql
    AS $$BEGIN
RETURN QUERY SELECT * FROM topology.type AS t WHERE t.name = get_type_by_name.name;
END
$$;


ALTER FUNCTION topology.get_type_by_name(name text) OWNER TO siox;

--
-- Name: FUNCTION get_type_by_name(name text); Type: COMMENT; Schema: topology; Owner: siox
--

COMMENT ON FUNCTION get_type_by_name(name text) IS 'Returns type given its name.';


--
-- Name: reset_all(); Type: FUNCTION; Schema: topology; Owner: siox
--

CREATE FUNCTION reset_all() RETURNS void
    LANGUAGE sql
    AS $$TRUNCATE topology.attribute, topology.object, topology.relation, topology.type, topology.value;
ALTER SEQUENCE topology.attribute_id_seq RESTART WITH 1;
ALTER SEQUENCE topology.object_id_seq    RESTART WITH 1;
ALTER SEQUENCE topology.type_id_seq      RESTART WITH 1;
$$;


ALTER FUNCTION topology.reset_all() OWNER TO siox;

SET search_path = activity, pg_catalog;

--
-- Name: activity_id_unique_id_seq; Type: SEQUENCE; Schema: activity; Owner: siox
--

CREATE SEQUENCE activity_id_unique_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE activity.activity_id_unique_id_seq OWNER TO siox;

--
-- Name: activity_id_unique_id_seq; Type: SEQUENCE OWNED BY; Schema: activity; Owner: siox
--

ALTER SEQUENCE activity_id_unique_id_seq OWNED BY activity_ids.unique_id;


--
-- Name: remote_call_id_unique_id_seq; Type: SEQUENCE; Schema: activity; Owner: siox
--

CREATE SEQUENCE remote_call_id_unique_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE activity.remote_call_id_unique_id_seq OWNER TO siox;

--
-- Name: remote_call_id_unique_id_seq; Type: SEQUENCE OWNED BY; Schema: activity; Owner: siox
--

ALTER SEQUENCE remote_call_id_unique_id_seq OWNED BY remote_call_ids.unique_id;


SET search_path = public, pg_catalog;

--
-- Name: commands; Type: VIEW; Schema: public; Owner: siox
--

CREATE VIEW commands AS
 SELECT DISTINCT split_part(b.value, ' '::text, 1) AS cmd
   FROM topology.relation a,
    topology.value b
  WHERE ((b.objectid = a.childobjectid) AND (a.childname = (( SELECT (relation.childobjectid)::character(100) AS childobjectid
           FROM topology.relation
          WHERE (relation.childname = 'description/commandLine'::text)))::text))
  ORDER BY split_part(b.value, ' '::text, 1);


ALTER TABLE public.commands OWNER TO siox;

--
-- Name: VIEW commands; Type: COMMENT; Schema: public; Owner: siox
--

COMMENT ON VIEW commands IS 'Table containing all command executed.';


--
-- Name: node; Type: TABLE; Schema: public; Owner: siox; Tablespace: 
--

CREATE TABLE node (
    objectid integer,
    attributeid integer,
    type smallint,
    value text
);


ALTER TABLE public.node OWNER TO siox;

--
-- Name: nodes; Type: VIEW; Schema: public; Owner: siox
--

CREATE VIEW nodes AS
 SELECT DISTINCT relation.childname
   FROM topology.relation
  WHERE (relation.relationtypeid = ( SELECT type.id
           FROM topology.type
          WHERE (type.name = 'Host'::text)))
  ORDER BY relation.childname;


ALTER TABLE public.nodes OWNER TO siox;

--
-- Name: VIEW nodes; Type: COMMENT; Schema: public; Owner: siox
--

COMMENT ON VIEW nodes IS 'Table containing all SIOX nodes.';


--
-- Name: programs; Type: VIEW; Schema: public; Owner: siox
--

CREATE VIEW programs AS
 SELECT relation.parentobjectid,
    relation.childname,
    relation.childobjectid,
    relation.relationtypeid
   FROM topology.relation
  WHERE (relation.relationtypeid = ( SELECT type.id
           FROM topology.type
          WHERE (type.name = 'ProcessID'::text)));


ALTER TABLE public.programs OWNER TO siox;

--
-- Name: program_with_attrs; Type: VIEW; Schema: public; Owner: siox
--

CREATE VIEW program_with_attrs AS
 SELECT p.parentobjectid,
    p.childname,
    p.childobjectid,
    p.relationtypeid,
    r2.childname AS attribute,
    v.value
   FROM programs p,
    topology.relation r1,
    topology.relation r2,
    topology.value v
  WHERE (((r1.parentobjectid = p.childobjectid) AND (r2.childobjectid = (regexp_replace(('0'::text || COALESCE(r1.childname, '0'::text)), '[^0-9]+'::text, ''::text, 'g'::text))::bigint)) AND (v.objectid = r1.childobjectid));


ALTER TABLE public.program_with_attrs OWNER TO siox;

--
-- Name: users; Type: VIEW; Schema: public; Owner: siox
--

CREATE VIEW users AS
 SELECT DISTINCT b.value AS "user"
   FROM topology.relation a,
    topology.value b
  WHERE ((b.objectid = a.childobjectid) AND (a.childname = (( SELECT (relation.childobjectid)::character(100) AS childobjectid
           FROM topology.relation
          WHERE (relation.childname = 'description/user-name'::text)))::text))
  ORDER BY b.value;


ALTER TABLE public.users OWNER TO siox;

--
-- Name: VIEW users; Type: COMMENT; Schema: public; Owner: siox
--

COMMENT ON VIEW users IS 'Table containing all user names.';


SET search_path = statistics, pg_catalog;

--
-- Name: stats; Type: TABLE; Schema: statistics; Owner: siox; Tablespace: 
--

CREATE TABLE stats (
    stat_id bigint NOT NULL,
    topology_id bigint,
    ontology_id bigint,
    value text,
    "timestamp" bigint
);


ALTER TABLE statistics.stats OWNER TO siox;

--
-- Name: stats_full; Type: VIEW; Schema: statistics; Owner: siox
--

CREATE VIEW stats_full AS
 SELECT s.stat_id,
    s.topology_id,
    s.ontology_id,
    s.value,
    s."timestamp",
    r.parentobjectid,
    r.childname,
    r.childobjectid,
    r.relationtypeid
   FROM stats s,
    topology.relation r
  WHERE (s.ontology_id = r.childobjectid)
  ORDER BY s."timestamp";


ALTER TABLE statistics.stats_full OWNER TO siox;

--
-- Name: stats_stat_id_seq; Type: SEQUENCE; Schema: statistics; Owner: siox
--

CREATE SEQUENCE stats_stat_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE statistics.stats_stat_id_seq OWNER TO siox;

--
-- Name: stats_stat_id_seq; Type: SEQUENCE OWNED BY; Schema: statistics; Owner: siox
--

ALTER SEQUENCE stats_stat_id_seq OWNED BY stats.stat_id;


SET search_path = topology, pg_catalog;

--
-- Name: attribute_id_seq; Type: SEQUENCE; Schema: topology; Owner: siox
--

CREATE SEQUENCE attribute_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE topology.attribute_id_seq OWNER TO siox;

--
-- Name: attribute_id_seq; Type: SEQUENCE OWNED BY; Schema: topology; Owner: siox
--

ALTER SEQUENCE attribute_id_seq OWNED BY attribute.id;


--
-- Name: commands; Type: VIEW; Schema: topology; Owner: siox
--

CREATE VIEW commands AS
 SELECT split_part(b.value, ' '::text, 1) AS cmd,
    a.childobjectid
   FROM relation a,
    value b
  WHERE ((b.objectid = a.childobjectid) AND (a.childname = (( SELECT (relation.childobjectid)::character(100) AS childobjectid
           FROM relation
          WHERE (relation.childname = 'description/commandLine'::text)))::text));


ALTER TABLE topology.commands OWNER TO siox;

--
-- Name: object_id_seq; Type: SEQUENCE; Schema: topology; Owner: siox
--

CREATE SEQUENCE object_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE topology.object_id_seq OWNER TO siox;

--
-- Name: object_id_seq; Type: SEQUENCE OWNED BY; Schema: topology; Owner: siox
--

ALTER SEQUENCE object_id_seq OWNED BY object.id;


--
-- Name: processes; Type: VIEW; Schema: topology; Owner: siox
--

CREATE VIEW processes AS
 SELECT relation.parentobjectid,
    relation.childname,
    relation.childobjectid,
    relation.relationtypeid
   FROM relation
  WHERE (relation.relationtypeid = ( SELECT type.id
           FROM type
          WHERE (type.name = 'ProcessID'::text)));


ALTER TABLE topology.processes OWNER TO siox;

--
-- Name: type_id_seq; Type: SEQUENCE; Schema: topology; Owner: siox
--

CREATE SEQUENCE type_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE topology.type_id_seq OWNER TO siox;

--
-- Name: type_id_seq; Type: SEQUENCE OWNED BY; Schema: topology; Owner: siox
--

ALTER SEQUENCE type_id_seq OWNED BY type.id;


SET search_path = activity, pg_catalog;

--
-- Name: unique_id; Type: DEFAULT; Schema: activity; Owner: siox
--

ALTER TABLE ONLY activity_ids ALTER COLUMN unique_id SET DEFAULT nextval('activity_id_unique_id_seq'::regclass);


--
-- Name: unique_id; Type: DEFAULT; Schema: activity; Owner: siox
--

ALTER TABLE ONLY remote_call_ids ALTER COLUMN unique_id SET DEFAULT nextval('remote_call_id_unique_id_seq'::regclass);


SET search_path = statistics, pg_catalog;

--
-- Name: stat_id; Type: DEFAULT; Schema: statistics; Owner: siox
--

ALTER TABLE ONLY stats ALTER COLUMN stat_id SET DEFAULT nextval('stats_stat_id_seq'::regclass);


SET search_path = topology, pg_catalog;

--
-- Name: id; Type: DEFAULT; Schema: topology; Owner: siox
--

ALTER TABLE ONLY attribute ALTER COLUMN id SET DEFAULT nextval('attribute_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: topology; Owner: siox
--

ALTER TABLE ONLY object ALTER COLUMN id SET DEFAULT nextval('object_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: topology; Owner: siox
--

ALTER TABLE ONLY type ALTER COLUMN id SET DEFAULT nextval('type_id_seq'::regclass);


SET search_path = activity, pg_catalog;

--
-- Name: activities_pkey; Type: CONSTRAINT; Schema: activity; Owner: siox; Tablespace: 
--

ALTER TABLE ONLY activities
    ADD CONSTRAINT activities_pkey PRIMARY KEY (unique_id);


--
-- Name: activity_id_pkey; Type: CONSTRAINT; Schema: activity; Owner: siox; Tablespace: 
--

ALTER TABLE ONLY activity_ids
    ADD CONSTRAINT activity_id_pkey PRIMARY KEY (unique_id);


--
-- Name: remote_call_id_pkey; Type: CONSTRAINT; Schema: activity; Owner: siox; Tablespace: 
--

ALTER TABLE ONLY remote_call_ids
    ADD CONSTRAINT remote_call_id_pkey PRIMARY KEY (unique_id);


--
-- Name: remote_calls_pkey; Type: CONSTRAINT; Schema: activity; Owner: siox; Tablespace: 
--

ALTER TABLE ONLY remote_calls
    ADD CONSTRAINT remote_calls_pkey PRIMARY KEY (unique_id);


SET search_path = statistics, pg_catalog;

--
-- Name: stats_pkey; Type: CONSTRAINT; Schema: statistics; Owner: siox; Tablespace: 
--

ALTER TABLE ONLY stats
    ADD CONSTRAINT stats_pkey PRIMARY KEY (stat_id);


SET search_path = topology, pg_catalog;

--
-- Name: attribute_name_domaintypeid_key; Type: CONSTRAINT; Schema: topology; Owner: siox; Tablespace: 
--

ALTER TABLE ONLY attribute
    ADD CONSTRAINT attribute_name_domaintypeid_key UNIQUE (name, domaintypeid);


--
-- Name: attribute_pkey; Type: CONSTRAINT; Schema: topology; Owner: siox; Tablespace: 
--

ALTER TABLE ONLY attribute
    ADD CONSTRAINT attribute_pkey PRIMARY KEY (id);


--
-- Name: object_pkey; Type: CONSTRAINT; Schema: topology; Owner: siox; Tablespace: 
--

ALTER TABLE ONLY object
    ADD CONSTRAINT object_pkey PRIMARY KEY (id);


--
-- Name: relation_childname_relationtypeid_parentobjectid_key; Type: CONSTRAINT; Schema: topology; Owner: siox; Tablespace: 
--

ALTER TABLE ONLY relation
    ADD CONSTRAINT relation_childname_relationtypeid_parentobjectid_key UNIQUE (childname, relationtypeid, parentobjectid);


--
-- Name: type_name_key; Type: CONSTRAINT; Schema: topology; Owner: siox; Tablespace: 
--

ALTER TABLE ONLY type
    ADD CONSTRAINT type_name_key UNIQUE (name);


--
-- Name: type_pkey; Type: CONSTRAINT; Schema: topology; Owner: siox; Tablespace: 
--

ALTER TABLE ONLY type
    ADD CONSTRAINT type_pkey PRIMARY KEY (id);


--
-- Name: value_objectid_attributeid_key; Type: CONSTRAINT; Schema: topology; Owner: siox; Tablespace: 
--

ALTER TABLE ONLY value
    ADD CONSTRAINT value_objectid_attributeid_key UNIQUE (objectid, attributeid);


SET search_path = activity, pg_catalog;

--
-- Name: activity_id_combi_idx; Type: INDEX; Schema: activity; Owner: siox; Tablespace: 
--

CREATE UNIQUE INDEX activity_id_combi_idx ON activity_ids USING btree (id, thread_id, cid_pid_nid, cid_pid_pid, cid_pid_time, cid_id);


--
-- Name: activity_id_component_idx; Type: INDEX; Schema: activity; Owner: siox; Tablespace: 
--

CREATE INDEX activity_id_component_idx ON activity_ids USING btree (cid_pid_nid, cid_pid_pid, cid_pid_time);


SET search_path = topology, pg_catalog;

--
-- Name: rel_parent_child_idx; Type: INDEX; Schema: topology; Owner: siox; Tablespace: 
--

CREATE INDEX rel_parent_child_idx ON relation USING btree (childobjectid, parentobjectid);


--
-- Name: public; Type: ACL; Schema: -; Owner: siox
--

REVOKE ALL ON SCHEMA public FROM PUBLIC;
REVOKE ALL ON SCHEMA public FROM siox;
GRANT ALL ON SCHEMA public TO siox;
GRANT ALL ON SCHEMA public TO PUBLIC;


--
-- PostgreSQL database dump complete
--


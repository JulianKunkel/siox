--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

--
-- Activity
--

CREATE SCHEMA activity;
COMMENT ON SCHEMA activity IS 'Activities and Remote Calls';

SET search_path = activity, pg_catalog;

CREATE TABLE activities (
    unique_id bigint NOT NULL,
    ucaid integer NOT NULL,
    time_start bigint NOT NULL,
    time_stop bigint,
    error_value integer,
    remote_calls bigint[],
    attributes character varying(1024)
);

COMMENT ON TABLE activities IS 'Activities';
COMMENT ON COLUMN activities.remote_calls IS 'Elements are of type activities.remote_call_id.unique_id';

CREATE SEQUENCE activities_unique_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE activities_unique_id_seq OWNED BY activities.unique_id;
ALTER TABLE ONLY activities ALTER COLUMN unique_id SET DEFAULT nextval('activities_unique_id_seq'::regclass);
ALTER TABLE ONLY activities ADD CONSTRAINT activities_pkey PRIMARY KEY (unique_id);


CREATE TABLE activity_ids (
    unique_id bigint NOT NULL,
    id integer NOT NULL,
    thread_id integer NOT NULL,
    cid_pid_nid integer NOT NULL,
    cid_pid_pid integer NOT NULL,
    cid_pid_time integer NOT NULL,
    cid_id integer NOT NULL
);

COMMENT ON TABLE activity_ids IS 'ActivityIDs';

CREATE SEQUENCE activity_id_unique_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE activity_id_unique_id_seq OWNED BY activity_ids.unique_id;
ALTER TABLE ONLY activity_ids ALTER COLUMN unique_id SET DEFAULT nextval('activity_id_unique_id_seq'::regclass);
ALTER TABLE ONLY activity_ids ADD CONSTRAINT activity_id_pkey PRIMARY KEY (unique_id);

CREATE UNIQUE INDEX activity_id_combi_idx ON activity_ids USING btree (id, thread_id, cid_pid_nid, cid_pid_pid, cid_pid_time, cid_id);


CREATE TABLE parents (
    child_id bigint NOT NULL,
    parent_id bigint NOT NULL
);

COMMENT ON TABLE parents IS 'Parent activities.';
COMMENT ON COLUMN parents.child_id IS 'Foreign key to activity_id.unique_id';
COMMENT ON COLUMN parents.parent_id IS 'Foreign key to activity_id.unique_id';


CREATE TABLE remote_call_ids (
    unique_id bigint NOT NULL,
    nid integer,
    uuid integer,
    instance integer,
    activity_uid bigint
);

COMMENT ON TABLE remote_call_ids IS 'RemoteCallIdentifiers';

CREATE SEQUENCE remote_call_id_unique_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE remote_call_id_unique_id_seq OWNED BY remote_call_ids.unique_id;
ALTER TABLE ONLY remote_call_ids ALTER COLUMN unique_id SET DEFAULT nextval('remote_call_id_unique_id_seq'::regclass);
ALTER TABLE ONLY remote_call_ids ADD CONSTRAINT remote_call_id_pkey PRIMARY KEY (unique_id);


CREATE TABLE remote_calls (
    unique_id bigint NOT NULL,
    attributes character varying(255)
);

COMMENT ON TABLE remote_calls IS 'RemoteCalls';
ALTER TABLE ONLY remote_calls ADD CONSTRAINT remote_calls_pkey PRIMARY KEY (unique_id);

--
-- Some useful views.
--

CREATE OR REPLACE VIEW activity.activity    AS SELECT * FROM activity.activities    RIGHT OUTER JOIN activity.activity_ids    USING (unique_id);
CREATE OR REPLACE VIEW activity.remote_call AS SELECT * FROM activity.remote_calls  RIGHT OUTER JOIN activity.remote_call_ids USING (unique_id);
CREATE OR REPLACE VIEW activity.parent      AS SELECT * FROM activity.activity AS a RIGHT OUTER JOIN activity.parents AS p    ON(a.unique_id = p.child_id);
CREATE OR REPLACE VIEW activity.child       AS SELECT * FROM activity.activity AS a RIGHT OUTER JOIN activity.parents AS p    ON(a.unique_id = p.parent_id);

--
-- API calls for the Activity schema.
--

-- 
-- This functions should be called as "select * from get_activity_by_uid(x)".
--

CREATE OR REPLACE FUNCTION activity.get_activity_by_uid(uid IN bigint) 
RETURNS activity.activity AS $$ 
DECLARE act activity.activity%ROWTYPE;
BEGIN
	SELECT * INTO act 
	FROM activity.activity AS a
	WHERE a.unique_id = get_activity_by_uid.uid LIMIT 1;
	RETURN act;
END
$$ LANGUAGE plpgsql;
COMMENT ON FUNCTION activity.get_activity_by_uid(uid IN bigint) IS 'Returns activity given its Unique-ID.';


CREATE OR REPLACE FUNCTION activity.get_activity_by_aid(id IN integer, thread_id IN integer, cpn IN integer, cpp IN integer, cpt IN integer, cid IN integer) 
RETURNS activity.activity AS $$
DECLARE act activity.activity%ROWTYPE;
BEGIN
	SELECT * INTO act 
	FROM activity.activity AS a
	WHERE a.id = get_activity_by_aid.id AND a.thread_id = get_activity_by_aid.thread_id AND a.cid_pid_nid = get_activity_by_aid.cpn AND a.cid_pid_pid = get_activity_by_aid.cpp AND a.cid_pid_time = get_activity_by_aid.cpt AND a.cid_id = get_activity_by_aid.cid LIMIT 1;
	RETURN act;
END
$$ LANGUAGE plpgsql;
COMMENT ON FUNCTION activity.get_activity_by_aid(id IN integer, thread_id IN integer, cpn IN integer, cpp IN integer, cpt IN integer, cid IN integer) IS 'Returns activity given its ActivityID.';


CREATE OR REPLACE FUNCTION activity.get_activity_uid(id IN integer, thread_id IN integer, cpn IN integer, cpp IN integer, cpt IN integer, cid IN integer) 
RETURNS activity.activity.unique_id%TYPE AS $$
DECLARE uid activity.activity.unique_id%TYPE;
BEGIN
	SELECT unique_id INTO uid 
	FROM activity.activity AS a
	WHERE a.id = get_activity_uid.id AND a.thread_id = get_activity_uid.thread_id AND a.cid_pid_nid = get_activity_uid.cpn AND a.cid_pid_pid = get_activity_uid.cpp AND a.cid_pid_time = get_activity_uid.cpt AND a.cid_id = get_activity_uid.cid LIMIT 1;
	RETURN uid;
END
$$ LANGUAGE plpgsql;
COMMENT ON FUNCTION activity.get_activity_uid(id IN integer, thread_id IN integer, cpn IN integer, cpp IN integer, cpt IN integer, cid IN integer) IS 'Returns Unique-ID given its ActivityID.';


CREATE OR REPLACE FUNCTION activity.get_activity_parents(activity_unique_id IN bigint)
RETURNS SETOF activity.parent AS $$
BEGIN
	RETURN QUERY SELECT * FROM activity.parent WHERE activity.parent.child_id = get_activity_parents.activity_unique_id;
END
$$ LANGUAGE plpgsql;
COMMENT ON FUNCTION activity.get_activity_parents(activity_unique_id IN bigint) IS 'Returns the parents for an activity given the activity unique ID.';


CREATE OR REPLACE FUNCTION activity.get_activity_children(activity_unique_id IN bigint)
RETURNS SETOF activity.child AS $$
BEGIN
	RETURN QUERY SELECT * FROM activity.child WHERE activity.child.parent_id = get_activity_children.activity_unique_id;
END
$$ LANGUAGE plpgsql;
COMMENT ON FUNCTION activity.get_activity_children(activity_unique_id IN bigint) IS 'Returns the children for an activity given the activity unique ID.';


CREATE OR REPLACE FUNCTION activity.get_remote_call_uid(nid IN integer, uuid IN integer, inst IN integer, act_uid IN bigint) 
RETURNS activity.remote_calls.unique_id%TYPE AS $$
DECLARE uid activity.remote_calls.unique_id%TYPE;
BEGIN
	SELECT unique_id INTO uid 
	FROM activity.remote_calls AS r
	WHERE r.nid = get_remote_call_uid.nid AND r.uuid = get_remote_call_uid.uuid AND r.instance = get_remote_call_uid.instance AND r.activity_uid = get_remote_call_uid.act_uid LIMIT 1;
	RETURN uid;
END
$$ LANGUAGE plpgsql;
COMMENT ON FUNCTION activity.get_remote_call_uid(nid IN integer, uuid IN integer, inst IN integer, act_uid IN bigint)  IS 'Returns Unique-ID given its RemoteCallID.';


CREATE OR REPLACE FUNCTION activity.get_remote_call_by_uid(uid IN bigint) 
RETURNS activity.remote_call AS $$ 
DECLARE rc activity.remote_call%ROWTYPE;
BEGIN
	SELECT * INTO rc
	FROM activity.remote_call AS r
	WHERE r.unique_id = get_remote_call_by_uid.uid LIMIT 1;
	RETURN rc;
END
$$ LANGUAGE plpgsql;
COMMENT ON FUNCTION activity.get_remote_call_by_uid(uid IN bigint)  IS 'Returns remote call given its Unique-ID.';


CREATE OR REPLACE FUNCTION activity.get_remote_call_by_rcid(nid IN integer, uuid IN integer, inst IN integer, act_uid IN bigint) 
RETURNS activity.remote_call AS $$
DECLARE rc activity.remote_call%ROWTYPE;
BEGIN
	SELECT * INTO rc 
	FROM activity.remote_call AS r
	WHERE r.nid = get_remote_call_by_rcid.nid AND r.uuid = get_remote_call_by_rcid.uuid AND r.instance = get_remote_call_by_rcid.inst AND r.activity_uid = get_remote_call_by_rcid.act_uid LIMIT 1;
	RETURN rc;
END
$$ LANGUAGE plpgsql;
COMMENT ON FUNCTION activity.get_remote_call_by_rcid(nid IN integer, uuid IN integer, inst IN integer, act_uid IN bigint)  IS 'Returns remote call given its RemoteCallID.';


CREATE OR REPLACE FUNCTION activity.get_remote_call_by_activity_uid(unique_id IN bigint) 
RETURNS SETOF activity.remote_call AS $$
BEGIN
	RETURN QUERY SELECT * FROM activity.remote_call AS r WHERE r.activity_uid = get_remote_call_by_activity_uid.unique_id;
END
$$ LANGUAGE plpgsql;
COMMENT ON FUNCTION activity.get_remote_call_by_activity_uid(unique_id IN bigint)  IS 'Returns remote calls given its activity unique ID.';


CREATE OR REPLACE FUNCTION activity.reset_all() 
RETURNS void AS $$
	TRUNCATE activity.activities, activity.activity_ids, activity.parents, activity.remote_calls, activity.remote_call_ids;
	ALTER SEQUENCE activity.activities_unique_id_seq RESTART WITH 1;
	ALTER SEQUENCE activity.activity_id_unique_id_seq RESTART WITH 1;
	ALTER SEQUENCE activity.remote_call_id_unique_id_seq RESTART WITH 1;
$$ LANGUAGE sql;
COMMENT ON FUNCTION activity.reset_all() IS 'Truncates all tables in the schema activities and resets auto-incremental keys to 1.';


--
-- TOPOLOGY
--

CREATE SCHEMA topology;

SET search_path = topology, pg_catalog;
SET default_tablespace = '';
SET default_with_oids = false;


CREATE TABLE attribute (
    id integer NOT NULL,
    name text,
    domaintypeid integer,
    datatype integer
);

CREATE SEQUENCE attribute_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

COMMENT ON TABLE attribute IS 'Attribute';
ALTER SEQUENCE attribute_id_seq OWNED BY attribute.id;
ALTER TABLE ONLY attribute ALTER COLUMN id SET DEFAULT nextval('attribute_id_seq'::regclass);
ALTER TABLE ONLY attribute ADD CONSTRAINT attribute_name_domaintypeid_key UNIQUE (name, domaintypeid);
ALTER TABLE ONLY attribute ADD CONSTRAINT attribute_pkey PRIMARY KEY (id);


CREATE TABLE object (
    id integer NOT NULL,
    typeid integer
);

CREATE SEQUENCE object_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

COMMENT ON TABLE object IS 'Object';
ALTER SEQUENCE object_id_seq OWNED BY object.id;
ALTER TABLE ONLY object ALTER COLUMN id SET DEFAULT nextval('object_id_seq'::regclass);
ALTER TABLE ONLY object ADD CONSTRAINT object_pkey PRIMARY KEY (id);


CREATE TABLE relation (
    parentobjectid integer,
    childname text,
    childobjectid integer,
    relationtypeid integer
);

COMMENT ON TABLE relation IS 'Relation';
ALTER TABLE ONLY relation ADD CONSTRAINT relation_childname_relationtypeid_parentobjectid_key UNIQUE (childname, relationtypeid, parentobjectid);


CREATE TABLE type (
    id integer NOT NULL,
    name text
);

CREATE SEQUENCE type_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

COMMENT ON TABLE type IS 'Type';
ALTER SEQUENCE type_id_seq OWNED BY type.id;
ALTER TABLE ONLY type ALTER COLUMN id SET DEFAULT nextval('type_id_seq'::regclass);
ALTER TABLE ONLY type ADD CONSTRAINT type_name_key UNIQUE (name);
ALTER TABLE ONLY type ADD CONSTRAINT type_pkey PRIMARY KEY (id);


CREATE TABLE value (
    objectid integer,
    attributeid integer,
    type smallint,
    value text
);

COMMENT ON TABLE value IS 'Value';
ALTER TABLE ONLY value ADD CONSTRAINT value_objectid_attributeid_key UNIQUE (objectid, attributeid);

CREATE OR REPLACE FUNCTION topology.get_type_by_name(name IN text)
RETURNS SETOF topology.type AS $$
BEGIN
	RETURN QUERY SELECT * FROM topology.type AS t WHERE t.name = get_type_by_name.name;
END
$$ LANGUAGE plpgsql;
COMMENT ON FUNCTION topology.get_type_by_name(name IN text)  IS 'Returns type given its name.';


CREATE OR REPLACE FUNCTION topology.get_type_by_id(id IN integer)
RETURNS SETOF topology.type AS $$
BEGIN
	RETURN QUERY SELECT * FROM topology.type AS t WHERE t.id = get_type_by_id.id;
END
$$ LANGUAGE plpgsql;
COMMENT ON FUNCTION topology.get_type_by_id(id IN integer)  IS 'Returns type given its ID.';


CREATE OR REPLACE FUNCTION topology.get_object_by_id(id IN integer)
RETURNS SETOF topology.object AS $$
BEGIN
	RETURN QUERY SELECT * FROM topology.object AS o WHERE o.id = get_object_by_id.id;
END
$$ LANGUAGE plpgsql;
COMMENT ON FUNCTION topology.get_object_by_id(id IN integer)  IS 'Returns object given its ID.';


CREATE OR REPLACE FUNCTION topology.get_relation(obj_id IN integer, type_id IN integer, name IN text)
RETURNS SETOF topology.relation AS $$
BEGIN
	RETURN QUERY SELECT * FROM topology.relation WHERE childName = get_relation.name AND parentObjectId = get_relation.obj_id AND relationType = get_relation.type_id;
END
$$ LANGUAGE plpgsql;
COMMENT ON FUNCTION topology.get_relation(obj_id IN integer, type_id IN integer, name IN text) IS 'Returns relation.';

CREATE OR REPLACE FUNCTION topology.get_attribute_by_name(type_id IN integer, name IN text)
RETURNS SETOF topology.attribute AS $$
BEGIN
	RETURN QUERY SELECT * FROM topology.attribute WHERE name = get_attribute_by_name.name AND domainTypeId = get_attribute_by_name.type_id;
END
$$ LANGUAGE plpgsql;
COMMENT ON FUNCTION topology.get_attribute_by_name(type_id IN integer, name IN text) IS 'Returns attribute given name and domain ID.';


CREATE OR REPLACE FUNCTION topology.get_attribute_by_id(id IN integer)
RETURNS SETOF topology.attribute AS $$
BEGIN
	RETURN QUERY SELECT * FROM topology.attribute WHERE id = get_attribute_by_id.id;
END
$$ LANGUAGE plpgsql;
COMMENT ON FUNCTION topology.get_attribute_by_id(id IN integer) IS 'Returns attribute given its numeric ID.';


CREATE OR REPLACE FUNCTION topology.get_attribute_by_object(obj_id IN integer, attr_id IN integer)
RETURNS SETOF topology.attribute AS $$
BEGIN
	RETURN QUERY SELECT * FROM topology.attribute WHERE objectId = get_attribute_by_object.obj_id AND attributeId = get_attribute_by_object.attr_id;
END
$$ LANGUAGE plpgsql;
COMMENT ON FUNCTION topology.get_attribute_by_object(obj_id IN integer, attr_id IN integer) IS 'Returns attribute given an object id and attribute-id.';


CREATE OR REPLACE FUNCTION topology.reset_all() 
RETURNS void AS $$
	TRUNCATE topology.attribute, topology.object, topology.relation, topology.type, topology.value;
	ALTER SEQUENCE topology.attribute_id_seq RESTART WITH 1;
	ALTER SEQUENCE topology.object_id_seq    RESTART WITH 1;
	ALTER SEQUENCE topology.type_id_seq      RESTART WITH 1;
$$ LANGUAGE sql;

--
-- SYSINFO
--

CREATE SCHEMA sysinfo;
COMMENT ON SCHEMA sysinfo IS 'This schema and its contents are probably deprecated. ';

SET search_path = sysinfo, pg_catalog;
SET default_tablespace = '';
SET default_with_oids = false;

CREATE TABLE activities (
    unique_id bigint NOT NULL,
    interface_id bigint NOT NULL,
    activity_name character varying(255) NOT NULL,
    ucaid bigint NOT NULL
);

CREATE SEQUENCE activities_unique_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE activities_unique_id_seq OWNED BY activities.unique_id;
ALTER TABLE ONLY activities ALTER COLUMN unique_id SET DEFAULT nextval('activities_unique_id_seq'::regclass);
ALTER TABLE ONLY activities ADD CONSTRAINT activities_pkey PRIMARY KEY (unique_id);

CREATE TABLE file_systems (
    unique_id bigint NOT NULL,
    name character varying(255) NOT NULL
);

CREATE SEQUENCE file_system_unique_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE file_system_unique_id_seq OWNED BY file_systems.unique_id;
ALTER TABLE ONLY file_systems ALTER COLUMN unique_id SET DEFAULT nextval('file_system_unique_id_seq'::regclass);
ALTER TABLE ONLY file_systems ADD CONSTRAINT file_system_pkey PRIMARY KEY (unique_id);

CREATE TABLE interfaces (
    unique_id bigint NOT NULL,
    name character varying(256) NOT NULL,
    implementation character varying(255) NOT NULL
);

CREATE SEQUENCE interfaces_unique_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE interfaces_unique_id_seq OWNED BY interfaces.unique_id;
ALTER TABLE ONLY interfaces ALTER COLUMN unique_id SET DEFAULT nextval('interfaces_unique_id_seq'::regclass);
ALTER TABLE ONLY interfaces ADD CONSTRAINT interfaces_pkey PRIMARY KEY (unique_id);

CREATE TABLE nodes (
    unique_id bigint NOT NULL,
    hostname character varying(255) NOT NULL
);

CREATE SEQUENCE nodes_unique_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE nodes_unique_id_seq OWNED BY nodes.unique_id;
ALTER TABLE ONLY nodes ALTER COLUMN unique_id SET DEFAULT nextval('nodes_unique_id_seq'::regclass);
ALTER TABLE ONLY nodes ADD CONSTRAINT nodes_pkey PRIMARY KEY (unique_id);

CREATE TABLE storage_devices (
    unique_id bigint NOT NULL,
    device_id bigint NOT NULL,
    node_id bigint NOT NULL,
    model_name character varying(255),
    local_address character varying(255)
);

CREATE SEQUENCE storage_device_unique_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE storage_device_unique_id_seq OWNED BY storage_devices.unique_id;
ALTER TABLE ONLY storage_devices ALTER COLUMN unique_id SET DEFAULT nextval('storage_device_unique_id_seq'::regclass);
ALTER TABLE ONLY storage_devices ADD CONSTRAINT storage_device_pkey PRIMARY KEY (unique_id);

CREATE OR REPLACE FUNCTION reset_all() 
RETURNS void AS $$
	TRUNCATE sysinfo.activities, sysinfo.file_systems, sysinfo.interfaces, sysinfo.nodes, sysinfo.storage_devices;
	ALTER SEQUENCE sysinfo.attribute_id_seq RESTART WITH 1;
	ALTER SEQUENCE sysinfo.object_id_seq RESTART WITH 1;
	ALTER SEQUENCE sysinfo.type_id_seq RESTART WITH 1;
$$ LANGUAGE sql;

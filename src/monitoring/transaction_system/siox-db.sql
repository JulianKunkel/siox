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
ALTER TABLE ONLY activities
    ADD CONSTRAINT activities_pkey PRIMARY KEY (unique_id);


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
ALTER TABLE ONLY activity_ids
    ADD CONSTRAINT activity_id_pkey PRIMARY KEY (unique_id);

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
ALTER TABLE ONLY remote_call_ids
    ADD CONSTRAINT remote_call_id_pkey PRIMARY KEY (unique_id);

CREATE TABLE remote_calls (
    unique_id bigint NOT NULL,
    attributes character varying(255)
);


COMMENT ON TABLE remote_calls IS 'RemoteCalls';
ALTER TABLE ONLY remote_calls
    ADD CONSTRAINT remote_calls_pkey PRIMARY KEY (unique_id);


CREATE FUNCTION reset_all() RETURNS void
    LANGUAGE sql
    AS $$truncate activity.activities, activity.activity_ids, activity.parents, activity.remote_calls, activity.remote_call_ids;
alter sequence activity.activities_unique_id_seq restart with 1;
alter sequence activity.activity_id_unique_id_seq restart with 1;
alter sequence activity.remote_call_id_unique_id_seq restart with 1;
$$;


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
ALTER TABLE ONLY attribute
    ADD CONSTRAINT attribute_name_domaintypeid_key UNIQUE (name, domaintypeid);
ALTER TABLE ONLY attribute
    ADD CONSTRAINT attribute_pkey PRIMARY KEY (id);


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
ALTER TABLE ONLY object
    ADD CONSTRAINT object_pkey PRIMARY KEY (id);


CREATE TABLE relation (
    parentobjectid integer,
    childname text,
    childobjectid integer,
    relationtypeid integer
);

COMMENT ON TABLE relation IS 'Relation';
ALTER TABLE ONLY relation
    ADD CONSTRAINT relation_childname_relationtypeid_parentobjectid_key UNIQUE (childname, relationtypeid, parentobjectid);


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
ALTER TABLE ONLY type
    ADD CONSTRAINT type_name_key UNIQUE (name);
ALTER TABLE ONLY type
    ADD CONSTRAINT type_pkey PRIMARY KEY (id);


CREATE TABLE value (
    objectid integer,
    attributeid integer,
    type smallint,
    value text
);

COMMENT ON TABLE value IS 'Value';
ALTER TABLE ONLY value
    ADD CONSTRAINT value_objectid_attributeid_key UNIQUE (objectid, attributeid);

CREATE FUNCTION reset_all() RETURNS void
    LANGUAGE sql
    AS $$truncate topology.attribute, topology.object, topology.relation, topology.type, topology.value;
alter sequence topology.attribute_id_seq restart with 1;
alter sequence topology.object_id_seq restart with 1;
alter sequence topology.type_id_seq restart with 1;
$$;

--
-- SYSINFO
--

CREATE SCHEMA sysinfo;
SET search_path = sysinfo, pg_catalog;
SET default_tablespace = '';
SET default_with_oids = false;
COMMENT ON SCHEMA sysinfo IS 'This schema and its contents are probably deprecated. ';

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
ALTER TABLE ONLY activities
    ADD CONSTRAINT activities_pkey PRIMARY KEY (unique_id);

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
ALTER TABLE ONLY file_systems
    ADD CONSTRAINT file_system_pkey PRIMARY KEY (unique_id);

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
ALTER TABLE ONLY interfaces
    ADD CONSTRAINT interfaces_pkey PRIMARY KEY (unique_id);

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
ALTER TABLE ONLY nodes
    ADD CONSTRAINT nodes_pkey PRIMARY KEY (unique_id);

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
ALTER TABLE ONLY storage_devices
    ADD CONSTRAINT storage_device_pkey PRIMARY KEY (unique_id);

CREATE FUNCTION reset_all() RETURNS void
    LANGUAGE sql
    AS $$truncate sysinfo.activities, sysinfo.file_systems, sysinfo.interfaces, sysinfo.nodes, sysinfo.storage_devices;
alter sequence sysinfo.attribute_id_seq restart with 1;
alter sequence sysinfo.object_id_seq restart with 1;
alter sequence sysinfo.type_id_seq restart with 1;
$$;

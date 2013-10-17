--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

--
-- Name: activity; Type: SCHEMA; Schema: -; Owner: postgres
--

CREATE SCHEMA activity;


ALTER SCHEMA activity OWNER TO postgres;

--
-- Name: SCHEMA activity; Type: COMMENT; Schema: -; Owner: postgres
--

COMMENT ON SCHEMA activity IS 'Activities and Remote Calls';


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
-- Name: process_id; Type: TYPE; Schema: public; Owner: postgres
--

CREATE TYPE process_id AS (
	nid bigint,
	pid bigint,
	"time" bigint
);


ALTER TYPE public.process_id OWNER TO postgres;

--
-- Name: component_id; Type: TYPE; Schema: public; Owner: postgres
--

CREATE TYPE component_id AS (
	pid process_id,
	num integer
);


ALTER TYPE public.component_id OWNER TO postgres;

--
-- Name: activity_id; Type: TYPE; Schema: public; Owner: postgres
--

CREATE TYPE activity_id AS (
	id bigint,
	cid component_id
);


ALTER TYPE public.activity_id OWNER TO postgres;

--
-- Name: attribute; Type: TYPE; Schema: public; Owner: postgres
--

CREATE TYPE attribute AS (
	key bigint,
	value character varying(255)
);


ALTER TYPE public.attribute OWNER TO postgres;

SET search_path = activity, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: activities; Type: TABLE; Schema: activity; Owner: postgres; Tablespace: 
--

CREATE TABLE activities (
    unique_id bigint NOT NULL,
    ucaid integer NOT NULL,
    time_start bigint NOT NULL,
    time_stop bigint,
    error_value integer,
    remote_calls bigint[],
    attributes character varying(255)
);


ALTER TABLE activity.activities OWNER TO postgres;

--
-- Name: COLUMN activities.remote_calls; Type: COMMENT; Schema: activity; Owner: postgres
--

COMMENT ON COLUMN activities.remote_calls IS 'Elements are of type activities.remote_call_id.unique_id';


--
-- Name: activities_unique_id_seq; Type: SEQUENCE; Schema: activity; Owner: postgres
--

CREATE SEQUENCE activities_unique_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE activity.activities_unique_id_seq OWNER TO postgres;

--
-- Name: activities_unique_id_seq; Type: SEQUENCE OWNED BY; Schema: activity; Owner: postgres
--

ALTER SEQUENCE activities_unique_id_seq OWNED BY activities.unique_id;


--
-- Name: activity_ids; Type: TABLE; Schema: activity; Owner: postgres; Tablespace: 
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


ALTER TABLE activity.activity_ids OWNER TO postgres;

--
-- Name: TABLE activity_ids; Type: COMMENT; Schema: activity; Owner: postgres
--

COMMENT ON TABLE activity_ids IS 'ActivityIDs';


--
-- Name: activity_id_unique_id_seq; Type: SEQUENCE; Schema: activity; Owner: postgres
--

CREATE SEQUENCE activity_id_unique_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE activity.activity_id_unique_id_seq OWNER TO postgres;

--
-- Name: activity_id_unique_id_seq; Type: SEQUENCE OWNED BY; Schema: activity; Owner: postgres
--

ALTER SEQUENCE activity_id_unique_id_seq OWNED BY activity_ids.unique_id;


--
-- Name: parents; Type: TABLE; Schema: activity; Owner: postgres; Tablespace: 
--

CREATE TABLE parents (
    child_id bigint NOT NULL,
    parent_id bigint NOT NULL
);


ALTER TABLE activity.parents OWNER TO postgres;

--
-- Name: TABLE parents; Type: COMMENT; Schema: activity; Owner: postgres
--

COMMENT ON TABLE parents IS 'Parent activities.';


--
-- Name: COLUMN parents.child_id; Type: COMMENT; Schema: activity; Owner: postgres
--

COMMENT ON COLUMN parents.child_id IS 'Foreign key to activity_id.unique_id';


--
-- Name: COLUMN parents.parent_id; Type: COMMENT; Schema: activity; Owner: postgres
--

COMMENT ON COLUMN parents.parent_id IS 'Foreign key to activity_id.unique_id';


--
-- Name: remote_call_ids; Type: TABLE; Schema: activity; Owner: postgres; Tablespace: 
--

CREATE TABLE remote_call_ids (
    unique_id bigint NOT NULL,
    nid integer,
    uuid integer,
    instance integer,
    activity_uid bigint
);


ALTER TABLE activity.remote_call_ids OWNER TO postgres;

--
-- Name: TABLE remote_call_ids; Type: COMMENT; Schema: activity; Owner: postgres
--

COMMENT ON TABLE remote_call_ids IS 'RemoteCallIdentifiers';


--
-- Name: remote_calls; Type: TABLE; Schema: activity; Owner: postgres; Tablespace: 
--

CREATE TABLE remote_calls (
    unique_id bigint NOT NULL,
    attributes character varying(255)
);


ALTER TABLE activity.remote_calls OWNER TO postgres;

--
-- Name: TABLE remote_calls; Type: COMMENT; Schema: activity; Owner: postgres
--

COMMENT ON TABLE remote_calls IS 'RemoteCalls';


--
-- Name: remte_call_id_unique_id_seq; Type: SEQUENCE; Schema: activity; Owner: postgres
--

CREATE SEQUENCE remte_call_id_unique_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE activity.remte_call_id_unique_id_seq OWNER TO postgres;

--
-- Name: remte_call_id_unique_id_seq; Type: SEQUENCE OWNED BY; Schema: activity; Owner: postgres
--

ALTER SEQUENCE remte_call_id_unique_id_seq OWNED BY remote_call_ids.unique_id;


--
-- Name: unique_id; Type: DEFAULT; Schema: activity; Owner: postgres
--

ALTER TABLE ONLY activities ALTER COLUMN unique_id SET DEFAULT nextval('activities_unique_id_seq'::regclass);


--
-- Name: unique_id; Type: DEFAULT; Schema: activity; Owner: postgres
--

ALTER TABLE ONLY activity_ids ALTER COLUMN unique_id SET DEFAULT nextval('activity_id_unique_id_seq'::regclass);


--
-- Name: unique_id; Type: DEFAULT; Schema: activity; Owner: postgres
--

ALTER TABLE ONLY remote_call_ids ALTER COLUMN unique_id SET DEFAULT nextval('remte_call_id_unique_id_seq'::regclass);


--
-- Name: activities_pkey; Type: CONSTRAINT; Schema: activity; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY activities
    ADD CONSTRAINT activities_pkey PRIMARY KEY (unique_id);


--
-- Name: activity_id_pkey; Type: CONSTRAINT; Schema: activity; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY activity_ids
    ADD CONSTRAINT activity_id_pkey PRIMARY KEY (unique_id);


--
-- Name: remote_calls_pkey; Type: CONSTRAINT; Schema: activity; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY remote_calls
    ADD CONSTRAINT remote_calls_pkey PRIMARY KEY (unique_id);


--
-- Name: remte_call_id_pkey; Type: CONSTRAINT; Schema: activity; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY remote_call_ids
    ADD CONSTRAINT remte_call_id_pkey PRIMARY KEY (unique_id);


--
-- Name: public; Type: ACL; Schema: -; Owner: postgres
--

REVOKE ALL ON SCHEMA public FROM PUBLIC;
REVOKE ALL ON SCHEMA public FROM postgres;
GRANT ALL ON SCHEMA public TO postgres;
GRANT ALL ON SCHEMA public TO PUBLIC;


--
-- PostgreSQL database dump complete
--


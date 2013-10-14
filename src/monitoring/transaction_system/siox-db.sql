--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

SET search_path = public, pg_catalog;

--
-- Name: attribute; Type: TYPE; Schema: public; Owner: postgres
--

CREATE TYPE attribute AS (
	key bigint,
	value character varying(255)
);


ALTER TYPE public.attribute OWNER TO postgres;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: activity; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE activity (
    unique_id bigint NOT NULL,
    aid_id bigint NOT NULL,
    aid_cid_nid bigint NOT NULL,
    aid_cid_pid bigint NOT NULL,
    aid_cid_time bigint NOT NULL,
    aid_cid_num integer NOT NULL,
    ucaid bigint NOT NULL,
    time_start bigint NOT NULL,
    time_stop bigint,
    error_value bigint,
    attributes attribute[]
);


ALTER TABLE public.activity OWNER TO postgres;

--
-- Name: activity_unique_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE activity_unique_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.activity_unique_id_seq OWNER TO postgres;

--
-- Name: activity_unique_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE activity_unique_id_seq OWNED BY activity.unique_id;


--
-- Name: hardware; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE hardware (
    id bigint NOT NULL,
    description character varying(255) NOT NULL
);


ALTER TABLE public.hardware OWNER TO postgres;

--
-- Name: hardware_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE hardware_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.hardware_id_seq OWNER TO postgres;

--
-- Name: hardware_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE hardware_id_seq OWNED BY hardware.id;


--
-- Name: instance; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE instance (
    id bigint NOT NULL,
    description character varying(255) NOT NULL
);


ALTER TABLE public.instance OWNER TO postgres;

--
-- Name: instance_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE instance_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.instance_id_seq OWNER TO postgres;

--
-- Name: instance_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE instance_id_seq OWNED BY instance.id;


--
-- Name: node; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE node (
    id bigint NOT NULL,
    hwid bigint NOT NULL,
    swid bigint,
    iid bigint
);


ALTER TABLE public.node OWNER TO postgres;

--
-- Name: node_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE node_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.node_id_seq OWNER TO postgres;

--
-- Name: node_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE node_id_seq OWNED BY node.id;


--
-- Name: ontology; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE ontology (
    id bigint NOT NULL,
    domain character varying(255),
    name character varying(255),
    type smallint
);


ALTER TABLE public.ontology OWNER TO postgres;

--
-- Name: parent; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE parent (
    parent_aid_id bigint NOT NULL,
    parent_cid_nid bigint NOT NULL,
    parent_cid_pid bigint NOT NULL,
    parent_cid_time bigint NOT NULL,
    parent_cid_num integer NOT NULL,
    child_unique_id bigint NOT NULL
);


ALTER TABLE public.parent OWNER TO postgres;

--
-- Name: software; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE software (
    id bigint NOT NULL,
    description character varying(255) NOT NULL
);


ALTER TABLE public.software OWNER TO postgres;

--
-- Name: software_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE software_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.software_id_seq OWNER TO postgres;

--
-- Name: software_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE software_id_seq OWNED BY software.id;


--
-- Name: unique_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY activity ALTER COLUMN unique_id SET DEFAULT nextval('activity_unique_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY hardware ALTER COLUMN id SET DEFAULT nextval('hardware_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY instance ALTER COLUMN id SET DEFAULT nextval('instance_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY node ALTER COLUMN id SET DEFAULT nextval('node_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY software ALTER COLUMN id SET DEFAULT nextval('software_id_seq'::regclass);


--
-- Name: activity_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY activity
    ADD CONSTRAINT activity_pkey PRIMARY KEY (unique_id);


--
-- Name: hardware_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY hardware
    ADD CONSTRAINT hardware_pkey PRIMARY KEY (id);


--
-- Name: instance_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY instance
    ADD CONSTRAINT instance_pkey PRIMARY KEY (id);


--
-- Name: node_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY node
    ADD CONSTRAINT node_pkey PRIMARY KEY (id);


--
-- Name: parent_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY parent
    ADD CONSTRAINT parent_pkey PRIMARY KEY (parent_aid_id, parent_cid_nid, parent_cid_pid, parent_cid_time, parent_cid_num, child_unique_id);


--
-- Name: software_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY software
    ADD CONSTRAINT software_pkey PRIMARY KEY (id);


--
-- Name: activity_aid_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE UNIQUE INDEX activity_aid_idx ON activity USING btree (aid_id, aid_cid_nid, aid_cid_pid, aid_cid_time, aid_cid_num);


--
-- Name: node_id_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE UNIQUE INDEX node_id_idx ON ontology USING btree (id);


--
-- Name: hardware_id_idx; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY node
    ADD CONSTRAINT hardware_id_idx FOREIGN KEY (hwid) REFERENCES hardware(id);


--
-- Name: instance_id_idx; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY node
    ADD CONSTRAINT instance_id_idx FOREIGN KEY (iid) REFERENCES instance(id);


--
-- Name: software_id_idx; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY node
    ADD CONSTRAINT software_id_idx FOREIGN KEY (swid) REFERENCES software(id);


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


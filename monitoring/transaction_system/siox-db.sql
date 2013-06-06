--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

--
-- Name: plpgsql; Type: EXTENSION; Schema: -; Owner: 
--

CREATE EXTENSION IF NOT EXISTS plpgsql WITH SCHEMA pg_catalog;


--
-- Name: EXTENSION plpgsql; Type: COMMENT; Schema: -; Owner: 
--

COMMENT ON EXTENSION plpgsql IS 'PL/pgSQL procedural language';


SET search_path = public, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: activities; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE activities (
    aid bigint NOT NULL,
    paid bigint,
    cid bigint NOT NULL,
    time_start bigint NOT NULL,
    time_stop bigint,
    status smallint DEFAULT 1,
    comment character varying(255)
);


ALTER TABLE public.activities OWNER TO postgres;

--
-- Name: COLUMN activities.paid; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN activities.paid IS 'Parent-ID';


--
-- Name: components; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE components (
    cid bigint NOT NULL,
    hwid bigint NOT NULL,
    swid bigint NOT NULL,
    iid bigint NOT NULL,
    status smallint DEFAULT 1
);


ALTER TABLE public.components OWNER TO postgres;

--
-- Name: hardware; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE hardware (
    hwid bigint NOT NULL,
    description character varying(255) NOT NULL
);


ALTER TABLE public.hardware OWNER TO postgres;

--
-- Name: hardware_hwid_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE hardware_hwid_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.hardware_hwid_seq OWNER TO postgres;

--
-- Name: hardware_hwid_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE hardware_hwid_seq OWNED BY hardware.hwid;


--
-- Name: instances; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE instances (
    iid bigint NOT NULL,
    description character varying(255) NOT NULL
);


ALTER TABLE public.instances OWNER TO postgres;

--
-- Name: instances_iid_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE instances_iid_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.instances_iid_seq OWNER TO postgres;

--
-- Name: instances_iid_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE instances_iid_seq OWNED BY instances.iid;


--
-- Name: nodes_unid_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE nodes_unid_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.nodes_unid_seq OWNER TO postgres;

--
-- Name: nodes_unid_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE nodes_unid_seq OWNED BY components.cid;


--
-- Name: software; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE software (
    swid bigint NOT NULL,
    description character varying(255) NOT NULL
);


ALTER TABLE public.software OWNER TO postgres;

--
-- Name: software_swid_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE software_swid_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.software_swid_seq OWNER TO postgres;

--
-- Name: software_swid_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE software_swid_seq OWNED BY software.swid;


--
-- Name: cid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY components ALTER COLUMN cid SET DEFAULT nextval('nodes_unid_seq'::regclass);


--
-- Name: hwid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY hardware ALTER COLUMN hwid SET DEFAULT nextval('hardware_hwid_seq'::regclass);


--
-- Name: iid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY instances ALTER COLUMN iid SET DEFAULT nextval('instances_iid_seq'::regclass);


--
-- Name: swid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY software ALTER COLUMN swid SET DEFAULT nextval('software_swid_seq'::regclass);


--
-- Name: activities_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY activities
    ADD CONSTRAINT activities_pkey PRIMARY KEY (aid, cid);


--
-- Name: hardware_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY hardware
    ADD CONSTRAINT hardware_pkey PRIMARY KEY (hwid);


--
-- Name: instances_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY instances
    ADD CONSTRAINT instances_pkey PRIMARY KEY (iid);


--
-- Name: nodes_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY components
    ADD CONSTRAINT nodes_pkey PRIMARY KEY (cid);


--
-- Name: software_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY software
    ADD CONSTRAINT software_pkey PRIMARY KEY (swid);


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
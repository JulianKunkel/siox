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

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: activity; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE activity (
    aid activity_id NOT NULL,
    ucaid bigint NOT NULL,
    time_start bigint NOT NULL,
    time_stop bigint,
    error_value integer,
    parents activity_id[],
    attributes attribute[]
);


ALTER TABLE public.activity OWNER TO postgres;

--
-- Name: node; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE node (
    id bigint NOT NULL,
    hardware character varying(255),
    software character varying(255),
    instance character varying(255)
);


ALTER TABLE public.node OWNER TO postgres;

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
-- Name: aid_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE UNIQUE INDEX aid_idx ON activity USING btree (aid);


--
-- Name: id_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE UNIQUE INDEX id_idx ON node USING btree (id);


--
-- Name: node_id_idx; Type: INDEX; Schema: public; Owner: postgres; Tablespace: 
--

CREATE UNIQUE INDEX node_id_idx ON ontology USING btree (id);


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
# Run as postgres user
# CREATE USER siox WITH PASSWORD 'siox';
# GRANT ALL PRIVILEGES ON DATABASE topology to siox;
# CREATE DATABASE topology;
# NOW reconnect as user siox
# \connect topology


drop table attribute;

CREATE TABLE attribute
(
  id serial NOT NULL,
  name text,
  domaintypeid integer,
  datatype integer,
  CONSTRAINT attribute_pkey PRIMARY KEY (id),
  UNIQUE(name,domaintypeid)
)
WITH (
  OIDS=FALSE
);

drop table object;
CREATE TABLE object
(
  id serial NOT NULL,
  typeid integer,
  CONSTRAINT object_pkey PRIMARY KEY (id)
)
WITH (
  OIDS=FALSE
);

drop table relation;
CREATE TABLE relation
(
  parentobjectid integer,
  childname text,
  childobjectid integer,
  relationtypeid integer,
  UNIQUE(childname,relationtypeid,parentobjectid)
)
WITH (
  OIDS=FALSE

);

drop table type;
CREATE TABLE type
(
  id serial NOT NULL,
  name text,
  CONSTRAINT type_pkey PRIMARY KEY (id),
  UNIQUE(name)
)
WITH (
  OIDS=FALSE
);


drop table value;
CREATE TABLE value
(
  objectid integer,
  attributeid integer,
  type smallint,
  value text,
  UNIQUE(objectid, attributeid)
)
WITH (
  OIDS=FALSE
);

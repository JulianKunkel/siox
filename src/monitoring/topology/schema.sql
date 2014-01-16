CREATE TABLE attribute
(
  id serial NOT NULL,
  name text,
  domaintypeid integer,
  datatype integer,
  CONSTRAINT attribute_pkey PRIMARY KEY (id)
)
WITH (
  OIDS=FALSE
);

CREATE TABLE object
(
  id serial NOT NULL,
  typeid integer,
  CONSTRAINT object_pkey PRIMARY KEY (id)
)
WITH (
  OIDS=FALSE
);

CREATE TABLE relation
(
  parentobjectid integer,
  childname text,
  childobjectid integer,
  relationtypeid integer
)
WITH (
  OIDS=FALSE
);

CREATE TABLE type
(
  id serial NOT NULL,
  name text,
  CONSTRAINT type_pkey PRIMARY KEY (id)
)
WITH (
  OIDS=FALSE
);

CREATE TABLE value
(
  objectid integer,
  attributeid integer,
  topologyvalue text
)
WITH (
  OIDS=FALSE
);

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
  topologyvalue text,
  UNIQUE(objectid, attributeid)
)
WITH (
  OIDS=FALSE
);

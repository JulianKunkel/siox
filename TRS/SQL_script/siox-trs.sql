-- SIOX SQL create script for PostgreSQL. 

CREATE DATABASE "Transaktionssystem-1";

\connect "Transaktionssystem-1"

-- DROP SCHEMA if exists Record_IO CASCADE;
CREATE SCHEMA Record_IO;

CREATE TABLE Record_IO.DID(
	fp	VARCHAR(40)	NOT NULL,
	fh	VARCHAR(40)	NOT NULL,
	fd	VARCHAR(40)	NOT NULL
);

CREATE TABLE Record_IO.AID(
	A_number NUMERIC(1,0)	NOT NULL
);

CREATE TABLE Record_IO.Caller_AID(
	CAID_number	NUMERIC(1,0)	NOT NULL
);

-- The SQL-Datatype TIME is depending on der SQL infrastructure up to 100ns precise.

CREATE TABLE Record_IO.Activity_duration(
	act_startpoint	TIME	NOT NULL,
	act_endpoint	TIME	NOT NULL
);

CREATE TABLE Record_IO.UNID(
	Makro_IID		VARCHAR(3)	NOT NULL,
	Makro_HID		VARCHAR(5)	NOT NULL,
	Midi_net_src		VARCHAR(20)	NOT NULL,
	Midi_net_target 	VARCHAR(20)	NOT NULL,
	Midi_net_protocol	VARCHAR(6)	NOT NULL,
	Mikro_softlayer		VARCHAR(30)	NOT NULL,
	Mikro_core		VARCHAR(10)	NOT NULL,
	Mikro_storagenode	VARCHAR(20)	NOT NULL
);

CREATE TABLE Record_IO.Call(
	Name_of_Call	VARCHAR(40)	NOT NULL,
	Size_Bytes	NUMERIC(1,0)	NOT NULL,
	Offset_Bytes	NUMERIC(1,0)	NOT NULL
);

CREATE TABLE Record_IO.Metrics(
	Throughput	DOUBLE PRECISION	NOT NULL
);

CREATE TABLE Record_IO.ACOM(
	SEND	TIME	NOT NULL,
	RECV	TIME	NOT NULL
);

-- ALTER TABLE Record_IO.DID ADD PRIMARY KEY (Y1);
-- ALTER TABLE Record_IO.AID ADD PRIMARY KEY (Y2);
-- ALTER TABLE Record_IO. Caller_AID ADD PRIMARY KEY (Y3);
-- ALTER TABLE Record_IO.Activity_duration ADD PRIMARY KEY (Y4);
-- ALTER TABLE Record_IO.UNID ADD PRIMARY KEY (Y5);
-- ALTER TABLE Record_IO.Call ADD PRIMARY KEY (Y6);
-- ALTER TABLE Record_IO.Metrics ADD PRIMARY KEY (Y7);
-- 
-- ALTER TABLE Record_IO.ACOM ADD PRIMARY KEY (Y8);
-- CREATE INDEX IX_DID on Record_IO.DID(Y1);
-- CREATE INDEX IX_AID on Record_IO.AID(Y2);
-- CREATE INDEX IX_Caller_AID on Record_IO.AID(Y3);
-- CREATE INDEX IX_Activity_duration on Record_IO.AID(Y4);
-- CREATE INDEX IX_UNID on Record_IO.UNID(Y5);
-- CREATE INDEX IX_Call on Record_IO.AID(Y6);
-- CREATE INDEX IX_Metrics on Record_IO.AID(Y7);
-- CREATE INDEX IX_ACOM on Record_IO.AID(Y8);

commit;

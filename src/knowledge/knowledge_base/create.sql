# su postgres
# psql

CREATE USER siox with ENCRYPTED PASSWORD 'local-siox-db-pwd';

CREATE DATABASE knowledge_base with ENCODING 'UTF-8' OWNER siox;

# psql -h localhost -U siox -W knowledge_base
# ENTER THE PWD:
# local-siox-db-pwd

# http://www.postgresql.org/docs/9.1/static/sql-createtable.html
# http://www.postgresql.org/docs/9.1/static/datatype-numeric.html
# http://www.postgresql.org/docs/9.1/static/datatype-character.html
# http://www.postgresql.org/docs/9.1/static/datatype-datetime.html

CREATE TABLE nodes (
   node_id        serial CONSTRAINT key PRIMARY KEY,
   hardware_uid  text CONSTRAINT secondkey UNIQUE,
);

CREATE TABLE network_interfaces (
  mac text CONSTRAINT key PRIMARY KEY,
  interface_name text,
  medium text,
  speed number,
);


CREATE TABLE storage_devices (
   device_id        serial CONSTRAINT key PRIMARY KEY,
   node_id          integer,
   model_name  text,
   local_address text NOT NULL,
   FOREIGN KEY (node_id) REFERENCES nodes (node_id),
);
# Local_address identifies the device within the node, could be a UID or device ID (sda2)
# How do we localize devices:
# select node_id from nodes where hardware_uid = "<hostname>"
# select all devices with the node_id, and compare local_address

CREATE TABLE filesystems (
   fs_id              serial CONSTRAINT key PRIMARY KEY,
   uid       text CONSTRAINT description UNIQUE,

);

# How do we localize filesystems?
# UNIQUE ID: <fs-type>-<ID> e.g. PVFS-my-fs

# Do we need mountpoints?
# No, because node limits and fs limits suffices.

# Show table:
# \d
# \d devices

# Plugin may need system information for node, device OR file system.
# The plugin is expected to work for one component.
# It is allowed to access / use multiple tables, e.g.
# systeminfo_for_<NAME>_<TABLE> zu erstellen.
# Sharing of a table with other plugins?

# Examples:

# Measured latency & Throughput of a file-system
CREATE TABLE systeminfo_for_iotestplugin_filesystem_info(
	fs_id integer,
	FOREIGN KEY (fs_id) REFERENCES filesystems (fs_id),
        max_throughput float,
        min_latency float,
        avg_latency ...
)

# Measured network throughput and latency
CREATE TABLE systeminfo_for_iotestplugin_nodeinfo(
	node_id integer,
	FOREIGN KEY (node_id) REFERENCES nodes (node_id),
        network_throughput float,
        network_latency float,
)


# Plugin statistics are useful per application, per node and global.
# Application statistics could be volatile while global onces help us to optimize / remove plugins..
CREATE TABLE systeminfo_for_iotestplugin_statistics(
	good_choice_count integer,
	bad_choice_count integer,
	last_good_choice timestamp,
)
# Maybe this information should be reported and stored in a RRD-database similar to munin and not in our DB at all?
# If so, we need standardized way to report and aggregate this information...


# PATTERN
CREATE TABLE knowledge_for_iotestplugin(
	pattern text CONSTRAINT key PRIMARY KEY,
)


# How can we create a report for an application showing all the statistics, the determined bottlenecks and expert system for the optimization potential?


project(SIOX)

add_subdirectory(activity_forwarder)
add_subdirectory(activity_net_forwarder)
add_subdirectory(anomaly-skeleton)
add_subdirectory(filewriter)
add_subdirectory(fadviseReadAhead)
add_subdirectory(file-surveyor)
add_subdirectory(activity-histogram-adpi)

if (PostgreSQL_FOUND)
	add_subdirectory(postgres_writer)
endif (PostgreSQL_FOUND)


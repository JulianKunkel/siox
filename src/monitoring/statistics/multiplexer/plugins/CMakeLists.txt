project(SIOX)

add_subdirectory(testListener)
add_subdirectory(filewriter)
add_subdirectory(statisticsHealthADPI)
add_subdirectory(energyEfficiencyHealthADPI)

if (PostgreSQL_FOUND)
	add_subdirectory(postgres_writer)
	add_subdirectory(postgres_interval_writer)
endif (PostgreSQL_FOUND)

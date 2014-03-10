project(SIOX)

add_subdirectory(testListener)
add_subdirectory(filewriter)
add_subdirectory(statisticsHealthADPI)

if (PostgreSQL_FOUND)
	add_subdirectory(postgres_writer)
endif (PostgreSQL_FOUND)

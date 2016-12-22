project(SIOX)

set(SIOX_INCLUDE_DIRS ${SIOX_INCLUDE_DIRS} ${SIOX_SOURCES}/statistics)


include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-monitoring-Statistic
	StatisticTypesSerializableText.cpp
)

target_link_libraries(
	siox-monitoring-Statistic
	#-L${Boost_LIBRARY_DIR}
	${Boost_LIBRARIES}
)


siox_run_serializer(StatisticsFileWriterOptions.hpp StatisticsFileWriterOptions.cpp)
siox_run_serializer(StatisticsIntegratorOptions.hpp StatisticsIntegratorOptions.cpp)
siox_run_text_serializer(StatisticTypesSerializable.hpp StatisticTypesSerializableText.cpp)

add_subdirectory(collector)
add_subdirectory(multiplexer)
add_subdirectory(test)

install(TARGETS siox-monitoring-Statistic LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

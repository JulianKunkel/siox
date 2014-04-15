project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_library(
	siox-monitoring-statisticsMultiplexerPlugin-healthADPI
	EnergyEfficiencyHealthADPI.cpp
	EnergyEfficiencyHealthADPIOptions.cpp
)

target_link_libraries(
	siox-monitoring-statisticsMultiplexerPlugin-healthADPI
	siox-core-AutoConfigurator
	siox-monitoring-Statistic
)

siox_run_serializer(EnergyEfficiencyHealthADPIOptions.hpp EnergyEfficiencyHealthADPIOptions.cpp)


install(TARGETS siox-monitoring-statisticsMultiplexerPlugin-energyEfficiencyHealthADPI LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})


#add_subdirectory(test)

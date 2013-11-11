project(SIOX)

include_directories(
	${SIOX_INCLUDE}
	${SIOX_SOURCES}
)

add_library(
	siox-core-reporting-ConsoleReporter
	ConsoleReporter.cpp
	ConsoleReporterOptions.cpp
)

target_link_libraries(
	siox-core-reporting-ConsoleReporter
	siox-core-AutoConfigurator
)

siox_run_serializer(ConsoleReporterOptions.hpp ConsoleReporterOptions.cpp)


add_subdirectory(test)

install(TARGETS siox-core-reporting-ConsoleReporter LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

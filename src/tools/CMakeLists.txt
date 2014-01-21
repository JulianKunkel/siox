project(SIOX)

INSTALL_PROGRAMS(/bin FILES siox-plot-trace-output)

add_subdirectory(TraceReader)
add_subdirectory(daemon)
add_subdirectory(instrumenter)

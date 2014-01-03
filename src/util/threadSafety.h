#ifndef UTIL_THREAD_SAFETY
#define UTIL_THREAD_SAFETY

#ifdef  __cplusplus
extern "C" {
#endif

// If we are in the monitoring namespace we should prevent calling a siox_ll function.
// Needed for the instrumentation with DL_SYM to avoid circular dependencies.
extern __thread int siox_monitoring_namespace;

inline int monitoring_namespace_active(){
	return siox_monitoring_namespace;
}

inline int monitoring_namespace_deactivated(){
	return siox_monitoring_namespace == 0;
}

inline void monitoring_namespace_inc(){
	siox_monitoring_namespace++;
}

inline void monitoring_namespace_dec(){
	siox_monitoring_namespace--;
}

inline void monitoring_namespace_protect_thread(){
	siox_monitoring_namespace++;
}

#ifdef  __cplusplus
}
#endif

#endif
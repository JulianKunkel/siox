#ifndef CORE_MODULE_INTERFACE_HPP
#define CORE_MODULE_INTERFACE_HPP

namespace core {
	class ModuleInterface {
		public:
			// Compile time and run time calculation of the module instanciator symbol names.
			#define MODULE_INSTANCIATOR_NAME(INTERFACE_NAME) get_instance_ ## INTERFACE_NAME
			static std::string instanciator_name(const std::string &interface_name) throw() {
				return "get_instance_" + interface_name;
			};

			virtual ~ModuleInterface(){};
	};
}

#endif

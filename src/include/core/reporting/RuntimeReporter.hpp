#ifndef CORE_COMPONENT_RUNTIME_REPORT_HPP
#define CORE_COMPONENT_RUNTIME_REPORT_HPP

#include <list>
#include <memory>

#include <core/reporting/ComponentReportInterface.hpp>
#include <core/component/Component.hpp>

namespace core{

/*
 * A module of this class receives the list of all existing components and may query them for reports.
 */
class RuntimeReporter : public core::Component {
	public:
		virtual void processFinalReport(const std::list<std::shared_ptr<ComponentReport>> & reports) = 0;
};

}

#define CORE_REPORTER_INTERFACE "core_reporter"


#endif
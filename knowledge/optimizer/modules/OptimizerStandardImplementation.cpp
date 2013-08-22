/**
 * @file OptimizerStandardImplementation.cpp
 *
 * A simple and straightforward implementation of the abstract Optimizer class.
 *
 * @date 2013-08-01
 * @author Michaela Zimmer
 */

#include <knowledge/optimizer/OptimizerImplementation.hpp>

#include <core/component/Component.hpp>
#include <knowledge/optimizer/OptimizerPlugin.hpp>
#include <monitoring/ontology/OntologyDatatypes.hpp>

#include <unordered_map>


namespace knowledge {

	class OptimizerStandardImplementation : public Optimizer {

		private:

			// Map to store plugins in, indexed by attributes' IDs
			unordered_map<OntologyAttributeID, OptimizerPlugin *> expert;


		protected:

			ComponentOptions * AvailableOptions() {
				return new ComponentOptions();
			}


		public:

			virtual void registerPlugin( const OntologyAttribute & attribute, const OptimizerPlugin * plugin ) {
				assert( plugin != nullptr );
				assert( expert[attribute.aID] == nullptr );

				expert[attribute.aID] = ( OptimizerPlugin * ) plugin;
			}


			virtual bool isPluginRegistered( const OntologyAttribute & attribute ) const {
				return ( expert.find( attribute.aID ) != expert.end() );
			}


			virtual void unregisterPlugin( const OntologyAttribute & attribute ) {
				expert.erase( attribute.aID );
			}


			virtual OntologyValue optimalParameter( const OntologyAttribute & attribute ) const throw( NotFoundError ) {
				///@todo Check for registered plug-in?
				auto res = expert.find( attribute.aID );

				if( res != expert.end() ) {
					return res->second->optimalParameter( attribute );
				} else {
					throw NotFoundError( "Illegal attribute!" );
				}
			}


			virtual void init() {

			}
	};
} // namespace knowledge


COMPONENT( knowledge::OptimizerStandardImplementation )

// BUILD_TEST_INTERFACE knowledge/optimizer/modules/

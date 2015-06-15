/*
 * =====================================================================================
 *
 *       Filename:  PredictorInterface.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/16/2015 09:53:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  PredictorInterface_INC
#define  PredictorInterface_INC

namespace ml {
	namespace alg {
		template <typename F, typename L> 
			class PredictorInterface
			{
				public:
					using Features = F;
					using Labels = L;
					using Sample = std::pair<F, L>;

					virtual Labels predict(const Features& features) = 0;
					virtual std::vector<Labels> predict(const std::vector<Features>& features) = 0;
					virtual void open(const std::string& filename) = 0;
					virtual int train() = 0;
					virtual int train(const std::vector<Features>&, const std::vector<Labels>&) = 0;
//					virtual void evaluate() = 0;
			};
	}		/* -----  end of namespace alg  ----- */
}		/* -----  end of namespace ml  ----- */
#endif   /* ----- #ifndef PredictorInterface_INC  ----- */

#ifndef __SVM_CONVERTER_H__
#define __SVM_CONVERTER_H__

#include "CoreMLConverter.h"

#include <shogun/classifier/svm/SVM.h>

namespace CoreML
{
	namespace Specification
	{
		class SupportVectorRegressor;
		class SupportVectorClassifier;
	}
}

namespace shogun
{
	namespace coreml
	{
		using SVMRegressorConverterType = CoreMLConverter<CSVM, CoreML::Specification::SupportVectorRegressor>;

		struct SVMRegressorConverter: SVMRegressorConverterType
		{
			virtual ~SVMRegressorConverter() = default;
			std::shared_ptr<CoreML::Specification::Model> convert(const CMachine* m) const override;
		};

		using SVMClassifierConverterType = CoreMLConverter<CSVM, CoreML::Specification::SupportVectorClassifier>;
		struct SVMClassifierConverter: CoreMLConverter<CSVM, CoreML::Specification::SupportVectorClassifier>
		{
			// FIXME
			//std::shared_ptr<CoreML::Specification::Model> convert(const CMachine* m) const override;
		};
	}
}

#endif

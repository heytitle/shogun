#include "CoreMLConverter.h"

#include <shogun/lib/exception/NotFittedException.h>


using namespace shogun;
using namespace shogun::coreml;


std::shared_ptr<CoreMLModel> shogun::coreml::convert(const CMachine* m)
{
	auto converter_registry = ConverterFactory::instance();
	std::string machine_name(m->get_name());
	if (!m->is_trained())
		throw NotFittedException("The supplied machine is not trained!");

	auto spec = (*converter_registry)(machine_name)()->convert(m);
	return std::make_shared<CoreMLModel>(spec);
}

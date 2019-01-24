#ifndef __COREML_CONVERTER_H__
#define __COREML_CONVERTER_H__

#include <functional>
#include <unordered_map>
#include <unordered_set>

#include <shogun/machine/Machine.h>

#include "CoreMLModel.h"

namespace shogun
{
	namespace coreml
	{
		static constexpr int32_t SPECIFICATION_VERSION = 1;

		struct ICoreMLConverter
		{
			virtual std::shared_ptr<CoreML::Specification::Model> convert(const CMachine* m) const = 0;
		};

		template <class, class>
		struct CoreMLConverter;

		template <class I, class O>
		struct CoreMLConverter: ICoreMLConverter
		{
			typedef I input_type;
			typedef O output_type;
			static const std::unordered_set<std::string> supported_types;

			static O* convert(const I* o);
		};

		class ConverterFactory
		{
			typedef std::function<std::shared_ptr<ICoreMLConverter>()> ConverterFactoryFunction;

		public:

			auto size() const
			{
				return m_registry.size();
			}

			auto register_converter(const std::string& machine_name, ConverterFactoryFunction f)
			{
				return m_registry.emplace(std::make_pair(machine_name, f)).second;
			}

			ConverterFactoryFunction& operator()(const std::string& m)
			{
				auto f = m_registry.find(m);
				if (f == m_registry.end())
					throw std::runtime_error("The provided machine cannot be converted to CoreML format!");
				return f->second;
			}

			static ConverterFactory* instance()
			{
				static ConverterFactory* f = new ConverterFactory();
				return f;
			}

		private:
			std::unordered_map<std::string, ConverterFactoryFunction> m_registry;
		};

		std::shared_ptr<CoreMLModel> convert(const CMachine* m);

#define REGISTER_COREML_CONVERTER(factory, classname, machines, function) \
	static int register_converter##classname = []() { \
		for (auto m: machines) \
			factory->register_converter(m, function); \
		return factory->size(); \
	}();

#define REGISTER_CONVERTER(classname, machines, function) \
	REGISTER_COREML_CONVERTER(ConverterFactory::instance(), classname, machines, function)
	}
}

#endif

#ifndef __COREML_MODEL_H__
#define __COREML_MODEL_H__

#include <memory>
#include <ostream>
#include <string>

namespace CoreML
{
	namespace Specification
	{
		class Model;
	}
}

namespace shogun
{
	namespace coreml
	{
		class CoreMLModel
		{
		public:
			CoreMLModel();
			CoreMLModel(std::shared_ptr<CoreML::Specification::Model>);
			~CoreMLModel();

			void save(const std::string& filename) const;
			void save(std::ostream& out) const;

			std::shared_ptr<CoreML::Specification::Model> get_specification() const
			{
				return m_spec;
			}

		private:
			std::shared_ptr<CoreML::Specification::Model> m_spec;
		};
    }
}
#endif

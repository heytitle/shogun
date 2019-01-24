#include "CoreMLModel.h"

#include <fstream>

#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "Model.pb.h"

using namespace shogun::coreml;

CoreMLModel::CoreMLModel():
	m_spec(std::make_shared<CoreML::Specification::Model>())
{
}

CoreMLModel::CoreMLModel(std::shared_ptr<CoreML::Specification::Model> spec):
	m_spec(spec)
{
}

CoreMLModel::~CoreMLModel()
{
	m_spec.reset();
}

void CoreMLModel::save(const std::string& filename) const
{
	std::fstream out(filename, std::ios::binary | std::ios::out);
	this->save(out);
	out.close();
}

void CoreMLModel::save(std::ostream& out) const
{
	::google::protobuf::io::OstreamOutputStream pb_out(&out);
	if (!m_spec->SerializeToZeroCopyStream(&pb_out))
		throw std::runtime_error("could not save");
}

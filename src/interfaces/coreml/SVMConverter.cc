#include "SVMConverter.h"
#include "KernelConverter.h"

#include "Model.pb.h"
#include "SVM.pb.h"

#include <shogun/classifier/svm/SVM.h>
#include <shogun/features/DenseFeatures.h>
#include <shogun/features/SparseFeatures.h>
#include <shogun/kernel/GaussianKernel.h>
#include <shogun/kernel/PolyKernel.h>
#include <shogun/kernel/SigmoidKernel.h>

using namespace shogun;
using namespace shogun::coreml;

//using SVMRegressorConverterType = CoreMLConverter<CSVM, CoreML::Specification::SupportVectorRegressor>;

template<>
const std::unordered_set<std::string> SVMRegressorConverterType::supported_types = {"LibSVR", "SVRLight"};

template<typename T>
static bool set_support_vectors(const CSVM* svm, const CKernel* k, T* machine_spec)
{
	auto svs = svm->get_support_vectors();
	auto lhs = k->get_lhs();
	if (lhs == nullptr)
		throw std::runtime_error("Features are not set in kernel (required for support vectors), cannot export to CoreML!");

	switch (lhs->get_feature_type())
	{
		case F_UNKNOWN:
		case F_BOOL:
		case F_BYTE:
		case F_ANY:
			throw std::runtime_error("CoreML does not support the provided feature type!");
	}

	switch (lhs->get_feature_class())
	{

		case C_DENSE:
		{
			auto svs_spec = machine_spec->mutable_densesupportvectors();
			// FIXME: support all CDenseFeatures type!
			auto dense_features = dynamic_cast<CDenseFeatures<float64_t>*>(lhs);
			for (auto sv_idx: svs)
			{
				auto sv = dense_features->get_feature_vector(sv_idx);
				auto sv_spec = svs_spec->add_vectors();
				for (auto v: sv)
					sv_spec->add_values(v);
			}
			return true;
		}
/*
		case C_SPARSE:
		{
			auto svs_spec = machine_spec->mutable_sparsesupportvectors();
			for (auto sv_idx: svs)
			{
				auto sv = lhs->get_sparse_feature_vector(sv_idx);
				auto sv_spec = svs_spec->add_vectors();
				for (index_t i = 0; i < sv->num_feat_entries; ++i)
				{
					auto node = sv_spec->add_nodes();
					node->set_value(sv->features[i].entry);
					node->set_index(sv->features[i].feat_index);
				}
			}
			break;
		}
		*/
		default:
			throw std::runtime_error("CoreML does not support the provided feature class!");

	}
	return false;
}

template<>
CoreML::Specification::SupportVectorRegressor* SVMRegressorConverterType::convert(const CSVM* svm)
{
	REQUIRE(
		SVMRegressorConverterType::supported_types.find(svm->get_name())
			!= SVMRegressorConverterType::supported_types.end(),
		"Exporting %s to CoreML format is not supported!", svm->get_name())
	auto spec = new CoreML::Specification::SupportVectorRegressor();

	try
	{
		// set kernel
		auto kernel = svm->get<CKernel*>("kernel");
		auto kernel_spec = KernelConverter::convert(kernel);
		spec->set_allocated_kernel(kernel_spec);

		// set coefficients
		auto coeffs = svm->get_alphas();
		auto coeffs_spec = spec->mutable_coefficients();
		for (auto i: coeffs)
			coeffs_spec->add_alpha(i);

		// set bias
		spec->set_rho(svm->get_bias());

		// set support vectors
		set_support_vectors(svm, kernel, spec);
	}
	catch(const std::runtime_error& e)
	{
		delete spec;
		throw;
	}

	return spec;
}

std::shared_ptr<CoreML::Specification::Model> SVMRegressorConverter::convert(const CMachine* m) const
{
	auto spec = std::make_shared<CoreML::Specification::Model>();
	spec->set_specificationversion(SPECIFICATION_VERSION);
	spec->set_allocated_supportvectorregressor(CoreMLConverter::convert(static_cast<const SVMRegressorConverter::input_type*>(m)));
	return spec;
}

REGISTER_CONVERTER(SVMRegressorConverter, SVMRegressorConverterType::supported_types, []() { return std::make_shared<SVMRegressorConverter>(); })

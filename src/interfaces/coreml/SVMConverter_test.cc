#include <gtest/gtest.h>
#include "SVMConverter.h"

#include <shogun/kernel/GaussianKernel.h>

#include <shogun/regression/svr/LibSVR.h>
#include <shogun/regression/svr/SVRLight.h>

#include <shogun/features/DenseFeatures.h>

#include "SVM.pb.h"

using namespace shogun;
using namespace shogun::coreml;

template <typename T>
class RegressorTest : public ::testing::Test {};

using RegressorTypes = ::testing::Types<CLibSVR, CSVRLight>;
TYPED_TEST_CASE(RegressorTest, RegressorTypes);

TYPED_TEST(RegressorTest, convert)
{
	//TODO: move fixture
	index_t n = 100;
	float64_t x_range=6;
	SGMatrix<float64_t> feat_train(1, n);
	SGVector<float64_t> lab_train(n);

	for (index_t i=0; i<n; ++i)
	{
		feat_train[i] = CMath::random(0.0, x_range);
		lab_train[i] = std::sin(feat_train[i]);
	}

	auto labels_train = some<CRegressionLabels>(lab_train);
	auto features_train = some<CDenseFeatures<float64_t>>(feat_train);

	// TODO: make kernel a parameter?
	auto k = some<CGaussianKernel>(10);
	k->init(features_train, features_train);
	auto m = some<TypeParam>(10.0, 0.01, k, labels_train);
	m->train();

	auto spec = std::shared_ptr<SVMRegressorConverter::output_type>(SVMRegressorConverterType::convert(m));

	// check rho
	ASSERT_EQ(m->get_bias(), spec->rho());

	// check coeffs
	auto coeffs = m->get_alphas();
	auto coeffs_spec = spec->coefficients();
	ASSERT_EQ(coeffs.size(), coeffs_spec.alpha_size());

	int ctr = 0;
	for (auto c: coeffs)
		ASSERT_EQ(c, coeffs_spec.alpha(ctr++));

	// check kernel
	ASSERT_TRUE(spec->has_kernel());
	auto kernel_spec = spec->kernel();
	ASSERT_TRUE(kernel_spec.has_rbfkernel());

	// check support vectors
	ASSERT_TRUE(spec->has_densesupportvectors());
	auto svs_idx = m->get_support_vectors();
	auto svs_spec = spec->densesupportvectors();

	ASSERT_EQ(svs_idx.vlen, svs_spec.vectors_size());
	ctr = 0;
	for (auto idx: svs_idx)
	{
		auto sv_spec = svs_spec.vectors(ctr++);
		auto sv = features_train->get_feature_vector(idx);
		ASSERT_EQ(sv.vlen, sv_spec.values_size());

		int j = 0;
		for (auto v: sv)
			ASSERT_EQ(v, sv_spec.values(j++));
	}
}

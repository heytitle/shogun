#include "KernelConverter.h"

#include "Model.pb.h"
#include "SVM.pb.h"

using namespace shogun;
using namespace shogun::coreml;

template<typename I>
struct converter
{
    static auto convert(const I* k);
    static auto convert(const CKernel* k)
    {
        return convert(k->as<const I>());
    }
};

template<>
auto converter<CGaussianKernel>::convert(const CGaussianKernel *k)
{
    auto rbf = new CoreML::Specification::RBFKernel();
    rbf->set_gamma(k->get_width());
    return rbf;
}

template<>
auto converter<CSigmoidKernel>::convert(const CSigmoidKernel *k)
{
    auto sigmoid_kernel = new CoreML::Specification::SigmoidKernel();
    sigmoid_kernel->set_gamma(k->get<float64_t>("gamma"));
    sigmoid_kernel->set_c(k->get<float64_t>("coef0"));
    return sigmoid_kernel;
}

template<>
auto converter<CPolyKernel>::convert(const CPolyKernel *k)
{
    auto poly_kernel = new CoreML::Specification::PolyKernel();
    poly_kernel->set_degree(k->get<int32_t>("degree"));
    poly_kernel->set_c(k->get<float64_t>("c"));
    poly_kernel->set_gamma(k->get<float64_t>("gamma"));

    return poly_kernel;
}

CoreML::Specification::Kernel* KernelConverter::convert(const CKernel *k)
{
    auto kernel_spec = new CoreML::Specification::Kernel();
    auto kernel_type = std::string(k->get_name());

	if (kernel_type == "LinearKernel")
	{
        kernel_spec->set_allocated_linearkernel(new CoreML::Specification::LinearKernel());
	}
	else if (kernel_type == "GaussianKernel")
	{
        //auto gk = k->as<const CGaussianKernel>();
        kernel_spec->set_allocated_rbfkernel(converter<CGaussianKernel>::convert(k));
	}
	else if (kernel_type == "SigmoidKernel")
	{
        kernel_spec->set_allocated_sigmoidkernel(converter<CSigmoidKernel>::convert(k));
	}
	else if (kernel_type == "PolyKernel")
	{
        kernel_spec->set_allocated_polykernel(converter<CPolyKernel>::convert(k));
	}
    else
    {
        delete kernel_spec;
        throw std::runtime_error("Kernel type is not supported in CoreML");
    }

    return kernel_spec;
};

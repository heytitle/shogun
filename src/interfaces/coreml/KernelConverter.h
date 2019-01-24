#ifndef __KERNEL_CONVERTER_H__
#define __KERNEL_CONVERTER_H__

#include "CoreMLConverter.h"

#include <shogun/kernel/Kernel.h>
#include <shogun/kernel/LinearKernel.h>
#include <shogun/kernel/GaussianKernel.h>
#include <shogun/kernel/PolyKernel.h>
#include <shogun/kernel/SigmoidKernel.h>

namespace CoreML
{
	namespace Specification
	{
		class Kernel;
	}
}

namespace shogun
{
	namespace coreml
	{
        struct KernelConverter
        {
            static CoreML::Specification::Kernel* convert(const CKernel* k);
        };
	}
}

#endif

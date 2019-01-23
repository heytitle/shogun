#ifndef __SG_VARIANT_H__
#define __SG_VARIANT_H__

#include <shogun/lib/config.h>

#include <variant>

namespace shogun
{
	using std::variant;
	using std::get;
	using std::get_if;
	using std::visit;
	using std::holds_alternative;
	using std::monostate;
	using std::bad_variant_access;
	using std::variant_size;
	using std::variant_size_v;
	using std::variant_alternative;
	using std::variant_alternative_t;
}

#endif // __SG_VARIANT_H__

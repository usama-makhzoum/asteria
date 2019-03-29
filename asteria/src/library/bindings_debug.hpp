// This file is part of Asteria.
// Copyleft 2018 - 2019, LH_Mouse. All wrongs reserved.

#ifndef ASTERIA_LIBRARY_BINDINGS_DEBUG_HPP_
#define ASTERIA_LIBRARY_BINDINGS_DEBUG_HPP_

#include "../fwd.hpp"

namespace Asteria {

extern bool std_debug_print(const Cow_Vector<Value>& values);
extern bool std_debug_dump(const Value& value, const Opt<D_integer>& indent = rocket::nullopt);

// Create an object that is to be referenced as `std.debug`.
extern D_object create_bindings_debug();

}  // namespace Asteria

#endif

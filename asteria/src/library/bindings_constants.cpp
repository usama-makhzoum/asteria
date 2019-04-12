// This file is part of Asteria.
// Copyleft 2018 - 2019, LH_Mouse. All wrongs reserved.

#include "../precompiled.hpp"
#include "bindings_constants.hpp"
#include "argument_reader.hpp"
#include "simple_binding_wrapper.hpp"
#include "../utilities.hpp"

namespace Asteria {

void create_bindings_constants(G_object& result, API_Version /*version*/)
  {
    //===================================================================
    // `std.constants.integer_max`
    //===================================================================
    result.insert_or_assign(rocket::sref("integer_max"),
      G_integer(
        INT64_MAX
      ));
    //===================================================================
    // `std.constants.integer_min`
    //===================================================================
    result.insert_or_assign(rocket::sref("integer_min"),
      G_integer(
        INT64_MIN
      ));
    //===================================================================
    // `std.constants.real_max`
    //===================================================================
    result.insert_or_assign(rocket::sref("real_max"),
      G_real(
        DBL_MAX
      ));
    //===================================================================
    // `std.constants.real_min`
    //===================================================================
    result.insert_or_assign(rocket::sref("real_min"),
      G_real(
        -DBL_MAX
      ));
    //===================================================================
    // `std.constants.real_epsilon`
    //===================================================================
    result.insert_or_assign(rocket::sref("real_epsilon"),
      G_real(
        DBL_EPSILON
      ));
    //===================================================================
    // `std.constants.size_max`
    //===================================================================
    result.insert_or_assign(rocket::sref("size_max"),
      G_integer(
        PTRDIFF_MAX
      ));
    //===================================================================
    // `std.constants.e`
    //===================================================================
    result.insert_or_assign(rocket::sref("e"),
      G_real(
        2.7182818284590452353602874713526624977572470937000
      ));
    //===================================================================
    // `std.constants.pi`
    //===================================================================
    result.insert_or_assign(rocket::sref("pi"),
      G_real(
        3.1415926535897932384626433832795028841971693993751
      ));
    //===================================================================
    // `std.constants.lb10`
    //===================================================================
    result.insert_or_assign(rocket::sref("lb10"),
      G_real(
        3.3219280948873623478703194294893901758648313930246
      ));
    //===================================================================
    // End of `std.constants`
    //===================================================================
  }

}  // namespace Asteria

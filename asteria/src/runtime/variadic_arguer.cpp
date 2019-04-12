// This file is part of Asteria.
// Copyleft 2018 - 2019, LH_Mouse. All wrongs reserved.

#include "../precompiled.hpp"
#include "variadic_arguer.hpp"
#include "../library/argument_reader.hpp"
#include "../utilities.hpp"

namespace Asteria {

void Variadic_Arguer::describe(std::ostream& os) const
  {
    os << "<built-in>.__varg([index]) @ " << this->m_sloc;
  }

Reference& Variadic_Arguer::invoke(Reference& self, const Global_Context& /*global*/, Cow_Vector<Reference>&& args) const
  {
    Argument_Reader reader(rocket::sref("<built-in>.__varg"), args);
    // Extract arguments.
    Opt<G_integer> qindex;
    if(reader.start().g(qindex).finish()) {
      auto nvargs = this->m_vargs.size();
      if(!qindex) {
        // Return the number of variadic arguments if `index` is `null` or absent.
        Reference_Root::S_constant xref = { G_integer(nvargs) };
        return self = rocket::move(xref);
      }
      // Return the argument at `index`.
      auto w = wrap_index(*qindex, nvargs);
      if(w.nprepend + w.nappend != 0) {
        ASTERIA_DEBUG_LOG("Variadic argument index is out of range: index = ", *qindex, ", nvarg = ", nvargs);
        return self = Reference_Root::S_null();
      }
      return self = this->m_vargs.at(w.rindex);
    }
    // Fail.
    reader.throw_no_matching_function_call();
  }

void Variadic_Arguer::enumerate_variables(const Abstract_Variable_Callback& callback) const
  {
    rocket::for_each(this->m_vargs, [&](const Reference& arg) { arg.enumerate_variables(callback);  });
  }

}  // namespace Asteria

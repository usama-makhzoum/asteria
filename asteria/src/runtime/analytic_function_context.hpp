// This file is part of Asteria.
// Copyleft 2018 - 2019, LH_Mouse. All wrongs reserved.

#ifndef ASTERIA_RUNTIME_ANALYTIC_FUNCTION_CONTEXT_HPP_
#define ASTERIA_RUNTIME_ANALYTIC_FUNCTION_CONTEXT_HPP_

#include "../fwd.hpp"
#include "analytic_context.hpp"
#include "../rocket/static_vector.hpp"
#include "../rocket/cow_vector.hpp"
#include "../rocket/refcounted_object.hpp"

namespace Asteria {

class Analytic_Function_Context : public Analytic_Context
  {
  private:
    // N.B. If you have ever changed the capacity, remember to update 'executive_function_context.hpp' as well.
    rocket::static_vector<Reference_Dictionary::Template, 7> m_predef_refs;

  public:
    explicit Analytic_Function_Context(const Abstract_Context *parent_opt) noexcept
      : Analytic_Context(parent_opt),
        m_predef_refs()
      {
      }
    ROCKET_NONCOPYABLE_DESTRUCTOR(Analytic_Function_Context);

  public:
    void initialize(const rocket::cow_vector<rocket::prehashed_string> &params);
  };

}

#endif

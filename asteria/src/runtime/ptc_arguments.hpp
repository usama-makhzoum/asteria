// This file is part of Asteria.
// Copyleft 2018 - 2020, LH_Mouse. All wrongs reserved.

#ifndef ASTERIA_RUNTIME_PTC_ARGUMENTS_HPP_
#define ASTERIA_RUNTIME_PTC_ARGUMENTS_HPP_

#include "../fwd.hpp"
#include "../source_location.hpp"

namespace asteria {

class PTC_Arguments
final
  : public Rcfwd<PTC_Arguments>
  {
  private:
    // These describe characteristics of the function call.
    Source_Location m_sloc;
    PTC_Aware m_ptc;

    // These are the target function and its arguments.
    cow_function m_target;
    cow_vector<Reference> m_args_self;

    // These are deferred expressions.
    cow_bivector<Source_Location, AVMC_Queue> m_defer;

    // These are source information of the enclosing function.
    Source_Location m_encl_sloc;
    cow_string m_encl_func;

  public:
    PTC_Arguments(const Source_Location& sloc, PTC_Aware ptc,
                  const cow_function& target, cow_vector<Reference>&& args_self)
      : m_sloc(sloc), m_ptc(ptc),
        m_target(target), m_args_self(::std::move(args_self))
      { }

    ASTERIA_COPYABLE_DESTRUCTOR(PTC_Arguments);

  public:
    const Source_Location&
    sloc()
    const noexcept
      { return this->m_sloc;  }

    PTC_Aware
    ptc_aware()
    const noexcept
      { return this->m_ptc;  }

    const cow_function&
    get_target()
    const noexcept
      { return this->m_target;  }

    const cow_vector<Reference>&
    get_arguments_and_self()
    const noexcept
      { return this->m_args_self;  }

    cow_vector<Reference>&
    open_arguments_and_self()
    noexcept
      { return this->m_args_self;  }

    const cow_bivector<Source_Location, AVMC_Queue>&
    get_defer()
    const noexcept
      { return this->m_defer;  }

    cow_bivector<Source_Location, AVMC_Queue>&
    open_defer()
    noexcept
      { return this->m_defer;  }

    const Source_Location&
    enclosing_sloc()
    const noexcept
      { return this->m_encl_sloc;  }

    const cow_string&
    enclosing_func()
    const noexcept
      { return this->m_encl_func;  }

    PTC_Arguments&
    set_enclosing_function(const Source_Location& xsloc, const cow_string& xfunc)
      {
        this->m_encl_sloc = xsloc;
        this->m_encl_func = xfunc;
        return *this;
      }
  };

}  // namespace asteria

#endif

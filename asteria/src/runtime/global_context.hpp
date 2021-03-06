// This file is part of Asteria.
// Copyleft 2018 - 2021, LH_Mouse. All wrongs reserved.

#ifndef ASTERIA_RUNTIME_GLOBAL_CONTEXT_HPP_
#define ASTERIA_RUNTIME_GLOBAL_CONTEXT_HPP_

#include "../fwd.hpp"
#include "abstract_context.hpp"
#include "../recursion_sentry.hpp"

namespace asteria {

class Global_Context
  : public Abstract_Context
  {
  private:
    Recursion_Sentry m_sentry;

    rcfwdp<Abstract_Hooks> m_qhooks;
    rcfwdp<Genius_Collector> m_gcoll;
    rcfwdp<Random_Engine> m_prng;
    rcfwdp<Loader_Lock> m_ldrlk;
    rcfwdp<Variable> m_vstd;

  public:
    // A global context has no parent.
    explicit
    Global_Context(API_Version version = api_version_latest);

  protected:
    bool
    do_is_analytic()
      const noexcept final
      { return this->is_analytic();  }

    Abstract_Context*
    do_get_parent_opt()
      const noexcept final
      { return this->get_parent_opt();  }

    Reference*
    do_create_lazy_reference(Reference* /*hint_opt*/, const phsh_string& /*name*/)
      const override
      { return nullptr;  }

  public:
    ASTERIA_NONCOPYABLE_DESTRUCTOR(Global_Context);

    bool
    is_analytic()
      const noexcept
      { return false;  }

    Abstract_Context*
    get_parent_opt()
      const noexcept
      { return nullptr;  }

    // This provides stack overflow protection.
    Recursion_Sentry
    copy_recursion_sentry()
      const
      { return this->m_sentry;  }

    const void*
    get_recursion_base()
      const noexcept
      { return this->m_sentry.get_base();  }

    Global_Context&
    set_recursion_base(const void* base)
      noexcept
      { return this->m_sentry.set_base(base), *this;  }

    // This helps debugging and profiling.
    ASTERIA_INCOMPLET(Abstract_Hooks)
    rcptr<Abstract_Hooks>
    get_hooks_opt()
      const noexcept
      { return unerase_pointer_cast<Abstract_Hooks>(this->m_qhooks);  }

    ASTERIA_INCOMPLET(Abstract_Hooks)
    Global_Context&
    set_hooks(rcptr<Abstract_Hooks> hooks_opt)
      noexcept
      { return this->m_qhooks = ::std::move(hooks_opt), *this;  }

    // These are interfaces for individual global components.
    ASTERIA_INCOMPLET(Genius_Collector)
    rcptr<Genius_Collector>
    genius_collector()
      const noexcept
      { return unerase_pointer_cast<Genius_Collector>(this->m_gcoll);  }

    ASTERIA_INCOMPLET(Random_Engine)
    rcptr<Random_Engine>
    random_engine()
      const noexcept
      { return unerase_pointer_cast<Random_Engine>(this->m_prng);  }

    ASTERIA_INCOMPLET(Loader_Lock)
    rcptr<Loader_Lock>
    loader_lock()
      const noexcept
      { return unerase_pointer_cast<Loader_Lock>(this->m_ldrlk);  }

    ASTERIA_INCOMPLET(Variable)
    rcptr<Variable>
    std_variable()
      const noexcept
      { return unerase_pointer_cast<Variable>(this->m_vstd);  }

    // Get the maximum API version that is supported when this library is built.
    // N.B. This function must not be inlined for this reason.
    API_Version
    max_api_version()
      const noexcept;
  };

}  // namespace asteria

#endif

// This file is part of Asteria.
// Copyleft 2018 - 2021, LH_Mouse. All wrongs reserved.

#ifndef ASTERIA_RUNTIME_REFERENCE_HPP_
#define ASTERIA_RUNTIME_REFERENCE_HPP_

#include "../fwd.hpp"
#include "../value.hpp"
#include "reference_modifier.hpp"

namespace asteria {

class Reference
  {
  public:
    enum Index : uint8_t
      {
        index_uninit     = 0,
        index_void       = 1,
        index_temporary  = 2,
        index_variable   = 3,
        index_ptc_args   = 4,
      };

  private:
    Value m_value;
    rcfwdp<Variable> m_var;
    rcfwdp<PTC_Arguments> m_ptca;

    cow_vector<Reference_Modifier> m_mods;
    Index m_index = index_uninit;

  public:
    // Constructors and assignment operators
    constexpr
    Reference()
      noexcept
      { }

    Reference(const Reference& other)
      noexcept
      { *this = other;  }

    Reference(Reference&& other)
      noexcept
      { *this = ::std::move(other);  }

    Reference&
    operator=(const Reference& other)
      noexcept
      {
        // Note not all fields have to be copied.
        switch(other.m_index) {
          case index_uninit:
          case index_void:
            break;

          case index_temporary:
            this->m_value = other.m_value;
            break;

          case index_variable:
            this->m_var = other.m_var;
            break;

          case index_ptc_args:
            this->m_ptca = other.m_ptca;
            break;
        }

        this->m_mods = other.m_mods;
        this->m_index = other.m_index;
        return *this;
      }

    Reference&
    operator=(Reference&& other)
      noexcept
      {
        // Note not all fields have to be moved.
        switch(other.m_index) {
          case index_uninit:
          case index_void:
            break;

          case index_temporary:
            this->m_value = ::std::move(other.m_value);
            break;

          case index_variable:
            this->m_var = ::std::move(other.m_var);
            break;

          case index_ptc_args:
            this->m_ptca = ::std::move(other.m_ptca);
            break;
        }

        this->m_mods = ::std::move(other.m_mods);
        this->m_index = other.m_index;
        return *this;
      }

  private:
    const Value&
    do_dereference_readonly_slow()
      const;

    Value&
    do_mutate_into_temporary_slow();

    Reference&
    do_finish_call_slow(Global_Context& global);

  public:
    ~Reference();

    Index
    index()
      const noexcept
      { return this->m_index;  }

    bool
    is_uninit()
      const noexcept
      { return this->index() == index_uninit;  }

    Reference&
    set_uninit()
      noexcept
      {
        this->m_index = index_uninit;
        return *this;
      }

    bool
    is_void()
      const noexcept
      { return this->index() == index_void;  }

    Reference&
    set_void()
      noexcept
      {
        this->m_index = index_void;
        return *this;
      }

    bool
    is_temporary()
      const noexcept
      { return this->index() == index_temporary;  }

    template<typename XValT>
    Reference&
    set_temporary(XValT&& xval)
      noexcept
      {
        this->m_value = ::std::forward<XValT>(xval);
        this->m_mods.clear();
        this->m_index = index_temporary;
        return *this;
      }

    bool
    is_variable()
      const noexcept
      { return this->index() == index_variable;  }

    ASTERIA_INCOMPLET(Variable)
    rcptr<Variable>
    get_variable_opt()
      const noexcept
      {
        return this->is_variable()
            ? unerase_pointer_cast<Variable>(this->m_var)
            : nullptr;
      }

    ASTERIA_INCOMPLET(Variable)
    Reference&
    set_variable(const rcptr<Variable>& var)
      noexcept
      {
        this->m_var = var;
        this->m_mods.clear();
        this->m_index = index_variable;
        return *this;
      }

    bool
    is_ptc_args()
      const noexcept
      { return this->index() == index_ptc_args;  }

    ASTERIA_INCOMPLET(PTC_Arguments)
    rcptr<PTC_Arguments>
    get_ptc_args_opt()
      const noexcept
      {
        return this->is_ptc_args()
            ? unerase_pointer_cast<PTC_Arguments>(this->m_ptca)
            : nullptr;
      }

    ASTERIA_INCOMPLET(PTC_Arguments)
    Reference&
    set_ptc_args(const rcptr<PTC_Arguments>& ptca)
      noexcept
      {
        this->m_ptca = ptca;
        this->m_index = index_ptc_args;
        return *this;
      }

    Reference&
    swap(Reference& other)
      noexcept
      {
        this->m_value.swap(other.m_value);
        this->m_var.swap(other.m_var);
        this->m_ptca.swap(other.m_ptca);
        this->m_mods.swap(other.m_mods);
        ::std::swap(this->m_index, other.m_index);
        return *this;
      }

    Variable_Callback&
    enumerate_variables(Variable_Callback& callback)
      const;

    // A modifier is created by a dot or bracket operator.
    // For instance, the expression `obj.x[42]` results in a reference having two
    // modifiers. Modifiers can be removed to yield references to ancestor objects.
    // Removing the last modifier shall yield the constant `null`.
    size_t
    count_modifiers()
      const noexcept
      { return this->m_mods.size();  }

    Reference&
    push_modifier_array_index(int64_t index)
      {
        Reference_Modifier::S_array_index xmod = { index };
        this->m_mods.emplace_back(::std::move(xmod));
        return *this;
      }

    Reference&
    push_modifier_object_key(const phsh_string& key)
      {
        Reference_Modifier::S_object_key xmod = { key };
        this->m_mods.emplace_back(::std::move(xmod));
        return *this;
      }

    Reference&
    push_modifier_array_head()
      {
        this->m_mods.emplace_back(Reference_Modifier::S_array_head());
        return *this;
      }

    Reference&
    push_modifier_array_tail()
      {
        this->m_mods.emplace_back(Reference_Modifier::S_array_tail());
        return *this;
      }

    Reference&
    pop_modifier()
      noexcept
      {
        if(ROCKET_EXPECT(this->m_mods.empty())) {
          // Set to null.
          this->m_value = nullopt;
          this->m_index = index_temporary;
        }
        else {
          // Drop a modifier.
          this->m_mods.pop_back();
        }
        return *this;
      }

    // These are conceptual read/write functions.
    // Some references are placeholders that do not denote values.
    ROCKET_FORCED_INLINE_FUNCTION
    const Value&
    dereference_readonly()
      const
      {
        return ROCKET_EXPECT(this->is_temporary() && this->m_mods.empty())
            ? this->m_value
            : this->do_dereference_readonly_slow();
      }

    ROCKET_FORCED_INLINE_FUNCTION
    Value&
    mutate_into_temporary()
      {
        return ROCKET_EXPECT(this->is_temporary() && this->m_mods.empty())
            ? this->m_value
            : this->do_mutate_into_temporary_slow();
      }

    ROCKET_FORCED_INLINE_FUNCTION
    Reference&
    finish_call(Global_Context& global)
      {
        return ROCKET_EXPECT(!this->is_ptc_args())
            ? *this
            : this->do_finish_call_slow(global);
      }

    Value&
    dereference_mutable()
      const;

    Value
    dereference_unset()
      const;
  };

inline
void
swap(Reference& lhs, Reference& rhs)
  noexcept
  { lhs.swap(rhs);  }

}  // namespace asteria

#endif

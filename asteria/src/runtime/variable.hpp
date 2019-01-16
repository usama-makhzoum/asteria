// This file is part of Asteria.
// Copyleft 2018 - 2019, LH_Mouse. All wrongs reserved.

#ifndef ASTERIA_RUNTIME_VARIABLE_HPP_
#define ASTERIA_RUNTIME_VARIABLE_HPP_

#include "../fwd.hpp"
#include "refcounted_base.hpp"
#include "value.hpp"

namespace Asteria {

class Variable : public Refcounted_Base
  {
  private:
    // These are only used during garbage collection and are uninitialized by default.
    mutable long m_gcref_intg;
    mutable double m_gcref_mant;

    Value m_value;
    bool m_immutable;

  public:
    template<typename XvalueT,
             ROCKET_ENABLE_IF(std::is_constructible<Value, XvalueT &&>::value)>
      Variable(XvalueT &&value, bool immutable)
      : m_value(std::forward<XvalueT>(value)), m_immutable(immutable)
      {
      }
    ROCKET_NONCOPYABLE_DESTRUCTOR(Variable);

  private:
    [[noreturn]] void do_throw_immutable() const;

  public:
    long get_gcref() const noexcept
      {
        return this->m_gcref_intg;
      }
    void init_gcref(long intg) const noexcept
      {
        this->m_gcref_intg = intg;
        this->m_gcref_mant = 1e-9;
      }
    void add_gcref(int dintg) const noexcept
      {
        this->m_gcref_intg += dintg;
      }
    void add_gcref(double dmant) const noexcept
      {
        this->m_gcref_mant += dmant;
        // Add with carry.
        const auto carry = static_cast<long>(this->m_gcref_mant);
        this->m_gcref_intg += carry;
        this->m_gcref_mant -= static_cast<double>(carry);
      }

    const Value & get_value() const noexcept
      {
        return this->m_value;
      }
    Value & open_value() noexcept
      {
        if(this->m_immutable) {
          this->do_throw_immutable();
        }
        return this->m_value;
      }
    bool is_immutable() const noexcept
      {
        return this->m_immutable;
      }
    template<typename XvalueT,
             ROCKET_ENABLE_IF(std::is_assignable<Value, XvalueT &&>::value)>
      void set_value(XvalueT &&value)
      {
        if(this->m_immutable) {
          this->do_throw_immutable();
        }
        this->m_value = std::forward<XvalueT>(value);
      }
    template<typename XvalueT,
             ROCKET_ENABLE_IF(std::is_assignable<Value, XvalueT &&>::value)>
      void reset(XvalueT &&value, bool immutable)
      {
        this->m_value = std::forward<XvalueT>(value);
        this->m_immutable = immutable;
      }

    void enumerate_variables(const Abstract_Variable_Callback &callback) const
      {
        this->m_value.enumerate_variables(callback);
      }
  };

}

#endif

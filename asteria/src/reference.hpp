// This file is part of Asteria.
// Copyleft 2018, LH_Mouse. All wrongs reserved.

#ifndef ASTERIA_REFERENCE_HPP_
#define ASTERIA_REFERENCE_HPP_

#include "fwd.hpp"
#include "reference_root.hpp"
#include "reference_modifier.hpp"

namespace Asteria
{

class Reference
  {
  private:
    Reference_root m_root;
    Vector<Reference_modifier> m_modifiers;

  public:
    Reference() noexcept
      : m_root(), m_modifiers()
      {
      }
    template<typename RootT, typename std::enable_if<std::is_constructible<Reference_root, RootT &&>::value>::type * = nullptr>
    Reference(RootT &&root, Vector<Reference_modifier> modifiers = { })
      : m_root(std::forward<RootT>(root)), m_modifiers(std::move(modifiers))
      {
      }
    Reference(const Reference &) noexcept;
    Reference & operator=(const Reference &) noexcept;
    Reference(Reference &&) noexcept;
    Reference & operator=(Reference &&) noexcept;
    ~Reference();

  public:
    const Reference_root & get_root() const noexcept
      {
        return m_root;
      }
    Reference_root & get_root() noexcept
      {
        return m_root;
      }
    const Vector<Reference_modifier> & get_modifiers() const noexcept
      {
        return m_modifiers;
      }
    Vector<Reference_modifier> & get_modifiers() noexcept
      {
        return m_modifiers;
      }
    Reference_root & set_root(Reference_root root, Vector<Reference_modifier> modifiers = { })
      {
        m_root = std::move(root);
        m_modifiers = std::move(modifiers);
        return m_root;
      }
    bool has_modifiers() const noexcept
      {
        return m_modifiers.empty() == false;
      }
    void clear_modifiers() noexcept
      {
        m_modifiers.clear();
      }
    Reference_modifier & push_modifier(Reference_modifier modifier)
      {
        return m_modifiers.emplace_back(std::move(modifier));
      }
    void pop_modifier()
      {
        m_modifiers.pop_back();
      }
  };

extern Value read_reference(const Reference &ref);
extern void write_reference(const Reference &ref, Value value);
extern Value unset_reference(const Reference &ref);

extern Reference reference_constant(Value value);
extern Reference reference_temp_value(Value value);
extern void materialize_reference(Reference &ref);
extern Reference indirect_reference_from(const Reference &parent, Reference_modifier modifier);

}

#endif

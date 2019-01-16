// This file is part of Asteria.
// Copyleft 2018 - 2019, LH_Mouse. All wrongs reserved.

#include "../precompiled.hpp"
#include "instantiated_function.hpp"
#include "reference.hpp"
#include "executive_context.hpp"
#include "../syntax/statement.hpp"
#include "../utilities.hpp"

namespace Asteria {

Instantiated_Function::~Instantiated_Function()
  {
  }

rocket::cow_string Instantiated_Function::describe() const
  {
    return ASTERIA_FORMAT_STRING("function `", this->m_zvarg.get().get_name(), "()` defined at \'", this->m_zvarg.get().get_location(), "\'");
  }

void Instantiated_Function::invoke(Reference &self_io, Global_Context &global, rocket::cow_vector<Reference> &&args) const
  {
    this->m_body_bnd.execute_as_function(self_io, global, this->m_zvarg, this->m_params, std::move(args));
  }

void Instantiated_Function::enumerate_variables(const Abstract_Variable_Callback &callback) const
  {
    this->m_body_bnd.enumerate_variables(callback);
  }

}

// This file is part of asteria.
// Copyleft 2018, LH_Mouse. All wrongs reserved.

#include "precompiled.hpp"
#include "expression.hpp"
#include "variable.hpp"
#include "reference.hpp"
#include "misc.hpp"

namespace Asteria {

Expression::~Expression(){
	//
}

Reference Expression::evaluate(Value_ptr<Scope> &scope) const {
	ASTERIA_DEBUG_LOG("NOT IMPLEMENTED YET");
	(void)scope;
	Reference::Direct_reference ref = { std::make_shared<Variable>(std::string("hello")) };
	return Reference(std::move(ref));
}

}

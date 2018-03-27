// This file is part of asteria.
// Copyleft 2018, LH_Mouse. All wrongs reserved.

#ifndef ASTERIA_FWD_HPP_
#define ASTERIA_FWD_HPP_

#include <boost/container/vector.hpp>
#include <boost/container/deque.hpp>
#include <boost/container/flat_map.hpp>
#include <iosfwd> // std::ostream
#include <string> // std::string
#include <functional> // std::function
#include <type_traits> // std::enable_if, std::decay, std::is_base_of
#include <utility> // std::move, std::pair
#include <cstddef> // std::nullptr_t
#include <cstdint> // std::int64_t
#include "value_ptr.hpp"
#include "observer_ptr.hpp"

namespace Asteria {

template<typename ...TypesT>
struct Type_tuple;

class Insertable_streambuf;
class Insertable_ostream;
class Logger;

class Variable;
class Reference;

class Reference;
class Initializer;
class Expression;
class Scope;
class Statement;

template<typename ElementT>
using Value_ptr_vector = boost::container::vector<Value_ptr<ElementT>>;
template<typename ElementT>
using Value_ptr_deque = boost::container::deque<Value_ptr<ElementT>>;
template<typename KeyT, typename ValueT>
using Value_ptr_map = boost::container::flat_map<KeyT, Value_ptr<ValueT>>;

using Null      = std::nullptr_t;
using Boolean   = bool;
using Integer   = std::int64_t;
using Double    = double;
using String    = std::string;
using Opaque    = std::pair<std::string, std::shared_ptr<void>>;
using Array     = Value_ptr_deque<Variable>;
using Object    = Value_ptr_map<std::string, Variable>;
using Function  = std::function<Asteria::Value_ptr<Variable> (boost::container::vector<Reference> &&)>;

}

// These are instantiated in 'variable.cpp'.
extern template class boost::container::deque<Asteria::Value_ptr<Asteria::Variable>>;
extern template class boost::container::flat_map<std::string, Asteria::Value_ptr<Asteria::Variable>>;
extern template class std::function<Asteria::Value_ptr<Asteria::Variable> (Asteria::Array &&)>;

#define ASTERIA_UNLESS_IS_BASE_OF(Base_, ParamT_)	\
	typename ::std::enable_if<	\
		::std::is_base_of<Base_, typename ::std::decay<ParamT_>::type>::value == false	\
		>::type * = nullptr

#endif

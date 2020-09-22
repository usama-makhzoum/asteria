// This file is part of Asteria.
// Copyleft 2018 - 2020, LH_Mouse. All wrongs reserved.

#ifndef ASTERIA_VALUE_HPP_
#  error Please include <asteria/value.hpp> instead.
#endif

namespace asteria {
namespace details_value {

// This controls implicit conversions to `Value` from other types.
// The main templte is undefined and is SFINAE-friendly.
// A member type alias `via_type` shall be provided, which shall designate one of
// the `V_*` candidates. If `nullable` is `false_type`, `ValT` shall be convertible
// to `via_type`.
template<typename ValT, typename = void>
struct Valuable_impl;

template<>
struct Valuable_impl<nullptr_t>
  {
    using via_type  = V_null;
    using nullable  = ::std::false_type;

    template<typename StorT>
    static
    void
    assign(StorT& stor, nullptr_t)
      {
        stor = V_null();
      }
  };

template<>
struct Valuable_impl<bool>
  {
    using via_type  = V_boolean;
    using nullable  = ::std::false_type;

    template<typename StorT, typename XValT>
    static
    void
    assign(StorT& stor, XValT&& xval)
      {
        stor = V_boolean(xval);
      }
  };

template<typename IntegerT>
struct Valuable_impl<IntegerT, typename ::std::enable_if<
            ::rocket::is_any_type_of<IntegerT,
                        signed char, short, int, long, long long>::value
        >::type>
  {
    using via_type  = V_integer;
    using nullable  = ::std::false_type;

    template<typename StorT, typename XValT>
    static
    void
    assign(StorT& stor, XValT&& xval)
      {
        stor = V_integer(xval);
      }
  };

template<typename FloatT>
struct Valuable_impl<FloatT, typename ::std::enable_if<
            ::rocket::is_any_type_of<FloatT,
                                 float, double>::value
        >::type>
  {
    using via_type  = V_real;
    using nullable  = ::std::false_type;

    template<typename StorT, typename XValT>
    static
    void
    assign(StorT& stor, XValT&& xval)
      {
        stor = V_real(xval);
      }
  };

template<typename StringT>
struct Valuable_impl<StringT, typename ::std::enable_if<
            ::rocket::is_any_type_of<StringT,
                        cow_string::shallow_type, cow_string>::value
        >::type>
  {
    using via_type  = V_string;
    using nullable  = ::std::false_type;

    template<typename StorT, typename XValT>
    static
    void
    assign(StorT& stor, XValT&& xval)
      {
        stor = V_string(::std::forward<XValT>(xval));
      }
  };

template<>
struct Valuable_impl<cow_opaque>
  {
    using via_type  = V_opaque;
    using nullable  = ::std::true_type;

    template<typename StorT, typename XValT>
    static
    void
    assign(StorT& stor, XValT&& xval)
      {
        if(xval)
          stor = V_opaque(::std::forward<XValT>(xval));
        else
          stor = V_null();
      }
  };

template<typename OpaqueT>
struct Valuable_impl<rcptr<OpaqueT>, typename ::std::enable_if<
            ::std::is_convertible<OpaqueT*,
                                  Abstract_Opaque*>::value
        >::type>
  {
    using via_type  = V_opaque;
    using nullable  = ::std::true_type;

    template<typename StorT, typename XValT>
    static
    void
    assign(StorT& stor, XValT&& xval)
      {
        if(xval)
          stor = V_opaque(::std::forward<XValT>(xval));
        else
          stor = V_null();
      }
  };

template<>
struct Valuable_impl<cow_function>
  {
    using via_type  = V_function;
    using nullable  = ::std::true_type;

    template<typename StorT, typename XValT>
    static
    void
    assign(StorT& stor, XValT&& xval)
      {
        if(xval)
          stor = V_function(::std::forward<XValT>(xval));
        else
          stor = V_null();
      }
  };

template<typename FunctionT>
struct Valuable_impl<rcptr<FunctionT>, typename ::std::enable_if<
            ::std::is_convertible<FunctionT*,
                                  const Abstract_Function*>::value
        >::type>
  {
    using via_type  = V_function;
    using nullable  = ::std::true_type;

    template<typename StorT, typename XValT>
    static
    void
    assign(StorT& stor, XValT&& xval)
      {
        if(xval)
          stor = V_function(::std::forward<XValT>(xval));
        else
          stor = V_null();
      }
  };

template<>
struct Valuable_impl<cow_vector<Value>>
  {
    using via_type  = V_array;
    using nullable  = ::std::false_type;

    template<typename StorT, typename XValT>
    static
    void
    assign(StorT& stor, XValT&& xval)
      {
        stor = V_array(::std::forward<XValT>(xval));
      }
  };

template<>
struct Valuable_impl<cow_dictionary<Value>>
  {
    using via_type  = V_object;
    using nullable  = ::std::false_type;

    template<typename StorT, typename XValT>
    static
    void
    assign(StorT& stor, XValT&& xval)
      {
        stor = V_object(::std::forward<XValT>(xval));
      }
  };

template<typename XValT, size_t N>
struct Valuable_impl<XValT [N]>
  {
    using via_type  = V_array;
    using nullable  = ::std::true_type;  // XXX: not really nullable

    template<typename StorT, typename XArrT>
    static
    void
    assign(StorT& stor, XArrT&& xarr)
      {
        V_array arr;
        arr.reserve(N);
        for(size_t k = 0;  k != N;  ++k)
          arr.emplace_back(static_cast<typename ::std::conditional<
                       ::std::is_lvalue_reference<XArrT&&>::value,
                                       const XValT&, XValT&&>::type>(xarr[k]));
        stor = ::std::move(arr);
      }
  };

template<typename TupleT>
struct Valuable_impl<TupleT, typename ::std::conditional<
               bool(::std::tuple_size<TupleT>::value), void, void
        >::type>
  {
    using via_type  = V_array;
    using nullable  = ::std::true_type;  // XXX: not really nullable

    template<size_t... N, typename XTupT>
    static
    void
    unpack_tuple_aux(V_array& arr, ::std::index_sequence<N...>, XTupT&& xtup)
      {
        int dummy[] = { (static_cast<void>(arr.emplace_back(
                               ::std::get<N>(::std::forward<XTupT>(xtup)))), 1)...  };
        (void)dummy;
      }

    template<typename StorT, typename XTupT>
    static
    void
    assign(StorT& stor, XTupT&& xtup)
      {
        V_array arr;
        constexpr size_t N = ::std::tuple_size<TupleT>::value;
        arr.reserve(N);
        unpack_tuple_aux(arr, ::std::make_index_sequence<N>(),
                                               ::std::forward<XTupT>(xtup));
        stor = ::std::move(arr);
      }
  };

template<typename XValT>
struct Valuable_impl<opt<XValT>, typename ::std::conditional<
               true, void, typename Valuable_impl<XValT>::via_type
        >::type>
  {
    using via_type  = typename Valuable_impl<XValT>::via_type;
    using nullable  = ::std::true_type;

    template<typename StorT, typename XOptT>
    static
    void
    assign(StorT& stor, XOptT&& xopt)
      {
        if(xopt)
          Valuable_impl<XValT>::assign(stor,
                  static_cast<typename ::std::conditional<
                                 ::std::is_lvalue_reference<XOptT&&>::value,
                                                const XValT&, XValT&&>::type>(*xopt));
        else
          stor = V_null();
      }
  };

template<typename XValT>
using Valuable = Valuable_impl<typename ::rocket::remove_cvref<XValT>::type, void>;

}  // namespace details_value
}  // namespace asteria

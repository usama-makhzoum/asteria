// This file is part of Asteria.
// Copyleft 2018, LH_Mouse. All wrongs reserved.

#include "precompiled.hpp"
#include "value.hpp"
#include "opaque_base.hpp"
#include "function_base.hpp"
#include "utilities.hpp"

namespace Asteria
{

Value::Value(const Value &) noexcept = default;
Value & Value::operator=(const Value &) noexcept = default;
Value::Value(Value &&) noexcept = default;
Value & Value::operator=(Value &&) noexcept = default;
Value::~Value() = default;

const char * get_type_name(Value::Type type) noexcept
  {
    switch(type) {
    case Value::type_null:
      return "null";
    case Value::type_boolean:
      return "boolean";
    case Value::type_integer:
      return "integer";
    case Value::type_double:
      return "double";
    case Value::type_string:
      return "string";
    case Value::type_opaque:
      return "opaque";
    case Value::type_function:
      return "function";
    case Value::type_array:
      return "array";
    case Value::type_object:
      return "object";
    default:
      ASTERIA_TERMINATE("An unknown value type enumeration `", type, "` is encountered.");
    }
  }

bool test_value(const Value &value)
  {
    switch(value.which()) {
    case Value::type_null:
      return false;
    case Value::type_boolean:
      {
        const auto &cand = value.as<D_boolean>();
        return cand;
      }
    case Value::type_integer:
      {
        const auto &cand = value.as<D_integer>();
        return cand != 0;
      }
    case Value::type_double:
      {
        const auto &cand = value.as<D_double>();
        return std::fpclassify(cand) != FP_ZERO;
      }
    case Value::type_string:
      {
        const auto &cand = value.as<D_string>();
        return cand.empty() == false;
      }
    case Value::type_opaque:
    case Value::type_function:
      return true;
    case Value::type_array:
      {
        const auto &cand = value.as<D_array>();
        return cand.empty() == false;
      }
    case Value::type_object:
      {
        const auto &cand = value.as<D_object>();
        return cand.empty() == false;
      }
    default:
      ASTERIA_TERMINATE("An unknown value type enumeration `", value.which(), "` is encountered.");
    }
  }
Value::Comparison_result compare_values(const Value &lhs, const Value &rhs) noexcept
  {
    // `null` is considered to be equal to `null` and less than anything else.
    if(lhs.which() != rhs.which()) {
      if(lhs.which() == Value::type_null) {
        return Value::comparison_less;
      }
      if(rhs.which() == Value::type_null) {
        return Value::comparison_greater;
      }
      return Value::comparison_unordered;
    }
    // If both operands have the same type, perform normal comparison.
    switch(lhs.which()) {
    case Value::type_null:
      return Value::comparison_equal;
    case Value::type_boolean:
      {
        const auto &cand_lhs = lhs.as<D_boolean>();
        const auto &cand_rhs = rhs.as<D_boolean>();
        if(cand_lhs < cand_rhs) {
          return Value::comparison_less;
        }
        if(cand_lhs > cand_rhs) {
          return Value::comparison_greater;
        }
        return Value::comparison_equal;
      }
    case Value::type_integer:
      {
        const auto &cand_lhs = lhs.as<D_integer>();
        const auto &cand_rhs = rhs.as<D_integer>();
        if(cand_lhs < cand_rhs) {
          return Value::comparison_less;
        }
        if(cand_lhs > cand_rhs) {
          return Value::comparison_greater;
        }
        return Value::comparison_equal;
      }
    case Value::type_double:
      {
        const auto &cand_lhs = lhs.as<D_double>();
        const auto &cand_rhs = rhs.as<D_double>();
        if(std::isunordered(cand_lhs, cand_rhs)) {
          return Value::comparison_unordered;
        }
        if(std::isless(cand_lhs, cand_rhs)) {
          return Value::comparison_less;
        }
        if(std::isgreater(cand_lhs, cand_rhs)) {
          return Value::comparison_greater;
        }
        return Value::comparison_equal;
      }
    case Value::type_string:
      {
        const auto &cand_lhs = lhs.as<D_string>();
        const auto &cand_rhs = rhs.as<D_string>();
        const int cmp = cand_lhs.compare(cand_rhs);
        if(cmp < 0) {
          return Value::comparison_less;
        }
        if(cmp > 0) {
          return Value::comparison_greater;
        }
        return Value::comparison_equal;
      }
    case Value::type_opaque:
    case Value::type_function:
      return Value::comparison_unordered;
    case Value::type_array:
      {
        const auto &array_lhs = lhs.as<D_array>();
        const auto &array_rhs = rhs.as<D_array>();
        const auto rlen = std::min(array_lhs.size(), array_rhs.size());
        for(std::size_t i = 0; i < rlen; ++i) {
          const auto res = compare_values(array_lhs[i], array_rhs[i]);
          if(res != Value::comparison_equal) {
            return res;
          }
        }
        if(array_lhs.size() < array_rhs.size()) {
          return Value::comparison_less;
        }
        if(array_lhs.size() > array_rhs.size()) {
          return Value::comparison_greater;
        }
        return Value::comparison_equal;
      }
    case Value::type_object:
      return Value::comparison_unordered;
    default:
      ASTERIA_TERMINATE("An unknown value type enumeration `", lhs.which(), "` is encountered.");
    }
  }

namespace
  {
    class Indent
      {
      private:
        unsigned m_num;

      public:
        explicit constexpr Indent(unsigned xnum) noexcept
          : m_num(xnum)
          {
          }

      public:
        unsigned num() const noexcept
          {
            return m_num;
          }
      };

    inline std::ostream & operator<<(std::ostream &os, const Indent &indent)
      {
        const std::ostream::sentry sentry(os);
        if(!sentry) {
          return os;
        }
        try {
          using traits_type = std::ostream::traits_type;
          const auto num = indent.num();
          for(unsigned i = 0; i < num; ++i) {
            if(traits_type::eq_int_type(os.rdbuf()->sputc(' '), traits_type::eof())) {
              os.setstate(std::ios_base::failbit);
              goto finish;
            }
          }
      finish:
          os.width(0);
        } catch(...) {
          // XXX: Relying on a private function is evil.
          rocket::details_cow_string::handle_io_exception(os);
        }
        return os;
      }

    class Quote
      {
      private:
        String m_str;

      public:
        explicit Quote(String xstr) noexcept
          : m_str(std::move(xstr))
          {
          }

      public:
        const String & str() const noexcept
          {
            return m_str;
          }
      };

    inline std::ostream & operator<<(std::ostream &os, const Quote &quote)
      {
        const std::ostream::sentry sentry(os);
        if(!sentry) {
          return os;
        }
        try {
          using traits_type = std::ostream::traits_type;
          const auto range = std::make_pair(quote.str().begin(), quote.str().end());
          if(traits_type::eq_int_type(os.rdbuf()->sputc('\"'), traits_type::eof())) {
            os.setstate(std::ios_base::failbit);
            goto finish;
          }
          for(auto it = range.first; it != range.second; ++it) {
            std::streamsize n_wr;
            const int ch = *it & 0xFF;
            switch(ch) {
            case '\"':
              n_wr = os.rdbuf()->sputn("\\\"", 2);
              break;
            case '\'':
              n_wr = os.rdbuf()->sputn("\\\'", 2);
              break;
            case '\\':
              n_wr = os.rdbuf()->sputn("\\\\", 2);
              break;
            case '\a':
              n_wr = os.rdbuf()->sputn("\\a", 2);
              break;
            case '\b':
              n_wr = os.rdbuf()->sputn("\\b", 2);
              break;
            case '\f':
              n_wr = os.rdbuf()->sputn("\\f", 2);
              break;
            case '\n':
              n_wr = os.rdbuf()->sputn("\\n", 2);
              break;
            case '\r':
              n_wr = os.rdbuf()->sputn("\\r", 2);
              break;
            case '\t':
              n_wr = os.rdbuf()->sputn("\\t", 2);
              break;
            case '\v':
              n_wr = os.rdbuf()->sputn("\\v", 2);
              break;
            default:
              if((0x20 <= ch) && (ch <= 0x7E)) {
                const bool failed = traits_type::eq_int_type(os.rdbuf()->sputc(static_cast<char>(ch)), traits_type::eof());
                n_wr = failed ? 0 : 1;
              } else {
                static constexpr char s_hex_table[] = "0123456789ABCDEF";
                char temp[4] = "\\x";
                temp[2] = s_hex_table[(ch >> 4) & 0x0F];
                temp[3] = s_hex_table[(ch >> 0) & 0x0F];
                n_wr = os.rdbuf()->sputn(temp, 4);
              }
              break;
            }
            if(n_wr == 0) {
              os.setstate(std::ios_base::failbit);
              goto finish;
            }
          }
          if(traits_type::eq_int_type(os.rdbuf()->sputc('\"'), traits_type::eof())) {
            os.setstate(std::ios_base::failbit);
            goto finish;
          }
      finish:
          os.width(0);
        } catch(...) {
          // XXX: Relying on a private function is evil.
          rocket::details_cow_string::handle_io_exception(os);
        }
        return os;
      }
  }

void dump_value(std::ostream &os, const Value &value, unsigned indent_next, unsigned indent_increment)
  {
    const auto type = value.which();
    switch(type) {
    case Value::type_null:
      // null
      os <<"null";
      return;
    case Value::type_boolean:
      {
        const auto &cand = value.as<D_boolean>();
        // boolean true
        os <<"boolean " <<std::boolalpha <<std::nouppercase <<cand;
        return;
      }
    case Value::type_integer:
      {
        const auto &cand = value.as<D_integer>();
        // integer 42
        os <<"integer " <<std::dec <<cand;
        return;
      }
    case Value::type_double:
      {
        const auto &cand = value.as<D_double>();
        // double 123.456
        os <<"double " <<std::dec <<std::nouppercase <<std::setprecision(std::numeric_limits<D_double>::max_digits10) <<cand;
        return;
      }
    case Value::type_string:
      {
        const auto &cand = value.as<D_string>();
        // string(5) "hello"
        os <<"string(" <<std::dec <<cand.size() <<") " <<Quote(cand);
        return;
      }
    case Value::type_opaque:
      {
        const auto &cand = value.as<D_opaque>();
        // opaque("typeid") "my opaque"
        os <<"opaque(\"" <<typeid(*cand).name() <<"\") " <<Quote(cand->describe());
        return;
      }
    case Value::type_function:
      {
        const auto &cand = value.as<D_function>();
        // function("typeid") "my function"
        os <<"function(\"" <<typeid(*cand).name() <<"\") " <<Quote(cand->describe());
        return;
      }
    case Value::type_array:
      {
        const auto &cand = value.as<D_array>();
        // array(3) = [
        //   0 = integer 1,
        //   1 = integer 2,
        //   2 = integer 3,
        // ]
        os <<"array(" <<std::dec <<cand.size() <<") [";
        for(auto it = cand.begin(); it != cand.end(); ++it) {
          os <<std::endl <<Indent(indent_next + indent_increment);
          os <<std::dec <<(it - cand.begin());
          os <<" = ";
          dump_value(os, *it, indent_next + indent_increment, indent_increment);
          os <<',';
        }
        os <<std::endl <<Indent(indent_next) <<']';
        return;
      }
    case Value::type_object:
      {
        const auto &cand = value.as<D_object>();
        // object(3) = {
        //   "one" = integer 1,
        //   "two" = integer 2,
        //   "three" = integer 3,
        // }
        os <<"object(" <<std::dec <<cand.size() <<") {";
        for(auto it = cand.begin(); it != cand.end(); ++it) {
          os <<std::endl <<Indent(indent_next + indent_increment);
          os <<Quote(it->first);
          os <<" = ";
          dump_value(os, it->second, indent_next + indent_increment, indent_increment);
          os <<',';
        }
        os <<std::endl <<Indent(indent_next) <<'}';
        return;
      }
    default:
      ASTERIA_TERMINATE("An unknown value type enumeration `", type, "` is encountered.");
    }
  }
std::ostream & operator<<(std::ostream &os, const Value &value)
  {
    dump_value(os, value);
    return os;
  }

}

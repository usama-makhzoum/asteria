// This file is part of Asteria.
// Copyleft 2018 - 2021, LH_Mouse. All wrongs reserved.

#include "../precompiled.hpp"
#include "debug.hpp"
#include "../runtime/argument_reader.hpp"
#include "../utils.hpp"

namespace asteria {
namespace {

Opt_integer
do_write_stderr_common(::rocket::tinyfmt_str&& fmt)
  {
    // Try writing standard output. Errors are ignored.
    auto nput = write_log_to_stderr(__FILE__, __LINE__, fmt.extract_string());
    if(nput < 0)
      return nullopt;

    // Return the number of bytes that have been written.
    return static_cast<int64_t>(nput);
  }

}  // namespace

Opt_integer
std_debug_logf(V_string templ, cow_vector<Value> values)
  {
    // Prepare inserters.
    cow_vector<::rocket::formatter> insts;
    insts.reserve(values.size());
    for(size_t i = 0;  i < values.size();  ++i)
      insts.push_back({
        [](tinyfmt& fmt, const void* ptr) -> tinyfmt&
          { return static_cast<const Value*>(ptr)->print(fmt);  },
        values.data() + i
      });

    // Compose the string into a stream.
    ::rocket::tinyfmt_str fmt;
    vformat(fmt, templ.data(), templ.size(), insts.data(), insts.size());
    return do_write_stderr_common(::std::move(fmt));
  }

Opt_integer
std_debug_dump(Value value, Opt_integer indent)
  {
    // Clamp the suggested indent so we don't produce overlong lines.
    size_t rindent = static_cast<size_t>(::rocket::clamp(indent.value_or(2), 0, 10));

    // Format the value.
    ::rocket::tinyfmt_str fmt;
    value.dump(fmt, rindent);
    return do_write_stderr_common(::std::move(fmt));
  }

void
create_bindings_debug(V_object& result, API_Version /*version*/)
  {
    result.insert_or_assign(sref("logf"),
      ASTERIA_BINDING_BEGIN("std.debug.logf", self, global, reader) {
        V_string templ;
        cow_vector<Value> values;

        reader.start_overload();
        reader.required(templ);          // template
        if(reader.end_overload(values))  // ...
          ASTERIA_BINDING_RETURN_MOVE(self,
                    std_debug_logf, templ, values);
      }
      ASTERIA_BINDING_END);

    result.insert_or_assign(sref("dump"),
      ASTERIA_BINDING_BEGIN("std.debug.dump", self, global, reader) {
        Value value;
        Opt_integer indent;

        reader.start_overload();
        reader.optional(value);     // [value]
        reader.optional(indent);    // [indent]
        if(reader.end_overload())
          ASTERIA_BINDING_RETURN_MOVE(self,
                    std_debug_dump, value, indent);
      }
      ASTERIA_BINDING_END);
  }

}  // namespace asteria

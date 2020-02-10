// This file is part of Asteria.
// Copyleft 2018 - 2019, LH_Mouse. All wrongs reserved.

#ifndef ASTERIA_LIBRARY_BINDINGS_PROCESS_HPP_
#define ASTERIA_LIBRARY_BINDINGS_PROCESS_HPP_

#include "../fwd.hpp"

namespace Asteria {

Ival std_process_execute(Sval path, Aopt argv = nullopt, Aopt envp = nullopt);
void std_process_daemonize();

// Create an object that is to be referenced as `std.process`.
void create_bindings_process(Oval& result, API_Version version);

}  // namespace Asteria

#endif
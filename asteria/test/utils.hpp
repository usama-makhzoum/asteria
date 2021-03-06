// This file is part of Asteria.
// Copyleft 2018 - 2021, LH_Mouse. All wrongs reserved.

#ifndef ASTERIA_TEST_UTILS_HPP_
#define ASTERIA_TEST_UTILS_HPP_

#include "../src/fwd.hpp"
#include "../src/utils.hpp"
#include <unistd.h>   // ::alarm()

#define ASTERIA_TEST_CHECK(expr)  \
    do  \
      try {  \
        if(static_cast<bool>(expr) == false) {  \
          /* failed */  \
          ::asteria::write_log_to_stderr(__FILE__, __LINE__,  \
              ::asteria::format_string(  \
                 "ASTERIA_TEST_CHECK FAIL: $1",  \
                 #expr  \
              ));  \
          ::abort();  \
          break;  \
        }  \
        \
        /* successful */  \
        ::asteria::write_log_to_stderr(__FILE__, __LINE__,  \
            ::asteria::format_string(  \
               "ASTERIA_TEST_CHECK PASS: $1",  \
               #expr  \
            ));  \
      }  \
      catch(exception& stdex) {  \
        /* failed */  \
        ::asteria::write_log_to_stderr(__FILE__, __LINE__,  \
            ::asteria::format_string(  \
               "ASTERIA_TEST_CHECK EXCEPTION: $1\n$2",  \
               #expr, stdex  \
            ));  \
        ::abort();  \
      }  \
    while(false)

#define ASTERIA_TEST_CHECK_CATCH(expr)  \
    do  \
      try {  \
        static_cast<void>(expr);  \
        \
        /* failed */  \
        ::asteria::write_log_to_stderr(__FILE__, __LINE__,  \
            ::asteria::format_string(  \
               "ASTERIA_TEST_CHECK_CATCH XPASS: $1",  \
               #expr  \
            ));  \
        ::abort();  \
      }  \
      catch(exception& stdex) {  \
        /* successful */  \
        ::asteria::write_log_to_stderr(__FILE__, __LINE__,  \
            ::asteria::format_string(  \
               "ASTERIA_TEST_CHECK XFAIL: $1",  \
               #expr  \
            ));  \
      }  \
    while(false)

// Set terminate handler.
static const auto asteria_test_terminate = ::std::set_terminate(
    [] {
      auto eptr = ::std::current_exception();
      if(eptr) {
        try {
          ::std::rethrow_exception(eptr);
        }
        catch(::std::exception& stdex) {
          ::fprintf(stderr,
              "`::std::terminate()` called after `::std::exception`:\n%s\n",
              stdex.what());
        }
        catch(...) {
          ::fprintf(stderr,
              "`::std::terminate()` called after an unknown exception\n");
        }
      }
      else
        ::fprintf(stderr,
            "`::std::terminate()` called without an exception\n");

      ::fflush(nullptr);
      ::_Exit(1);
    });

// Set kill timer.
static const auto asteria_test_alarm = ::alarm(30);

#endif

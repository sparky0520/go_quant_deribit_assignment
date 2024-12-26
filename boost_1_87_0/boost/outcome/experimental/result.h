/* C interface for result
(C) 2017-2024 Niall Douglas <http://www.nedproductions.biz/> (6 commits)
File Created: Aug 2017


Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#ifndef BOOST_OUTCOME_EXPERIMENTAL_RESULT_H
#define BOOST_OUTCOME_EXPERIMENTAL_RESULT_H

#include <assert.h>
#include <stddef.h>  // for size_t
#include <stdint.h>  // for intptr_t

#include "../detail/try.h"

#ifndef BOOST_OUTCOME_C_WEAK
#ifdef _MSC_VER
#define BOOST_OUTCOME_C_WEAK inline
#else
#define BOOST_OUTCOME_C_WEAK __attribute__((weak))
#endif
#endif

#ifndef BOOST_OUTCOME_C_MSVC_FORCE_EMIT
#ifdef _MSC_VER
#ifdef __cplusplus
#define BOOST_OUTCOME_C_MSVC_FORCE_EMIT(x) extern "C" __declspec(selectany) void *x##_emit = x;
#else
#define BOOST_OUTCOME_C_MSVC_FORCE_EMIT(x) extern __declspec(selectany) void *x##_emit = x;
#endif
#else
#define BOOST_OUTCOME_C_MSVC_FORCE_EMIT(x)
#endif
#endif


#ifndef BOOST_OUTCOME_C_NODISCARD
#if __STDC_VERSION__ >= 202000L || __cplusplus >= 201700L
#define BOOST_OUTCOME_C_NODISCARD [[nodiscard]]
#ifdef __cplusplus
#define BOOST_OUTCOME_C_NODISCARD_EXTERN_C extern "C" [[nodiscard]]
#else
#define BOOST_OUTCOME_C_NODISCARD_EXTERN_C [[nodiscard]] extern
#endif
#elif defined(__GNUC__) || defined(__clang__)
#define BOOST_OUTCOME_C_NODISCARD __attribute__((warn_unused_result))
#ifdef __cplusplus
#define BOOST_OUTCOME_C_NODISCARD_EXTERN_C extern "C" __attribute__((warn_unused_result))
#else
#define BOOST_OUTCOME_C_NODISCARD_EXTERN_C extern __attribute__((warn_unused_result))
#endif
#else
#define BOOST_OUTCOME_C_NODISCARD
#define BOOST_OUTCOME_C_NODISCARD_EXTERN_C extern
#endif
#endif

#ifndef BOOST_OUTCOME_C_INLINE
#if __STDC_VERSION__ >= 199900L || __cplusplus > 0
#define BOOST_OUTCOME_C_INLINE inline
#elif defined(__GNUC__) || defined(__clang__)
#define BOOST_OUTCOME_C_INLINE __inline
#endif
#endif

#include "../outcome_gdb.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define BOOST_OUTCOME_C_DECLARE_RESULT(ident, R, S)                                                                                                                        \
  struct cxx_result_##ident                                                                                                                                    \
  {                                                                                                                                                            \
    union                                                                                                                                                      \
    {                                                                                                                                                          \
      R value;                                                                                                                                                 \
      S error;                                                                                                                                                 \
    };                                                                                                                                                         \
    unsigned flags;                                                                                                                                            \
  }

#define BOOST_OUTCOME_C_RESULT(ident) struct cxx_result_##ident


#define BOOST_OUTCOME_C_RESULT_HAS_VALUE(r) (((r).flags & 1U) == 1U)

#define BOOST_OUTCOME_C_RESULT_HAS_ERROR(r) (((r).flags & 2U) == 2U)

#define BOOST_OUTCOME_C_RESULT_ERROR_IS_ERRNO(r) (((r).flags & (1U << 4U)) == (1U << 4U))

  /***************************** <system_error2> support ******************************/

#define BOOST_OUTCOME_C_DECLARE_STATUS_CODE(ident, value_type)                                                                                                             \
  struct cxx_status_code_##ident                                                                                                                               \
  {                                                                                                                                                            \
    void *domain;                                                                                                                                              \
    value_type value;                                                                                                                                          \
  };

#define BOOST_OUTCOME_C_STATUS_CODE(ident) struct cxx_status_code_##ident

  extern BOOST_OUTCOME_C_WEAK void outcome_make_result_status_code_failure_posix(void *out, size_t bytes, size_t offset, int errcode);
  extern BOOST_OUTCOME_C_WEAK void outcome_make_result_status_code_failure_system(void *out, size_t bytes, size_t offset, intptr_t errcode);
  extern int outcome_status_code_equal(const void *a, const void *b);
  extern int outcome_status_code_equal_generic(const void *a, int errcode);
  extern const char *outcome_status_code_message(const void *a);


#ifdef __cplusplus
#define BOOST_OUTCOME_C_DECLARE_RESULT_STATUS_CODE_CXX(ident, R, S)                                                                                                        \
  static_assert(std::is_trivially_copyable<R>::value || BOOST_OUTCOME_V2_NAMESPACE::trait::is_move_bitcopying<R>::value,                                             \
                "R must be trivially copyable or move bitcopying to be used in a C Result");                                                                   \
  static_assert(std::is_trivially_copyable<S>::value || BOOST_OUTCOME_V2_NAMESPACE::trait::is_move_bitcopying<S>::value,                                             \
                "S must be trivially copyable or move bitcopying to be used in a C Result");                                                                   \
  inline BOOST_OUTCOME_V2_NAMESPACE::experimental::status_result<R> to_result(const cxx_result_status_code_##ident &v)                                               \
  {                                                                                                                                                            \
    union type_punner_t                                                                                                                                        \
    {                                                                                                                                                          \
      cxx_result_status_code_##ident c;                                                                                                                        \
      BOOST_OUTCOME_V2_NAMESPACE::experimental::status_result<R> cpp;                                                                                                \
                                                                                                                                                               \
      type_punner_t()                                                                                                                                          \
          : c{}                                                                                                                                                \
      {                                                                                                                                                        \
      }                                                                                                                                                        \
      ~type_punner_t() {}                                                                                                                                      \
    } pun;                                                                                                                                                     \
                                                                                                                                                               \
    pun.c = v;                                                                                                                                                 \
    return std::move(pun.cpp);                                                                                                                                 \
  }                                                                                                                                                            \
  BOOST_OUTCOME_C_NODISCARD inline cxx_result_status_code_##ident to_##ident(BOOST_OUTCOME_V2_NAMESPACE::experimental::status_result<R> v)                                 \
  {                                                                                                                                                            \
    union type_punner_t                                                                                                                                        \
    {                                                                                                                                                          \
      BOOST_OUTCOME_V2_NAMESPACE::experimental::status_result<R> cpp;                                                                                                \
      cxx_result_status_code_##ident c;                                                                                                                        \
                                                                                                                                                               \
      type_punner_t(BOOST_OUTCOME_V2_NAMESPACE::experimental::status_result<R> v)                                                                                    \
          : cpp(std::move(v))                                                                                                                                  \
      {                                                                                                                                                        \
      }                                                                                                                                                        \
      ~type_punner_t() {}                                                                                                                                      \
    } pun{std::move(v)};                                                                                                                                       \
                                                                                                                                                               \
    return pun.c;                                                                                                                                              \
  }
#else
#define BOOST_OUTCOME_C_DECLARE_RESULT_STATUS_CODE_CXX(ident, R, S)
#endif

#define BOOST_OUTCOME_C_DECLARE_RESULT_STATUS_CODE(ident, R, S)                                                                                                            \
  struct cxx_result_status_code_##ident                                                                                                                        \
  {                                                                                                                                                            \
    R value;                                                                                                                                                   \
    unsigned flags;                                                                                                                                            \
    S error;                                                                                                                                                   \
  };                                                                                                                                                           \
  BOOST_OUTCOME_C_NODISCARD static BOOST_OUTCOME_C_INLINE struct cxx_result_status_code_##ident outcome_make_result_##ident##_success(R value)                             \
  { /* We special case this so it inlines efficiently */                                                                                                       \
    struct cxx_result_status_code_##ident ret;                                                                                                                 \
    ret.value = value;                                                                                                                                         \
    ret.flags = 1 /* have_value */;                                                                                                                            \
    return ret;                                                                                                                                                \
  }                                                                                                                                                            \
  BOOST_OUTCOME_C_NODISCARD_EXTERN_C BOOST_OUTCOME_C_WEAK struct cxx_result_status_code_##ident outcome_make_result_##ident##_failure_posix(int errcode)                   \
  {                                                                                                                                                            \
    struct cxx_result_status_code_##ident ret;                                                                                                                 \
    assert(outcome_make_result_status_code_failure_posix); /* If this fails, you need to compile this file at least once in C++. */                            \
    outcome_make_result_status_code_failure_posix((void *) &ret, sizeof(ret), offsetof(struct cxx_result_status_code_##ident, flags), errcode);                \
    return ret;                                                                                                                                                \
  }                                                                                                                                                            \
  BOOST_OUTCOME_C_MSVC_FORCE_EMIT(outcome_make_result_##ident##_failure_posix)                                                                                       \
  BOOST_OUTCOME_C_NODISCARD_EXTERN_C BOOST_OUTCOME_C_WEAK struct cxx_result_status_code_##ident outcome_make_result_##ident##_failure_system(intptr_t errcode)             \
  {                                                                                                                                                            \
    struct cxx_result_status_code_##ident ret;                                                                                                                 \
    assert(outcome_make_result_status_code_failure_system); /* If this fails, you need to compile this file at least once in C++. */                           \
    outcome_make_result_status_code_failure_system((void *) &ret, sizeof(ret), offsetof(struct cxx_result_status_code_##ident, flags), errcode);               \
    return ret;                                                                                                                                                \
  }                                                                                                                                                            \
  BOOST_OUTCOME_C_MSVC_FORCE_EMIT(outcome_make_result_##ident##_failure_system)                                                                                      \
  BOOST_OUTCOME_C_DECLARE_RESULT_STATUS_CODE_CXX(ident, R, S)

#define BOOST_OUTCOME_C_RESULT_STATUS_CODE(ident) struct cxx_result_status_code_##ident

#define BOOST_OUTCOME_C_TO_RESULT_STATUS_CODE(ident, ...) to_##ident(__VA_ARGS__)
#define BOOST_OUTCOME_C_MAKE_RESULT_STATUS_CODE_SUCCESS(ident, ...) outcome_make_result_##ident##_success(__VA_ARGS__)
#define BOOST_OUTCOME_C_MAKE_RESULT_STATUS_CODE_FAILURE_POSIX(ident, ...) outcome_make_result_##ident##_failure_posix(__VA_ARGS__)
#define BOOST_OUTCOME_C_MAKE_RESULT_STATUS_CODE_FAILURE_SYSTEM(ident, ...) outcome_make_result_##ident##_failure_system(__VA_ARGS__)


  struct cxx_status_code_posix
  {
    void *domain;
    int value;
  };
#define BOOST_OUTCOME_C_DECLARE_RESULT_ERRNO(ident, R) BOOST_OUTCOME_C_DECLARE_RESULT_STATUS_CODE(posix_##ident, R, struct cxx_status_code_posix)
#define BOOST_OUTCOME_C_RESULT_ERRNO(ident) BOOST_OUTCOME_C_RESULT_STATUS_CODE(posix_##ident)

  struct cxx_status_code_system
  {
    void *domain;
    intptr_t value;
  };
#define BOOST_OUTCOME_C_DECLARE_RESULT_SYSTEM(ident, R) BOOST_OUTCOME_C_DECLARE_RESULT_STATUS_CODE(system_##ident, R, struct cxx_status_code_system)
#define BOOST_OUTCOME_C_RESULT_SYSTEM(ident) BOOST_OUTCOME_C_RESULT_STATUS_CODE(system_##ident)

#define BOOST_OUTCOME_C_TO_RESULT_SYSTEM_CODE(ident, ...) to_system_##ident(__VA_ARGS__)
#define BOOST_OUTCOME_C_MAKE_RESULT_SYSTEM_SUCCESS(ident, ...) BOOST_OUTCOME_C_MAKE_RESULT_STATUS_CODE_SUCCESS(system_##ident, __VA_ARGS__)
#define BOOST_OUTCOME_C_MAKE_RESULT_SYSTEM_FAILURE_POSIX(ident, ...) BOOST_OUTCOME_C_MAKE_RESULT_STATUS_CODE_FAILURE_POSIX(system_##ident, __VA_ARGS__)
#define BOOST_OUTCOME_C_MAKE_RESULT_SYSTEM_FAILURE_SYSTEM(ident, ...) BOOST_OUTCOME_C_MAKE_RESULT_STATUS_CODE_FAILURE_SYSTEM(system_##ident, __VA_ARGS__)

#define BOOST_OUTCOME_C_RESULT_SYSTEM_TRY_IMPLV(unique, retstmt, cleanup, spec, ...)                                                                                       \
  BOOST_OUTCOME_TRYV2_UNIQUE_STORAGE(unique, spec, __VA_ARGS__);                                                                                                     \
  BOOST_OUTCOME_TRY_LIKELY_IF(BOOST_OUTCOME_C_RESULT_HAS_VALUE(unique));                                                                                                         \
  else                                                                                                                                                         \
  {                                                                                                                                                            \
    retstmt;                                                                                                                                                   \
  }
#define BOOST_OUTCOME_C_RESULT_SYSTEM_TRY_IMPLA(unique, retstmt, cleanup, var, ...)                                                                                        \
  BOOST_OUTCOME_C_RESULT_SYSTEM_TRY_IMPLV(unique, retstmt, cleanup, var, __VA_ARGS__)                                                                                      \
  BOOST_OUTCOME_TRY2_VAR(var) = unique.value

#define BOOST_OUTCOME_C_RESULT_SYSTEM_TRY_IMPL_RETURN(unique, ident)                                                                                                       \
  BOOST_OUTCOME_C_RESULT_SYSTEM(ident) unique##_f;                                                                                                                         \
  unique##_f.flags = (unique).flags;                                                                                                                           \
  unique##_f.error = (unique).error;                                                                                                                           \
  return unique##_f

#define BOOST_OUTCOME_C_RESULT_SYSTEM_TRY_WITH_SPEC_AND_CLEANUP_AND_NEW_RETURN_TYPE(unique, spec, ident, cleanup, ...)                                                     \
  BOOST_OUTCOME_C_RESULT_SYSTEM_TRY_IMPLA(unique, BOOST_OUTCOME_C_RESULT_SYSTEM_TRY_IMPL_RETURN(unique, ident), cleanup, spec, __VA_ARGS__)
#define BOOST_OUTCOME_C_RESULT_SYSTEM_TRY_WITH_SPEC_AND_CLEANUP(unique, spec, cleanup, ...) BOOST_OUTCOME_C_RESULT_SYSTEM_TRY_IMPLA(unique, return unique, cleanup, spec, __VA_ARGS__)
#define BOOST_OUTCOME_C_RESULT_SYSTEM_TRY_WITH_CLEANUP(unique, cleanup, ...) BOOST_OUTCOME_C_RESULT_SYSTEM_TRY_IMPLV(unique, return unique, cleanup, deduce, __VA_ARGS__)
#define BOOST_OUTCOME_C_RESULT_SYSTEM_TRY_SAME_RETURN_TYPE(unique, ...) BOOST_OUTCOME_C_RESULT_SYSTEM_TRY_IMPLV(unique, return unique, , deduce, __VA_ARGS__)

#define BOOST_OUTCOME_C_RESULT_SYSTEM_TRY_INVOKE_TRY8(a, b, c, d, e, f, g, h)                                                                                              \
  BOOST_OUTCOME_C_RESULT_SYSTEM_TRY_WITH_SPEC_AND_CLEANUP_AND_NEW_RETURN_TYPE(BOOST_OUTCOME_TRY_UNIQUE_NAME, a, b, c, d, e, f, g, h)
#define BOOST_OUTCOME_C_RESULT_SYSTEM_TRY_INVOKE_TRY7(a, b, c, d, e, f, g)                                                                                                 \
  BOOST_OUTCOME_C_RESULT_SYSTEM_TRY_WITH_SPEC_AND_CLEANUP_AND_NEW_RETURN_TYPE(BOOST_OUTCOME_TRY_UNIQUE_NAME, a, b, c, d, e, f, g)
#define BOOST_OUTCOME_C_RESULT_SYSTEM_TRY_INVOKE_TRY6(a, b, c, d, e, f)                                                                                                    \
  BOOST_OUTCOME_C_RESULT_SYSTEM_TRY_WITH_SPEC_AND_CLEANUP_AND_NEW_RETURN_TYPE(BOOST_OUTCOME_TRY_UNIQUE_NAME, a, b, c, d, e, f)
#define BOOST_OUTCOME_C_RESULT_SYSTEM_TRY_INVOKE_TRY5(a, b, c, d, e) BOOST_OUTCOME_C_RESULT_SYSTEM_TRY_WITH_SPEC_AND_CLEANUP_AND_NEW_RETURN_TYPE(BOOST_OUTCOME_TRY_UNIQUE_NAME, a, b, c, d, e)
#define BOOST_OUTCOME_C_RESULT_SYSTEM_TRY_INVOKE_TRY4(a, b, c, d) BOOST_OUTCOME_C_RESULT_SYSTEM_TRY_WITH_SPEC_AND_CLEANUP_AND_NEW_RETURN_TYPE(BOOST_OUTCOME_TRY_UNIQUE_NAME, a, b, c, d)
#define BOOST_OUTCOME_C_RESULT_SYSTEM_TRY_INVOKE_TRY3(a, b, c) BOOST_OUTCOME_C_RESULT_SYSTEM_TRY_WITH_SPEC_AND_CLEANUP(BOOST_OUTCOME_TRY_UNIQUE_NAME, a, b, c)
#define BOOST_OUTCOME_C_RESULT_SYSTEM_TRY_INVOKE_TRY2(a, b) BOOST_OUTCOME_C_RESULT_SYSTEM_TRY_WITH_CLEANUP(BOOST_OUTCOME_TRY_UNIQUE_NAME, a, b)
#define BOOST_OUTCOME_C_RESULT_SYSTEM_TRY_INVOKE_TRY1(expr) BOOST_OUTCOME_C_RESULT_SYSTEM_TRY_SAME_RETURN_TYPE(BOOST_OUTCOME_TRY_UNIQUE_NAME, expr)

#define BOOST_OUTCOME_C_RESULT_SYSTEM_TRY(...) BOOST_OUTCOME_TRY_CALL_OVERLOAD(BOOST_OUTCOME_C_RESULT_SYSTEM_TRY_INVOKE_TRY, __VA_ARGS__)

#define BOOST_OUTCOME_C_MAKE_RESULT_SYSTEM_FROM_ENUM(ident, enum_name, ...) outcome_make_result_##ident##_failure_system_enum_##enum_name(__VA_ARGS__)
#ifndef __cplusplus
// Declares the function in C, needs to occur at least once in a C++ source file to get implemented
#define BOOST_OUTCOME_C_DECLARE_RESULT_SYSTEM_FROM_ENUM(ident, enum_name, uuid, ...)                                                                                       \
  BOOST_OUTCOME_C_NODISCARD_EXTERN_C struct cxx_result_status_code_system_##ident outcome_make_result_##ident##_failure_system_enum_##enum_name(enum enum_name v);
#else
}

#include "../config.hpp"
#include "status-code/config.hpp"
#include "status-code/system_code.hpp"
#include "status_result.hpp"


#include "status-code/posix_code.hpp"
#ifdef _WIN32
#include "status-code/win32_code.hpp"
#endif

#include <algorithm>
#include <cstring>

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

// You need to include this C header in at least one C++ source file to have these C helper functions be implemented
extern "C" BOOST_OUTCOME_C_WEAK void outcome_make_result_status_code_failure_posix(void *out, size_t bytes, size_t offset, int errcode)
{
  using value_type = BOOST_OUTCOME_V2_NAMESPACE::experimental::posix_code::value_type;
  union type_punner_t
  {
    BOOST_OUTCOME_V2_NAMESPACE::experimental::status_result<intptr_t> cpp;
    struct cxx_status_code
    {
      intptr_t value;
      unsigned flags;
      cxx_status_code_system error;
    } c;

    explicit type_punner_t(BOOST_OUTCOME_V2_NAMESPACE::experimental::status_result<intptr_t> res)
        : cpp(std::move(res))
    {
    }
    ~type_punner_t() {}
  } pun{BOOST_OUTCOME_V2_NAMESPACE::experimental::posix_code(errcode)};
  static_assert(sizeof(pun.cpp) == sizeof(pun.c), "");
  static constexpr size_t punoffset = offsetof(type_punner_t::cxx_status_code, flags);
  assert(bytes - offset >= sizeof(pun.cpp) - punoffset);
  const size_t tocopy = std::min(bytes - offset, sizeof(pun.cpp) - punoffset);
  memcpy(out, (void *) &pun.c, sizeof(value_type));
  memcpy((void *) ((char *) out + offset), (const void *) ((const char *) &pun.c + punoffset), tocopy);
}
BOOST_OUTCOME_C_MSVC_FORCE_EMIT(outcome_make_result_status_code_failure_posix)

extern "C" BOOST_OUTCOME_C_WEAK void outcome_make_result_status_code_failure_system(void *out, size_t bytes, size_t offset, intptr_t errcode)
{
  using value_type = BOOST_OUTCOME_V2_NAMESPACE::experimental::system_code::value_type;
  union type_punner_t
  {
    BOOST_OUTCOME_V2_NAMESPACE::experimental::status_result<intptr_t> cpp;
    struct cxx_status_code
    {
      intptr_t value;
      unsigned flags;
      cxx_status_code_system error;
    } c;

    explicit type_punner_t(BOOST_OUTCOME_V2_NAMESPACE::experimental::status_result<intptr_t> res)
        : cpp(std::move(res))
    {
    }
    ~type_punner_t() {}
  } pun{
#ifdef _WIN32
  BOOST_OUTCOME_V2_NAMESPACE::experimental::win32_code((BOOST_OUTCOME_V2_NAMESPACE::experimental::win32::DWORD) errcode)
#else
  BOOST_OUTCOME_V2_NAMESPACE::experimental::posix_code((int) errcode)
#endif
  };
  static_assert(sizeof(pun.cpp) == sizeof(pun.c), "");
  static constexpr size_t punoffset = offsetof(type_punner_t::cxx_status_code, flags);
  assert(bytes - offset >= sizeof(pun.cpp) - punoffset);
  const size_t tocopy = std::min(bytes - offset, sizeof(pun.cpp) - punoffset);
  memcpy(out, (void *) &pun.c, sizeof(value_type));
  memcpy((void *) ((char *) out + offset), (const void *) ((const char *) &pun.c + punoffset), tocopy);
}
BOOST_OUTCOME_C_MSVC_FORCE_EMIT(outcome_make_result_status_code_failure_system)

extern "C" BOOST_OUTCOME_C_WEAK int outcome_status_code_equal(const void *_a, const void *_b)
{
  const auto *a = (const BOOST_OUTCOME_SYSTEM_ERROR2_NAMESPACE::system_code *) _a;
  const auto *b = (const BOOST_OUTCOME_SYSTEM_ERROR2_NAMESPACE::system_code *) _b;
  return *a == *b;
}
BOOST_OUTCOME_C_MSVC_FORCE_EMIT(outcome_status_code_equal)

extern "C" BOOST_OUTCOME_C_WEAK int outcome_status_code_equal_generic(const void *_a, int errcode)
{
  const auto *a = (const BOOST_OUTCOME_SYSTEM_ERROR2_NAMESPACE::system_code *) _a;
  return *a == (BOOST_OUTCOME_SYSTEM_ERROR2_NAMESPACE::errc) errcode;
}
BOOST_OUTCOME_C_MSVC_FORCE_EMIT(outcome_status_code_equal_generic)

extern "C" BOOST_OUTCOME_C_WEAK const char *outcome_status_code_message(const void *_a)
{
  static thread_local BOOST_OUTCOME_SYSTEM_ERROR2_NAMESPACE::system_code::string_ref msg((const char *) nullptr, 0);
  const auto *a = (const BOOST_OUTCOME_SYSTEM_ERROR2_NAMESPACE::system_code *) _a;
  msg = a->message();
  return msg.c_str();
}
BOOST_OUTCOME_C_MSVC_FORCE_EMIT(outcome_status_code_message)

BOOST_OUTCOME_V2_NAMESPACE_BEGIN
namespace experimental
{
  namespace detail
  {
    template <class RetType, class EnumType> inline RetType outcome_make_result_failure_system_enum(EnumType v)
    {
      using value_type = BOOST_OUTCOME_V2_NAMESPACE::experimental::system_code::value_type;
      union type_punner_t
      {
        BOOST_OUTCOME_V2_NAMESPACE::experimental::status_result<intptr_t> cpp;
        struct cxx_status_code
        {
          intptr_t value;
          unsigned flags;
          cxx_status_code_system error;
        } c;

        explicit type_punner_t(BOOST_OUTCOME_V2_NAMESPACE::experimental::status_result<intptr_t> res)
            : cpp(std::move(res))
        {
        }
        ~type_punner_t() {}
      } pun{BOOST_OUTCOME_V2_NAMESPACE::experimental::quick_status_code_from_enum_code<EnumType>(v)};
      static constexpr size_t bytes = sizeof(RetType);
      static constexpr size_t offset = offsetof(RetType, flags);
      static constexpr size_t punoffset = offsetof(typename type_punner_t::cxx_status_code, flags);
      assert(bytes - offset >= sizeof(pun.cpp) - punoffset);
      const size_t tocopy = std::min(bytes - offset, sizeof(pun.cpp) - punoffset);
      RetType ret;
      memcpy(&ret, (void *) &pun.c, sizeof(value_type));
      memcpy((void *) ((char *) &ret + offset), (const void *) ((const char *) &pun.c + punoffset), tocopy);
      return ret;
    }
  }  // namespace detail
}  // namespace experimental
BOOST_OUTCOME_V2_NAMESPACE_END

// Unique UUID for the enum PLEASE use https://www.random.org/cgi-bin/randbyte?nbytes=16&format=h
// .. is sequence of {enum_name::value, "text description", {errc::equivalent, ...}},
#define BOOST_OUTCOME_C_DECLARE_RESULT_SYSTEM_FROM_ENUM(ident, enum_name, uuid, ...)                                                                                       \
  BOOST_OUTCOME_SYSTEM_ERROR2_NAMESPACE_BEGIN                                                                                                                                \
  template <> struct quick_status_code_from_enum<enum enum_name> : quick_status_code_from_enum_defaults<enum enum_name>                                        \
  {                                                                                                                                                            \
    static constexpr const auto domain_name = #enum_name;                                                                                                      \
    static constexpr const auto domain_uuid = uuid;                                                                                                            \
    static const std::initializer_list<mapping> &value_mappings()                                                                                              \
    {                                                                                                                                                          \
      static const std::initializer_list<mapping> v = {__VA_ARGS__};                                                                                           \
      return v;                                                                                                                                                \
    }                                                                                                                                                          \
  };                                                                                                                                                           \
  BOOST_OUTCOME_SYSTEM_ERROR2_NAMESPACE_END                                                                                                                                  \
  extern "C" BOOST_OUTCOME_C_NODISCARD BOOST_OUTCOME_C_WEAK struct cxx_result_status_code_system_##ident outcome_make_result_##ident##_failure_system_enum_##enum_name(    \
  enum enum_name v)                                                                                                                                            \
  {                                                                                                                                                            \
    return BOOST_OUTCOME_V2_NAMESPACE::experimental::detail::outcome_make_result_failure_system_enum<struct cxx_result_status_code_system_##ident>(v);               \
  }                                                                                                                                                            \
  BOOST_OUTCOME_C_MSVC_FORCE_EMIT(outcome_make_result_##ident##_failure_system_enum_##enum_name)

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

#endif

#endif

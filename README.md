# What is noexcept-compile-time-check?

The noexcept-compile-time-check (or noexcept-ctcheck for short) is a C++11 library with several simple macros that give you some control over the content of `noexcept` methods and functions in modern C++.

A simple example just to give you some impression:

```cpp
#include <noexcept_ctcheck/pub.hpp>
...
class some_complex_container {
   one_container first_data_part_;
   another_container second_data_part_;
   ...
public:
   friend void swap(some_complex_container & a, some_complex_container & b) noexcept {
      using std::swap;
      // We'll get a compile-time error if that swap is not noexcept.
      NOEXCEPT_CTCHECK_ENSURE_NOEXCEPT_STATEMENT(
         swap(a.first_data_part_, b.first_data_part_));
      NOEXCEPT_CTCHECK_ENSURE_NOEXCEPT_STATEMENT(
         swap(a.second_data_part_, b.second_data_part_));
      ...
   }
   ...
   void clean() noexcept {
      // We'll get a compile-time error if clean() for subcontainers is not noexcept.
      NOEXCEPT_CTCHECK_ENSURE_NOEXCEPT_STATEMENT(first_data_part_.clean());
      NOEXCEPT_CTCHECK_ENSURE_NOEXCEPT_STATEMENT(second_data_part_.clean());
      ...
   }
   ...
};

class some_resource_holder {
public:
   ...
   // Note that release() is not noexcept method and can throw.
   void release() { ... }

   // The destructor of some_resource_holder should be noexcept.
   ~some_resource_holder() noexcept {
      try {
         // We'll get a compile-time error if release() is noexcept and out try-catch is redundant.
         NOEXCEPT_CTCHECK_ENSURE_NOT_NOEXCEPT_STATEMENT(release());
      } catch(...) {}
   }
};
```

# The motivation

There are some places where we rely on the absence of exceptions in some code: implementation of swap-functions, move- and/or copy-operators, destructors, cleanup functions and so on. It is safe to call only non-throwing methods/functions in those places. The modern C++ (C++11 and above) has `noexcept` modificator for methods/functions. That modificator significantly simplifies the writing of exception safe code. We can check the signatures of called functions and do appropriate actions. For example:

```cpp
class some_complex_business_logic_class {
   first_resource_type first_resource_; // Has noexcept release() method.
   second_resource_type second_resource_; // Has close() method that throws.
   ...
public:
   ~some_complex_business_logic_class() noexcept {
      // It's safe to call release() for the first resource without enclosing try-catch.
      first_resource_.release();

      // But the call to second resource's close() should be enclosed in try-catch.
      try { second_resource_.close(); } catch(...) {}
   }
   ...
};
```
But there is a problem: the compiler doesn't warn if I call some throwing method/function inside my noexcept-method/function. It opens a wide field for subtle errors.

For example, suppose that I make a mistake and mixed up the info about `release()` and `close()` exception safety. In reality `first_resource_type::release()` throws, but `second_resource_type::close()` doesn't. It means that my implementation of `some_complex_business_logic_class`'s destructor is wrong. But the compiler doesn't tell me about that.

Or another case: suppose that I used some 3-rd party library v.1.0 in that `first_resource_type::release()` is `noexcept`. I wrote some code that relied on the fact that `release()` didn't throw exceptions. But after some time I switch to v.3.0 of that library. And in v.3.0 `release()` isn't `noexcept` anymore. I should rewrite a part of my code, but I can forget about that and compiler doesn't help me here.

The noexcept-ctcheck library contains several simple macros that allow forcing noexcept-ness of some code fragments in the compile-time. It allows getting some help from a compiler and that especially important for the maintenance of old code.

## Origins

The first version of those macros was developed during the evolution of [RESTinio](https://github.com/Stiffstream/restinio) library. It helped us to catch some errors with the exception safety of callbacks passed to Asio. So the idea behind noexcept-ctcheck library has already proved its evaluability.

## Why macros?

The similar effect can be obtained by using raw `static_assert` in the code. For example, we can write:
```cpp
static_assert(noexcept(close()), "close() is expected to be noexcept");
close();
```
instead of:
```cpp
NOEXCEPT_CTCHECK_ENSURE_NOEXCEPT_STATEMENT(close());
```
But macros has an important advantage: they can easily be turned off. It can be necessary, for example, if you update some 3-rd party library and get too many compile-time errors.

# What is inside?

There is just one header-file that should be included: `noexcept_ctcheck/pub.hpp`. It contains several macros with names like `NOEXCEPT_CTCHECK_`.

## The main working principle

The macros are divided into two groups.

The macros from the first group performs compile-time checks and then execute the specified statement. Those macros are in the following form:

```cpp
#define NOEXCEPT_CTCHECK_ENSURE_NOEXCEPT_STATEMENT(stmt) \
	do { \
		static_assert(noexcept(stmt), "this statement is expected to be noexcept: " #stmt); \
		stmt; \
	} while(false)
```
It means that those macros can't be used as expressions. So we can write that:
```cpp
NOEXCEPT_CTCHECK_ENSURE_NOEXCEPT_STATEMENT(fclose(file));
```
But can't write that:
```cpp
FILE * file = NOEXCEPT_CTCHECK_ENSURE_NOEXCEPT_STATEMENT(fopen(...));
```

The macros from the second group only perform compile-time checking without execution of any code in the run-time. For example:
```cpp
// Only compile-time here.
NOEXCEPT_CTCHECK_STATIC_ASSERT_NOEXCEPT(fopen(...));
FILE * file = fopen(...);
```

## NOEXCEPT_CTCHECK_ENSURE_NOEXCEPT_STATEMENT

The macro `NOEXCEPT_CTCHECK_ENSURE_NOEXCEPT_STATEMENT(stmt)` performs a compile-time check and inserts the `stmt` into the source code (so `stmt` will be executed at run-time). It generates compile-time error if `stmt` is not noexcept.

It is intended to be used for the check that there is no need for enclosing try-catch blocks:
```cpp
class some_resource_owner {
   some_resource resource_;
   ...
public:
   ~some_resource_owner() noexcept {
      NOEXCEPT_CTCHECK_ENSURE_NOEXCEPT_STATEMENT(release(resource_));
      ...
   }
   ...
};
```

## NOEXCEPT_CTCHECK_ENSURE_NOT_NOEXCEPT_STATEMENT

The macro `NOEXCEPT_CTCHECK_ENSURE_NOT_NOEXCEPT_STATEMENT(stmt)` performs a compile-time check inserts the `stmt` into the source code (so `stmt` will be executed at run-time). It generates compile-time error if `stmt` is noexcept.

It is intended to be used for the check that there is a need for enclosing try-catch blocks:
```cpp
class some_resource_owner {
   some_resource resource_;
   ...
public:
   ~some_resource_owner() noexcept {
      try {
         NOEXCEPT_CTCHECK_ENSURE_NOT_NOEXCEPT_STATEMENT(
            release(resource_));
      } catch(...) {}
      ...
   }
   ...
};
```

## NOEXCEPT_CTCHECK_STATIC_ASSERT_NOEXCEPT

The macro `NOEXCEPT_CTCHECK_STATIC_ASSERT_NOEXCEPT(stmt)` performs a compile-time check of `stmt` but doesn't inserts `stmt` into the code (so `stmt` will not be executed at run-time). It generates compile-time error if `stmt` is not noexcept.

It is intended to be used for the check that there is no need for enclosing try-catch blocks:
```cpp
void remove_appropriate_items_at_front(some_container_t & cnt) noexcept {
	NOEXCEPT_CTCHECK_STATIC_ASSERT_NOEXCEPT(cnt.empty());
	NOEXCEPT_CTCHECK_STATIC_ASSERT_NOEXCEPT(cnt.front());
	NOEXCEPT_CTCHECK_STATIC_ASSERT_NOEXCEPT(cnt.pop_front());

	while(!cnt.empty() && some_conditions(cnt.front()) {
		// We don't expect exceptions here.
		cnt.pop_front();
	}
}
```

## NOEXCEPT_CTCHECK_STATIC_ASSERT_NOT_NOEXCEPT

The macro `NOEXCEPT_CTCHECK_STATIC_ASSERT_NOT_NOEXCEPT(stmt)` performs a compile-time check of `stmt` but doesn't inserts `stmt` into the code (so `stmt` will not be executed at run-time). It generates compile-time error if `stmt` is noexcept.

It is intended to be used for the check that there is a need for enclosing try-catch blocks:
```cpp
void some_class::do_something() noexcept {
	// If the constructor of 'some_type' throws then we have to use try-catch block.
	NOEXCEPT_CTCHECK_STATIC_ASSERT_NOT_NOEXCEPT(
		some_type{
				std::declval<some_param_type>(),
				std::declval<another_param_type>()});
	try {
 		some_type resource{first_arg, second_arg};
		...
	} catch(...) {}
	...
}
```

## How to disable compile-time checking?

To disable compile-time checking just define `NOEXCEPT_CTCHECK_DISABLE` symbol before including `noexcept_ctcheck/pub.hpp`. For example, that way:

```cpp
#define NOEXCEPT_CTCHECK_DISABLE
#include <noexcept_ctcheck/pub.hpp>
```

# How to get and use?

It is a header-only library without any external dependencies. Just copy `noexcept_ctcheck` directory into your project.

A package for Conan and/or vcpkg can be created by a request. If you want noexcept-ctcheck be available via those package managers please open an issue.

# License

noexcept-ctcheck is developed by [stiffstream](https://stiffstream.com) and distributed under BSD-3-Clause license.


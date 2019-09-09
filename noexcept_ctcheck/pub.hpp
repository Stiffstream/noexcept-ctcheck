/*!
 * @file
 * @brief The main macros of the library.
 */

#if !defined(NOEXCEPT_CTCHECK_PUB_HPP_)
#define NOEXCEPT_CTCHECK_PUB_HPP_

#if defined(NOEXCEPT_CTCHECK_DISABLE)

	#define NOEXCEPT_CTCHECK_ENSURE_NOEXCEPT_STATEMENT(stmt)
	#define NOEXCEPT_CTCHECK_STATIC_ASSERT_NOEXCEPT(expr)
	#define NOEXCEPT_CTCHECK_STATIC_ASSERT_NOT_NOEXCEPT(expr)

#else

/*!
 * @brief A wrapper around static_assert for checking that a statement
 * is noexcept and execution of that statement.
 *
 * Usage example:
 * @code
 * some_class::~some_class() noexcept {
 * 	// We should have a guarantee that this call doesn't throw.
 * 	NOEXCEPT_CTCHECK_ENSURE_NOEXCEPT_STATEMENT(m_some_resouce.release());
 * 	...
 * }
 * @endcode
 */
#define NOEXCEPT_CTCHECK_ENSURE_NOEXCEPT_STATEMENT(stmt) \
	do { \
		static_assert(noexcept(stmt), "this statement is expected to be noexcept: " #stmt); \
		stmt; \
	} while(false)

/*!
 * @brief A wrapper around static_assert for checking that a statement
 * is not noexcept and execution of that statement.
 *
 * Usage example:
 * @code
 * some_class::~some_class() noexcept {
 * 	// If that call throws then we have to use try-catch block.
 * 	try {
 * 		NOEXCEPT_CTCHECK_ENSURE_NOT_NOEXCEPT_STATEMENT(m_some_resouce.release());
 * 	}
 * 	catch(...) {}
 * 	...
 * }
 * @endcode
 */
#define NOEXCEPT_CTCHECK_ENSURE_NOT_NOEXCEPT_STATEMENT(stmt) \
	do { \
		static_assert(!noexcept(stmt), "this statement is not expected to be noexcept: " #stmt); \
		stmt; \
	} while(false)

/*!
 * @brief A wrapper around static_assert for checking that an expression is
 * noexcept
 *
 * Usage example:
 * @code
 * void remove_appropriate_items_at_front(some_container_t & cnt) noexcept {
 * 	NOEXCEPT_CTCHECK_STATIC_ASSERT_NOEXCEPT(cnt.empty());
 * 	NOEXCEPT_CTCHECK_STATIC_ASSERT_NOEXCEPT(cnt.front());
 * 	NOEXCEPT_CTCHECK_STATIC_ASSERT_NOEXCEPT(cnt.pop_front());
 *
 * 	while(!cnt.empty() && some_conditions(cnt.front()) {
 * 		// We don't expect exceptions here.
 * 		cnt.pop_front();
 * 	}
 * }
 * @endcode
 */
#define NOEXCEPT_CTCHECK_STATIC_ASSERT_NOEXCEPT(expr) \
	static_assert(noexcept(expr), #expr " is expected to be noexcept" )

/*!
 * @brief A wrapper around static_assert for checking that an expression is
 * not noexcept.
 *
 * Usage example:
 * @code
 * void some_class::do_something() noexcept {
 * 	// If the constructor of 'some_type' throws then we have to use try-catch block.
 * 	NOEXCEPT_CTCHECK_STATIC_ASSERT_NOT_NOEXCEPT(
 * 		some_type{
 * 				std::declval<some_param_type>(),
 * 				std::declval<another_param_type>()});
 * 	try {
 * 		some_type resource{first_arg, second_arg};
 * 		...
 * 	}
 * 	catch(...) {}
 * 	...
 * }
 * @endcode
 */
#define NOEXCEPT_CTCHECK_STATIC_ASSERT_NOT_NOEXCEPT(expr) \
	static_assert(!noexcept(expr), #expr " is not expected to be noexcept" )

#endif

#endif


#include <iostream>

#include <noexcept_ctcheck/pub.hpp>

class safe_resource {
public:
	safe_resource() {}
	~safe_resource() noexcept {
		NOEXCEPT_CTCHECK_ENSURE_NOEXCEPT_STATEMENT(close());
	}

	void acquire() {}
	void use() {}
	void close() noexcept {}
};

class unsafe_resource {
public:
	unsafe_resource() {}
	~unsafe_resource() noexcept {
		try {
			NOEXCEPT_CTCHECK_ENSURE_NOT_NOEXCEPT_STATEMENT(close());
		}
		catch(...) {}
	}

	void acquire() {}
	void use() {}
	void close() {}
};

template< typename T >
void try_acquire_and_use_resource() noexcept {
	// We expect that T's constructor throws.
	NOEXCEPT_CTCHECK_STATIC_ASSERT_NOT_NOEXCEPT(T{});
	// We expect that T's destructor doesn't throw.
	NOEXCEPT_CTCHECK_STATIC_ASSERT_NOEXCEPT(std::declval<T>().~T());

	try {
		T resource;
		resource.acquire();
		resource.use();
	}
	catch(...) {}
}

int main() {
	try_acquire_and_use_resource<safe_resource>();
	try_acquire_and_use_resource<unsafe_resource>();

	return 0;
}


#include <iostream>

// Uncomment this to disable compile-time checks.
//#define NOEXCEPT_CTCHECK_DISABLE
#include <noexcept_ctcheck/pub.hpp>

class safe_resource {
public:
	safe_resource() {}
	~safe_resource() noexcept {
		NOEXCEPT_CTCHECK_ENSURE_NOEXCEPT_STATEMENT(release());
	}

	void acquire() { std::cout << "safe_resource::acquire()" << std::endl; }
	void use() { std::cout << "safe_resource::use()" << std::endl; }
	void release() noexcept { std::cout << "safe_resource::release()" << std::endl; }
};

class unsafe_resource {
public:
	unsafe_resource() {}
	~unsafe_resource() noexcept {
		try {
			NOEXCEPT_CTCHECK_ENSURE_NOT_NOEXCEPT_STATEMENT(release());
		}
		catch(...) {}
	}

	void acquire() { std::cout << "unsafe_resource::acquire()" << std::endl; }
	void use() { std::cout << "unsafe_resource::use()" << std::endl; }
	void release() { std::cout << "unsafe_resource::release()" << std::endl; }
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


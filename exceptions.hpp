#ifndef SJTU_EXCEPTIONS_HPP
#define SJTU_EXCEPTIONS_HPP

#include <cstddef>
#include <cstring>
#include <string>

namespace sjtu {

	class exception {
	protected:
		const std::string variant = "";
		std::string detail = "";
	public:
		exception() {}

		exception(const exception &ec) : variant(ec.variant), detail(ec.detail) {}

		virtual std::string what() {
			return variant + " " + detail;
		}
	};

	class permission_denied : public exception {
		/* __________________________ */
	};

	class invalid_iterator : public exception {

	};

	class index_out_of_bound : public exception {

	};

	class invalid_operation : public exception {

	};

	class overflow : public exception {

	};
}
#endif

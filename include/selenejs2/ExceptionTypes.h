#pragma once

#include <exception>
#include <string>
#include <utility>

#include <duktape.h>

namespace seljs2 {

class BaseSeleneJSException : public std::exception {};

class SeleneJSException : public BaseSeleneJSException {
	std::string _message;
public:
	explicit SeleneJSException(const std::string &message)
		: _message(message) {}
	char const * what() const noexcept override {
		return _message.c_str();
	}
};

class TypeError : public BaseSeleneJSException {
    std::string _message;
public:
    explicit TypeError(std::string expected)
      : _message(std::move(expected)
                 + " expected, got no object.") {}
    explicit TypeError(std::string expected, std::string const & actual)
      : _message(std::move(expected)
                 + " expected, got " + actual + '.') {}
    char const * what() const noexcept override {
        return _message.c_str();
    }
};

class CopyUnregisteredType : public BaseSeleneJSException {
public:
    using TypeID = std::reference_wrapper<const std::type_info>;
    explicit CopyUnregisteredType(TypeID type) : _type(type) {}

    TypeID getType() const
    {
        return _type;
    }
    char const * what() const noexcept override {
        return "Tried to copy an object of an unregistered type. "
               "Please register classes before passing instances by value.";
    }
private:
    TypeID _type;
};

class NotImplementedError : public BaseSeleneJSException {
public:
	char const * what() const noexcept override {
		return "Not implemented.";
	}
};

class NotSupportedError : public BaseSeleneJSException {
public:
	char const * what() const noexcept override {
		return "Not supported.";
	}
};

// gets message and possible traceback from Error object
inline std::string ErrorMessage(duk_context *L, duk_idx_t index) {
	std::string ret;
	duk_idx_t eindex = duk_normalize_index(L, index);

	// TODO: check if index is really an error object
	// gets the "stack" property of the Error object
	if (duk_is_object(L, eindex)) {
		duk_get_prop_string(L, eindex, "stack");
		if (duk_is_string(L, -1)) {
			size_t size;
			const char *buff = duk_get_lstring(L, -1, &size);
			ret = std::string{ buff, size };
			duk_pop(L);
			return ret;
		}
		duk_pop(L);
	}

	// gets message from object using ToString()
	ret = duk_safe_to_string(L, eindex);
	return ret;
}


}

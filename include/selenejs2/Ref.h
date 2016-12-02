#pragma once

#include "BaseContext.h"
#include "ResourceHandler.h"
#include "ExceptionTypes.h"
#include "Type.h"
#include "Class.h"
#include "detail/refs.h"
#include "detail/value.h"
#include "detail/refsholder.h"

#include <iostream>
#include <memory>
#include <vector>

namespace seljs2 {

class RefPush
{
public:
	virtual ~RefPush() {}
	virtual void push(BaseContext *ctx) = 0;
};

class Ref 
{
public:
	Ref(BaseContext *ctx, std::shared_ptr<RefPush> refpush = std::shared_ptr<RefPush>()) :
		_ctx(ctx), _refpush(refpush), _ref()
	{
		if (!_refpush)
			_ref.reset(new detail::refsholder(ctx));
	}

	virtual ~Ref() 
	{
	}

	Ref(const Ref& other) :
		_ctx(other._ctx),
		_refpush(other._refpush),
		_ref(other._ref)
	{
	}

	Ref& operator=(const Ref& other)
	{
		_ctx = other._ctx;
		_refpush = other._refpush;
		_ref = other._ref;
	}

	Ref(Ref&& other) :
		_ctx(other._ctx), _refpush(other._refpush), _ref()
	{
		_ref.swap(other._ref);
	}

	Ref& operator=(Ref&& other)
	{
		_ctx = other._ctx;
		_refpush = other._refpush;
		_ref.swap(other._ref);
		return *this;
	}

	Type type() const
	{
		ResetStackOnScopeExit r(*_ctx);
		push();
		duk_int_t t = duk_get_type(*_ctx, -1);
		switch (t)
		{
		case DUK_TYPE_NONE: return Type::NONE;
		case DUK_TYPE_UNDEFINED: return Type::UNDEFINED;
		case DUK_TYPE_NULL: return Type::TNULL;
		case DUK_TYPE_BOOLEAN: return Type::BOOLEAN;
		case DUK_TYPE_NUMBER: return Type::NUMBER;
		case DUK_TYPE_STRING: return Type::STRING;
		case DUK_TYPE_BUFFER: return Type::BUFFER;
		case DUK_TYPE_POINTER: return Type::POINTER;
		case DUK_TYPE_LIGHTFUNC: return Type::LIGHTFUNC;
		case DUK_TYPE_OBJECT: {
			if (duk_is_array(*_ctx, -1)) return Type::ARRAY;
			if (duk_is_function(*_ctx, -1)) return Type::FUNCTION;
			return Type::OBJECT;
		};
		}
		return Type::UNKNOWN;
	}

	void push() const
	{
		if (_refpush) 
		{
			_refpush->push(_ctx);
		}
		else
		{
			if (_ref)
				detail::duv_push_ref(*_ctx, _ref->ref());
			else
				duk_push_undefined(*_ctx);
		}
	}

	template<typename T>
	T get() const 
	{
		ResetStackOnScopeExit r(*_ctx);
		push();
		return detail::_get<T>(detail::_id<T>{}, *_ctx, -1);
	}

	template<typename T>
	T cast() const
	{
		ResetStackOnScopeExit r(*_ctx);
		push();
		return detail::_cast<T>(detail::_id<T>{}, *_ctx, -1);
	}

	template<typename T>
	void set(const std::string &name, T&& value)
	{
		ResetStackOnScopeExit r(*_ctx);
		push();
		detail::_push<T>(*_ctx, std::forward<T>(value));
		duk_put_prop_string(*_ctx, -2, name.c_str());
	}

	template<typename T>
	void set(const int index, T&& value)
	{
		ResetStackOnScopeExit r(*_ctx);
		push();
		detail::_push<T>(*_ctx, std::forward<T>(value));
		duk_put_prop_index(*_ctx, -2, index);
	}

	template<typename T>
	Ref setRef(const std::string &name, T&& value)
	{
		ResetStackOnScopeExit r(*_ctx);
		push();
		detail::_push<T>(*_ctx, std::forward<T>(value));
		duk_put_prop_string(*_ctx, -2, name.c_str());
		return (*this)[name];
	}

	template<typename T>
	Ref setRef(const int index, T&& value)
	{
		ResetStackOnScopeExit r(*_ctx);
		push();
		detail::_push<T>(*_ctx, std::forward<T>(value));
		duk_put_prop_index(*_ctx, -2, index);
		return (*this)[index];
	}

	Ref&& operator[](const std::string& name) && 
	{
		ResetStackOnScopeExit r(*_ctx);
		push();
		duk_get_prop_string(*_ctx, -1, name.c_str());
		_refpush.reset();
		_ref.reset(new detail::refsholder(_ctx));
		return std::move(*this);
	}

	Ref&& operator[](const int index) &&
	{
		ResetStackOnScopeExit r(*_ctx);
		push();
		duk_get_prop_index(*_ctx, -1, index);
		_refpush.reset();
		_ref.reset(new detail::refsholder(_ctx));
		return std::move(*this);
	}

	Ref operator[](const std::string &name) &
	{
		ResetStackOnScopeExit r(*_ctx);
		push();
		duk_get_prop_string(*_ctx, -1, name.c_str());
		return Ref(_ctx);
	}

	Ref operator[](const int index) &
	{
		ResetStackOnScopeExit r(*_ctx);
		push();
		duk_get_prop_index(*_ctx, -1, index);
		return Ref(_ctx);
	}

	bool hasKey(const std::string& name) const {
		ResetStackOnScopeExit r(*_ctx);
		push();
		return duk_is_object(*_ctx, -1) && duk_has_prop_string(*_ctx, -1, name.c_str());
	}

	int length() const {
		ResetStackOnScopeExit r(*_ctx);
		push();
		return duk_get_length(*_ctx, -1);
	}

	std::vector<std::string> keyList(duk_uint_t enum_flags = 0) const {
		ResetStackOnScopeExit r(*_ctx);
		push();
		std::vector<std::string> ret;
		duk_enum(*_ctx, -1, enum_flags);
		while (duk_next(*_ctx, -1, 0)) {
			ret.push_back(std::string(duk_get_string(*_ctx, -1)));
			duk_pop(*_ctx);
		}
		duk_pop(*_ctx); // enum
		return ret;
	}

	template <typename T, typename... Args, typename... Funs>
	void SetClass(const std::string &name, Funs... funs) {
		ResetStackOnScopeExit r(*_ctx);
		auto fun_tuple = std::make_tuple(std::forward<Funs>(funs)...);
		push();
		typename detail::_indices_builder<sizeof...(Funs)>::type d;
		_ctx->registry().RegisterClass<T, Args...>(name, fun_tuple, d);
		if (duk_is_undefined(*_ctx, -1) == 0) {
			// set name as constructor function
			duk_put_prop_string(*_ctx, -2, name.c_str());
		}
	}

	template <typename T, typename Constructor, typename Destructor, typename... Funs>
	void SetClassCustom(const std::string &name, Funs... funs) {
		ResetStackOnScopeExit r(*_ctx);
		auto fun_tuple = std::make_tuple(std::forward<Funs>(funs)...);
		push();
		typename detail::_indices_builder<sizeof...(Funs)>::type d;
		_ctx->registry().RegisterClassCustom<T, Constructor, Destructor>(name, fun_tuple, d);
		if (duk_is_undefined(*_ctx, -1) == 0) {
			// set name as constructor function
			duk_put_prop_string(*_ctx, -2, name.c_str());
		}
	}

	template <typename T>
	void SetClassObj(const std::string &name, T &t) {
		ResetStackOnScopeExit r(*_ctx);
		push();
		_ctx->registry().RegisterClassObj(t);
		duk_put_prop_string(*_ctx, -2, name.c_str());
		duk_pop(*_ctx);
	}

	BaseContext *ctx() const { return _ctx; }
private:
	BaseContext *_ctx;
	std::shared_ptr<RefPush> _refpush;
	std::shared_ptr<detail::refsholder> _ref;
};
}
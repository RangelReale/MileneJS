#pragma once

#include "ClassFun.h"
#include "Ctor.h"
#include "Dtor.h"
#include "detail/PrototypeRegistry.h"
#include <map>
#include <memory>
#include <vector>
#include <stack>

namespace miljs {

struct BaseClass {
    virtual ~BaseClass() {}
	virtual bool hasCtor() const { return false; }
};


template <typename T,
          typename A,
		  typename B,
          typename... Members>
class Class : public BaseClass {
private:
    std::string _name;
    std::string _prototype_name;
    std::unique_ptr<BaseFun> _ctor;
    std::unique_ptr<BaseFun> _dtor;
    using Funs = std::vector<std::unique_ptr<BaseFun>>;
    Funs _funs;

	template <typename AX = A>
	typename std::enable_if<std::is_void<AX>::value>::type
	_register_ctor(duk_context *ctx) {
		_ctor.reset();
	}

	template <typename AX = A>
	typename std::enable_if<!std::is_void<AX>::value>::type
	_register_ctor(duk_context *ctx) {
        _ctor.reset(new A(ctx, _prototype_name.c_str()));
    }

	template <typename BX = B>
	typename std::enable_if<std::is_void<BX>::value>::type
	_register_dtor(duk_context *ctx) {
		_dtor.reset();
	}

	template <typename BX = B>
	typename std::enable_if<!std::is_void<BX>::value>::type
	_register_dtor(duk_context *ctx) {
        _dtor.reset(new B(ctx, _prototype_name.c_str()));
    }

	template <typename M>
    void _register_member(duk_context *ctx,
                          const char *member_name,
                          M T::*member) {
        _register_member(ctx, member_name, member,
                         typename std::is_const<M>::type{});
    }

    template <typename M>
    void _register_member(duk_context *ctx,
                          const char *member_name,
                          M T::*member,
                          std::false_type) {
        std::function<M(T*)> lambda_get = [member](T *t) {
            return t->*member;
        };
        _funs.emplace_back(
			std::make_unique<ClassFun<1, T, M>>(
                ctx, std::string{member_name},
                lambda_get));

        std::function<void(T*, M)> lambda_set = [member](T *t, M value) {
            (t->*member) = value;
        };
        _funs.emplace_back(
			std::make_unique<ClassFun<0, T, void, M>>(
                ctx, std::string("set_") + member_name,
                lambda_set));
    }

    template <typename M>
    void _register_member(duk_context *ctx,
                          const char *member_name,
                          M T::*member,
                          std::true_type) {
        std::function<M(T*)> lambda_get = [member](T *t) {
            return t->*member;
        };
        _funs.emplace_back(
			std::make_unique<ClassFun<1, T, M>>(
                ctx, std::string{member_name},
                lambda_get));
    }

    template <typename Ret, typename... Args>
    void _register_member(duk_context *ctx,
                          const char *fun_name,
                          Ret(T::*fun)(Args&&...)) {
        std::function<Ret(T*, Args&&...)> lambda = [fun](T *t, Args&&... args) -> Ret {
            return (t->*fun)(std::forward<Args>(args)...);
        };
        constexpr int arity = detail::_arity<Ret>::value;
        _funs.emplace_back(
			std::make_unique<ClassFun<arity, T, Ret, Args...>>(
                ctx, std::string(fun_name),
                lambda));
    }

    template <typename Ret, typename... Args>
    void _register_member(duk_context *ctx,
                          const char *fun_name,
                          Ret(T::*fun)(Args...)) {
        std::function<Ret(T*, Args...)> lambda = [fun](T *t, Args... args) {
            return (t->*fun)(args...);
        };
        constexpr int arity = detail::_arity<Ret>::value;
        _funs.emplace_back(
            std::make_unique<ClassFun<arity, T, Ret, Args...>>(
                ctx, std::string(fun_name),
                lambda));
    }

    template <typename Ret, typename... Args>
    void _register_member(duk_context *ctx,
                          const char *fun_name,
                          Ret(T::*fun)(Args...) const) {
        std::function<Ret(const T*, Args...)> lambda =
            [fun](const T *t, Args... args) {
                return (t->*fun)(args...);
            };
        constexpr int arity = detail::_arity<Ret>::value;
        _funs.emplace_back(
			std::make_unique<ClassFun<arity, const T, Ret, Args...>>(
                ctx, std::string(fun_name),
                lambda));
    }

    void _register_members(duk_context *ctx) {}

    template <typename M, typename... Ms>
    void _register_members(duk_context *ctx,
                           const char *name,
                           M member,
                           Ms... members) {
        _register_member(ctx, name, member);
        _register_members(ctx, members...);
    }
public:
    Class(duk_context *ctx,
          const std::string &name,
          Members... members) : _name(name) {
        _prototype_name = _name + "_lib";
        detail::PrototypeRegistry::PushNewPrototype(ctx, typeid(T), _prototype_name);
		_register_dtor(ctx);
        _register_ctor(ctx);
        _register_members(ctx, members...);

		// leave constructor function on the stack
		duk_get_prop_string(ctx, -1, "\xFF" "constructor");
		if (duk_is_function(ctx, -1)) {
			// set the prototype of the constructor function
			// invert with the prototype
			duk_insert(ctx, -2); // [constructor, prototype]
			duk_put_prop_string(ctx, -2, "prototype"); // MUST set by prop, duk_set_prototype doesn't work for this case!
		}
		else {
			duk_pop(ctx);
			duk_push_undefined(ctx);
		}
    }
    ~Class() = default;
    Class(const Class &) = delete;
    Class& operator=(const Class &) = delete;
    Class(Class &&other) = default;
    Class& operator=(Class &&other) = default;

	bool hasCtor() const override { 
		return _ctor.operator bool();
	}
};
}

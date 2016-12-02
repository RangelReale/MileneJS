#pragma once

#include "Class.h"
//#include "ClassObj.h"
#include <functional>
//#include "Fun.h"
#include "detail/PrototypeRegistry.h"
//#include "Obj.h"
//#include "util.h"
#include <vector>
#include <unordered_map>
#include <memory>
#include <typeindex>

namespace seljs2 {
namespace detail {
template <typename T>
struct lambda_traits : public lambda_traits<decltype(&T::operator())> {};

template <typename T, typename Ret, typename... Args>
struct lambda_traits<Ret(T::*)(Args...) const> {
    using Fun = std::function<Ret(Args...)>;
};
}
class Registry {
private:
    //std::vector<std::unique_ptr<BaseFun>> _funs;
    //std::vector<std::unique_ptr<BaseObj>> _objs;
    std::unordered_map<std::type_index, std::shared_ptr<BaseClass>> _classes;
	//std::vector<std::unique_ptr<BaseClassObj>> _classobjs;
	duk_context *_ctx;
public:
    Registry(duk_context *ctx) : _ctx(ctx) {
        detail::PrototypeRegistry::Create(_ctx);
    }

	/*
	template <typename L>
    void Register(L lambda) {
        Register((typename detail::lambda_traits<L>::Fun)(lambda));
    }

    template <typename Ret, typename... Args>
    void Register(std::function<Ret(Args...)> fun) {
        constexpr int arity = detail::_arity<Ret>::value;
        _funs.emplace_back(
            seljs::make_unique<Fun<arity, Ret, Args...>>(
                _state, fun));
    }

    template <typename Ret, typename... Args>
    void Register(Ret (*fun)(Args...)) {
        constexpr int arity = detail::_arity<Ret>::value;
        _funs.emplace_back(
            seljs::make_unique<Fun<arity, Ret, Args...>>(
                _state, fun));
    }

    template <typename T, typename... Funs>
    void Register(T &t, std::tuple<Funs...> funs) {
        Register(t, funs,
                 typename detail::_indices_builder<sizeof...(Funs)>::type{});
    }

    template <typename T, typename... Funs, size_t... N>
    void Register(T &t, std::tuple<Funs...> funs, detail::_indices<N...>) {
        RegisterObj(t, std::get<N>(funs)...);
    }

    template <typename T, typename... Funs>
    void RegisterObj(T &t, Funs... funs) {
        _objs.emplace_back(seljs::make_unique<Obj<T, Funs...>>(_state, &t, funs...));
    }
	*/

	template <typename T, typename... CtorArgs, typename... Funs, size_t... N>
	std::shared_ptr<BaseClass> RegisterClass(const std::string &name, std::tuple<Funs...> funs,
                       detail::_indices<N...>) {
        return RegisterClassWorker<T, CtorArgs...>(name, std::get<N>(funs)...);
    }

    template <typename T, typename... CtorArgs, typename... Funs>
	std::shared_ptr<BaseClass> RegisterClassWorker(const std::string &name, Funs... funs) {
		std::shared_ptr<BaseClass> c = std::make_shared<Class<T, Ctor<T, CtorArgs...>, Dtor<T>, Funs...>>(
			_ctx, name, funs...);
        _classes.insert(std::make_pair(std::type_index(typeid(T)), c));
		return c;
    }

	template <typename T, typename Constructor, typename Destructor, typename... Funs, size_t... N>
	std::shared_ptr<BaseClass> RegisterClassCustom(const std::string &name, std::tuple<Funs...> funs,
		detail::_indices<N...>) {
		return RegisterClassCustomWorker<T, Constructor, Destructor>(name, std::get<N>(funs)...);
	}

	template <typename T, typename Constructor, typename Destructor, typename... Funs>
	std::shared_ptr<BaseClass> RegisterClassCustomWorker(const std::string &name, Funs... funs) {
		std::shared_ptr<BaseClass> c = std::make_shared<Class<T, Constructor, Destructor, Funs...>>(
			_ctx, name, funs...);
		_classes.insert(std::make_pair(std::type_index(typeid(T)), c));
		return c;
	}

	template <typename T>
	void RegisterClassObj(T &t) {
		_classobjs.emplace_back(seljs::make_unique<ClassObj<T>>(_state, &t));
	}
};
}

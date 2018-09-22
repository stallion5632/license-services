#pragma once

#include <functional>
#include <map>
#include <string>
#include <iostream>

#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>

// 简单的消息分发机制
// ref:https://www.cnblogs.com/qicosmos/archive/2013/04/28/3048919.html
// 《深入应用C++11代码优化与工程级应用》第12章
// 修改：将原先的_Object && object改为左值引用，并去掉std::move,原因是在linux平台下，无法使用messagehandler的成员变量。
template <typename _ReturnArgs = void>
class MessageBus
{
public:
    // regist message
    template< class... _Args, class _Func, class = typename std::enable_if<!std::is_member_function_pointer<_Func>::value>::type>
    void attach(std::string key, _Func && func)
    {
        std::function<_ReturnArgs(_Args...)> fn = [&](_Args... args) { return func(std::forward<_Args>(args)...); };
        m_map.insert(std::make_pair(key, std::move(fn)));
    }

    // non-const member function
    template<class... _Args, class _Class, class... _DeclareArgs, class _Object>
    void attach(std::string key, _ReturnArgs(_Class::*func)(_DeclareArgs...), _Object & object)
    {
        std::function<_ReturnArgs(_Args...)> fn = [&, func](_Args... args) { return (object.*func)(std::forward<_Args>(args)...); };
        m_map.insert(std::make_pair(key, fn));
    }

    // const member function
    template<class... _Args, class _Class, class... _DeclareArgs, class _Object>
    void attach(std::string key, _ReturnArgs(_Class::*func)(_DeclareArgs...) const, _Object & object)
    {
        std::function<_ReturnArgs(_Args...)> fn = [&, func](_Args... args) { return (object.*func)(std::forward<_Args>(args)...); };
        m_map.insert(std::make_pair(key, fn));
    }

    // Broadcast messages, call all the callback functions
    template<typename... _Args>
    void sendMessage(std::string key, _Args... args)
    {
        auto range = m_map.equal_range(key);
        for (auto it = range.first; it != range.second; it++)
        {
            std::function<_ReturnArgs(_Args...)> func = boost::any_cast<std::function<_ReturnArgs(_Args...)>>(it->second);
            func(args...);
        }
    }

    // remove message
    template<typename... _Args>
    void remove(std::string key)
    {
        auto range = m_map.equal_range(key);
        m_map.erase(range.first, range.second);
    }

public:
    MessageBus() = default;
    ~MessageBus() = default;
private:
    MessageBus(const MessageBus&) = delete;
    MessageBus& operator=(const MessageBus&) = delete;

    std::multimap<std::string, boost::any> m_map;
};

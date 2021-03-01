#ifndef FUNCTION_MAXIMA_H
#define FUNCTION_MAXIMA_H
#include <cassert>
#include <iostream>
#include <memory>
#include <set>
#include <stdexcept>

struct InvalidArg : public std::exception {
    virtual const char* what() const noexcept
    {
        return "invalid argument value";
    }
};

template <typename A, typename V>
class FunctionMaxima {
public:
    class point_type {
        std::shared_ptr<std::pair<A, V>> arg_and_val;

        point_type(const A& arg, const V& value)
            : arg_and_val(std::make_shared<std::pair<A, V>>(arg, value))
        {
        }

    public:
        const A& arg() const noexcept
        {
            return (*arg_and_val).first;
        }

        const V& value() const noexcept
        {
            return (*arg_and_val).second;
        }
        friend class FunctionMaxima;
    };

private:
    struct PointArgComp {
        using is_transparent = void;
        bool operator()(const point_type& lhs, const point_type& rhs) const
        {
            return lhs.arg() < rhs.arg();
        }

        bool operator()(const A& lhs, const point_type& rhs) const
        {
            return lhs < rhs.arg();
        }

        bool operator()(const point_type& lhs, const A& rhs) const
        {
            return lhs.arg() < rhs;
        }
    };

    struct PointValThenArgComp {
        bool operator()(const point_type& lhs, const point_type& rhs) const
        {
            if (not(lhs.value() < rhs.value()) and not(rhs.value() < lhs.value())) {
                return lhs.arg() < rhs.arg();
            } else {
                return rhs.value() < lhs.value();
            }
        }
    };

public:
    using size_type = size_t;
    using iterator = typename std::multiset<point_type, PointArgComp>::const_iterator;
    using mx_iterator = typename std::set<point_type, PointValThenArgComp>::const_iterator;

private:
    std::multiset<point_type, PointArgComp> function;
    std::set<point_type, PointValThenArgComp> maxima;

    // Set `ign_it` to `std::end(function)` if nothing to be ignored.
    iterator fun_prev(const iterator& it, const iterator& ign_it) const noexcept;

    // Set `ign_it` to `std::end(function)` if nothing to be ignored.
    iterator fun_next(const iterator& it, const iterator& ign_it) const noexcept;

    bool refers_to_left_maxima(const iterator& it, const iterator& ign_it) const
    {
        return fun_prev(it, ign_it) == std::end(function) or not((*it).value() < (*fun_prev(it, ign_it)).value());
    }

    bool refers_to_right_maxima(const iterator& it, const iterator& ign_it) const
    {
        return fun_next(it, ign_it) == std::end(function) or not((*it).value() < (*fun_next(it, ign_it)).value());
    }

    bool refers_to_local_maxima(const iterator& it, const iterator& ign_it) const
    {
        return refers_to_left_maxima(it, ign_it) and refers_to_right_maxima(it, ign_it);
    }

    void swap(FunctionMaxima& other) noexcept
    {
        this->function.swap(other.function);
        this->maxima.swap(other.maxima);
    }

    // Helper class for `set_value` and `erase` exception safe implementations
    template <size_t MAX_SIZE>
    class MaximaEraseList {
        std::set<point_type, PointValThenArgComp>& maxima;
        std::array<mx_iterator, MAX_SIZE> erase_list;
        size_t size;
        bool processed = false;

    public:
        MaximaEraseList(std::set<point_type, PointValThenArgComp>& maxima)
            : maxima(maxima)
            , erase_list()
            , size(0)
        {
        }

        void add(const mx_iterator& it) noexcept
        {
            assert(size < MAX_SIZE);
            erase_list[size] = it;
            ++size;
        }

        void process() noexcept
        {
            assert(not processed);
            for (size_t i = 0; i < size; ++i) {
                if (erase_list[i] != std::end(maxima)) {
                    maxima.erase(erase_list[i]);
                }
            }
            processed = true;
        }
    };

public:
    FunctionMaxima() = default;
    FunctionMaxima(const FunctionMaxima& other) = default;
    FunctionMaxima& operator=(FunctionMaxima other) noexcept
    {
        this->swap(other);
        return (*this);
    }

    const V& value_at(const A& a) const;

    void set_value(const A& a, const V& v);

    void erase(const A& a);

    iterator begin() const noexcept
    {
        return std::begin(function);
    }

    iterator end() const noexcept
    {
        return std::end(function);
    }

    iterator find(const A& a) const
    {
        return function.find(a);
    }

    mx_iterator mx_begin() const noexcept
    {
        return std::begin(maxima);
    }

    mx_iterator mx_end() const noexcept
    {
        return std::end(maxima);
    }

    size_type size() const noexcept
    {
        return function.size();
    }
};

// Set `ign_it` to `std::end(function)` if nothing to be ignored.
template <typename A, typename V>
typename FunctionMaxima<A, V>::iterator FunctionMaxima<A, V>::fun_prev(
    const iterator& it, const iterator& ign_it) const noexcept
{
    if (it == std::begin(function)) {
        return std::end(function);
    }
    auto result = std::prev(it);
    if (result == ign_it) {
        if (result == std::begin(function)) {
            return std::end(function);
        }
        --result;
    }
    return result;
}

// Set `ign_it` to `std::end(function)` if nothing to be ignored.
template <typename A, typename V>
typename FunctionMaxima<A, V>::iterator FunctionMaxima<A, V>::fun_next(
    const iterator& it, const iterator& ign_it) const noexcept
{
    if (std::next(it) == std::end(function)) {
        return std::end(function);
    }
    auto result = std::next(it);
    if (result == ign_it) {
        ++result;
    }
    return result;
}

template <typename A, typename V>
const V& FunctionMaxima<A, V>::value_at(const A& a) const
{
    auto it = function.find(a);
    if (it == std::end(function)) {
        throw InvalidArg();
    } else {
        return (*it).value();
    }
}

template <typename A, typename V>
void FunctionMaxima<A, V>::set_value(const A& a, const V& v)
{
    constexpr size_t MAX_LEN_TO_ERASE = 4;
    MaximaEraseList<MAX_LEN_TO_ERASE> inserted(maxima), to_erase(maxima);

    auto old_val_it = function.find(a);

    if (old_val_it != std::end(function) and not(v < old_val_it->value()) and not(old_val_it->value() < v)) {
        return;
    }

    if (old_val_it != std::end(function)) {
        to_erase.add(maxima.find(*old_val_it));
    }

    auto new_val_it = function.insert(point_type(a, v));

    try {
        for (auto it : {fun_prev(new_val_it, old_val_it), new_val_it, fun_next(new_val_it, old_val_it)}) {
            if (it != std::end(function)) {
                if (refers_to_local_maxima(it, old_val_it)) {
                    const auto& [mxit, success] = maxima.insert(*it);
                    if (success) {
                        inserted.add(mxit);
                    }
                } else {
                    to_erase.add(maxima.find(*it));
                }
            }
        }
    } catch (...) {
        function.erase(new_val_it);
        inserted.process();
        throw;
    }

    if (old_val_it != std::end(function)) {
        function.erase(old_val_it);
    }

    to_erase.process();
}

template <typename A, typename V>
void FunctionMaxima<A, V>::erase(const A& a)
{
    constexpr size_t MAX_LEN_TO_ERASE = 3;
    MaximaEraseList<MAX_LEN_TO_ERASE> inserted(maxima), to_erase(maxima);

    auto erase_it = function.find(a);
    if (erase_it == std::end(function)) {
        return;
    }

    to_erase.add(maxima.find(*erase_it));

    try {
        for (auto it : {fun_prev(erase_it, erase_it), fun_next(erase_it, erase_it)}) {
            if (it != std::end(function)) {
                const auto& [mxit, success] = maxima.insert(*it);
                if (success) {
                    inserted.add(mxit);
                }
                if (not refers_to_local_maxima(it, erase_it)) {
                    to_erase.add(mxit);
                }
            }
        }
    } catch (...) {
        inserted.process();
        throw;
    }

    function.erase(erase_it);
    to_erase.process();
}
#endif

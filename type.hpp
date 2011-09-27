#pragma once

#include <boost/variant.hpp>
#include <functional>

namespace backend {

class monotype_t;
class polytype_t;
class sequence_t;
class tuple_t;
class fn_t;
class int32_mt;
class int64_mt;
class uint32_mt;
class uint64_mt;
class float32_mt;
class float64_mt;
class bool_mt;
class void_mt;

namespace detail {
typedef boost::variant<
    monotype_t &,
    polytype_t &,
    sequence_t &,
    tuple_t &,
    fn_t &,
    int32_mt &,
    int64_mt &,
    uint32_mt &,
    uint64_mt &,
    float32_mt &,
    float64_mt &,
    bool_mt &,
    void_mt &
    > type_base;

struct make_type_base_visitor
    : boost::static_visitor<type_base>
{
    make_type_base_visitor(void *p)
        : ptr(p)
        {}
    template<typename Derived>
    type_base operator()(const Derived &) const {
        // use of std::ref disambiguates variant's copy constructor dispatch
        return type_base(std::ref(*reinterpret_cast<Derived*>(ptr)));
    }
    void *ptr;
};

type_base make_type_base(void *ptr, const type_base &other) {
    return boost::apply_visitor(make_type_base_visitor(ptr), other);
}

}

class type_t
    : public detail::type_base
{
public:
    typedef detail::type_base super_t;
    template<typename Derived>
    type_t(Derived &self)
        : super_t(std::ref(self)) //use of std::ref disambiguates variant's copy constructor dispatch
        {}

    type_t(const type_t &other)
        : super_t(detail::make_type_base(this, other))
        {}

};

class polytype_t :
        public type_t {
    polytype_t() : type_t(*this) {}

};

}

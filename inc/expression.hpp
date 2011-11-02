#pragma once

#include "node.hpp"
#include <vector>
#include <memory>
#include <ostream>
#include <boost/iterator/indirect_iterator.hpp>

#include "type.hpp"
#include "monotype.hpp"
#include "ctype.hpp"

namespace backend {



class expression
    : public node
{
protected:
    std::shared_ptr<type_t> m_type;
    std::shared_ptr<ctype::type_t> m_ctype;
    template<typename Derived>
    expression(Derived &self,
               const std::shared_ptr<type_t>& type = void_mt,
               const std::shared_ptr<ctype::type_t>& ctype = ctype::void_mt)
        : node(self), m_type(type), m_ctype(ctype)
        {}
public:
    const type_t& type(void) const;
    const ctype::type_t& ctype(void) const;
};

class literal
    : public expression
{
protected:
    const std::string m_val;
public:
    template<typename Derived>
    literal(Derived &self,
            const std::string& val,
            const std::shared_ptr<type_t>& type = void_mt,
            const std::shared_ptr<ctype::type_t>& ctype = ctype::void_mt)
        : expression(self, type, ctype), m_val(val)
        {}
    literal(const std::string& val,
            const std::shared_ptr<type_t>& type = void_mt,
            const std::shared_ptr<ctype::type_t>& ctype = ctype::void_mt);
    
    const std::string& id(void) const;

};

class name
    : public literal
{   
public:
    name(const std::string &val,
         const std::shared_ptr<type_t>& type = void_mt,
         const std::shared_ptr<ctype::type_t>& ctype = ctype::void_mt);
    
    template<typename Derived>
    name(Derived& self, const std::string &val,
         const std::shared_ptr<type_t>& type,
         const std::shared_ptr<ctype::type_t>& ctype) :
        literal(self, val, type, ctype) {}
};

class tuple
    : public expression
{
public:
    tuple(std::vector<std::shared_ptr<expression> > &&values,
          const std::shared_ptr<type_t>& type = void_mt,
          const std::shared_ptr<ctype::type_t>& ctype = ctype::void_mt);
protected:
    const std::vector<std::shared_ptr<expression> > m_values;
public:
    typedef decltype(boost::make_indirect_iterator(m_values.cbegin())) const_iterator;
    const_iterator begin() const;

    const_iterator end() const;
    
    int arity() const;
};

class apply
    : public expression
{
protected:
    const std::shared_ptr<name> m_fn;
    const std::shared_ptr<tuple> m_args;
public:
    apply(const std::shared_ptr<name> &fn,
          const std::shared_ptr<tuple> &args);
    
    const name &fn(void) const;
    
    const tuple &args(void) const;
};

class lambda
    : public expression
{
protected:
    const std::shared_ptr<tuple> m_args;
    const std::shared_ptr<expression> m_body;
public:
    lambda(const std::shared_ptr<tuple> &args,
           const std::shared_ptr<expression> &body,
           const std::shared_ptr<type_t>& type = void_mt,
           const std::shared_ptr<ctype::type_t>& ctype = ctype::void_mt);
    
    const tuple &args(void) const;
    
    const expression &body(void) const;
};

class closure
    : public expression
{
protected:
    const std::shared_ptr<tuple> m_args;
    const std::shared_ptr<expression> m_body;
    
public:
    closure(const std::shared_ptr<tuple> &args,
            const std::shared_ptr<expression> &body,
            const std::shared_ptr<type_t>& type = void_mt,
            const std::shared_ptr<ctype::type_t>& ctype = ctype::void_mt);
    const tuple &args(void) const;
    const expression &body(void) const;
};


}
    

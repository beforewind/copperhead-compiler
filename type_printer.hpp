#pragma once

#include "monotype.hpp"
#include "ctype.hpp"

namespace backend
{

class repr_type_printer
    : public boost::static_visitor<>
{
public:
    inline repr_type_printer(std::ostream &os)
        : m_os(os)
        {}
    inline void operator()(const monotype_t &mt) {
        m_os << mt.name();
    }
    inline void operator()(const polytype_t &pt) {
    }
    inline void operator()(const sequence_t &st) {
        m_os << st.name();
        open(); boost::apply_visitor(*this, st.sub()); close();
    }
    inline void operator()(const fn_t &ft) {
        m_os << ft.name();
        open();
        boost::apply_visitor(*this, ft.args());
        boost::apply_visitor(*this, ft.result());
        close();
    }
    inline void operator()(const tuple_t &tt) {
        m_os << tt.name();
        open();
        for(auto i = tt.begin();
            i != tt.end();
            i++) {
            boost::apply_visitor(*this, *i);
            if (std::next(i) != tt.end())
                sep();
        }
        close();
    }
        
private:
    std::ostream &m_os;
    inline void sep() const {
        m_os << ", ";
    }
    inline void open() const {
        m_os << "(";
    }
    inline void close() const {
        m_os << ")";
    }
};

namespace ctype {
class ctype_printer
    : public boost::static_visitor<>
{
public:
    inline ctype_printer(std::ostream &os)
        : m_os(os)
        {}
    inline void operator()(const monotype_t &mt) {
        m_os << mt.name();
    }
    inline void operator()(const sequence_t &st) {
        m_os << st.name();
        open();
        boost::apply_visitor(*this, st.sub());
        close();
    }
    inline void operator()(const polytype_t &pt) {
    }
private:
    std::ostream &m_os;
    inline void sep() const {
        m_os << ", ";
    }
    inline void open() const {
        m_os << "(";
    }
    inline void close() const {
        m_os << ")";
    }
};
}
}

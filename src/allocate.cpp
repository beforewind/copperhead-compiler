#include "allocate.hpp"

using std::vector;
using std::shared_ptr;
using std::make_shared;
using std::move;
using std::string;
using std::static_pointer_cast;
using backend::utility::make_vector;


namespace backend {


allocate::allocate(const copperhead::system_variant& target,
                   const string& entry_point) : m_target(target),
                                                m_entry_point(entry_point),
                                                m_in_entry(false),
                                                m_allocations()
        {}

allocate::result_type allocate::operator()(const procedure &n) {
    if (n.id().id()  == m_entry_point) {
        m_in_entry = true;
        vector<shared_ptr<const statement> > statements;
        for(auto i = n.stmts().begin();
            i != n.stmts().end();
            i++) {
            auto new_stmt = static_pointer_cast<const statement>(
                boost::apply_visitor(*this, *i));
            if (m_allocations.size() > 0) {
                for(auto j = m_allocations.begin();
                    j != m_allocations.end();
                    j++) {
                    statements.push_back(*j);
                }
                m_allocations.clear();
            }
            statements.push_back(new_stmt);
        }
        auto stmts = make_shared<const suite>(move(statements));
        auto args =
            static_pointer_cast<const tuple>(
                boost::apply_visitor(*this, n.args()));
        auto t = n.type().ptr();
        auto ct = n.ctype().ptr();
        auto id =
            static_pointer_cast<const name>(
                boost::apply_visitor(*this, n.id()));
        result_type allocated = make_shared<const procedure>(
            id, args, stmts, t, ct);

        m_in_entry = false;
        return allocated;
    } else {
        return this->rewriter::operator()(n);
    }
}

allocate::result_type allocate::operator()(const bind &n) {
    if (m_in_entry &&
        detail::isinstance<ctype::sequence_t>(
            n.lhs().ctype()) &&
        detail::isinstance<apply>(n.rhs())) {

        //We can only deal with names on the LHS of a bind
        //TUPLE FIX
        bool lhs_is_name = detail::isinstance<name>(n.lhs());
        assert(lhs_is_name);
        const name& pre_lhs = boost::get<const name&>(n.lhs());

        //Construct cuarray for result
        const ctype::sequence_t& pre_lhs_ct =
            boost::get<const ctype::sequence_t&>(pre_lhs.ctype());
        shared_ptr<const ctype::type_t> sub_lhs_ct =
            pre_lhs_ct.sub().ptr();
        shared_ptr<const ctype::type_t> impl_seq_ct =
            make_shared<const ctype::polytype_t>(
                make_vector<shared_ptr<const ctype::type_t> >
                (make_shared<const ctype::monotype_t>(copperhead::to_string(m_target)))
                (sub_lhs_ct),
                make_shared<const ctype::monotype_t>("sequence"));
        
        shared_ptr<const ctype::tuple_t> tuple_impl_seq_ct =
            make_shared<const ctype::tuple_t>(
                make_vector<shared_ptr<const ctype::type_t> >(impl_seq_ct));
        shared_ptr<const ctype::type_t> result_ct =
            make_shared<const ctype::monotype_t>("sp_cuarray");
        shared_ptr<const type_t> result_t =
            pre_lhs.type().ptr();
        shared_ptr<const name> result_name = make_shared<const name>(
            detail::wrap_array_id(pre_lhs.id()),
            result_t,
            result_ct);

        shared_ptr<const expression> new_rhs =
            static_pointer_cast<const expression>(
                boost::apply_visitor(*this, n.rhs()));
            
        shared_ptr<const bind> allocator = make_shared<const bind>(
            result_name, new_rhs);
        m_allocations.push_back(allocator);
            

        shared_ptr<const name> new_lhs = static_pointer_cast<const name>(
            boost::apply_visitor(*this, n.lhs()));
        shared_ptr<const templated_name> getter_name =
            make_shared<const templated_name>(
                detail::make_sequence(),
                tuple_impl_seq_ct);
        shared_ptr<const tuple> getter_args =
            make_shared<const tuple>(
                make_vector<shared_ptr<const expression> >(result_name)
                (make_shared<const apply>(
                    make_shared<const name>(copperhead::to_string(m_target)),
                    make_shared<const tuple>(make_vector<shared_ptr<const expression> >())))
                (make_shared<const literal>("true")));
        shared_ptr<const apply> getter_call =
            make_shared<const apply>(getter_name, getter_args);
        shared_ptr<const bind> retriever =
            make_shared<const bind>(new_lhs, getter_call);
        return retriever;
    } else {
        return this->rewriter::operator()(n);
    }
}


}

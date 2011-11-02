#include "thrust/decl.hpp"

namespace backend {

namespace detail {
std::vector<const char*> thrust_fn_names = {
    "adjacent_difference",
    "map1", "map2", "map3", "map4", "map5",
    "map6", "map7", "map8", "map9", "map10",
    "scan",
    "rscan",
    "indices",
    "permute"/*,
    "exscan",
    "exrscan"*/
};

}


std::shared_ptr<library> get_thrust() {
    std::map<ident, fn_info> exported_fns;
    for(auto i = detail::thrust_fn_names.begin();
        i != detail::thrust_fn_names.end();
        i++) {
        exported_fns.insert(std::pair<ident, fn_info>(
                       ident(std::string(*i), iteration_structure::parallel),
                       //XXX Need to put real types in
                       fn_info(void_mt,
                               std::make_shared<phase_t>(
                                   std::vector<completion>{},
                                   completion::invariant)

                           )));

    }
    //XXX HACK.  NEED boost::filesystem path manipulation
    std::string library_path(std::string(detail::get_path(PRELUDE_PATH)) +
                             "/../thrust");
    std::string thrust_path(std::string(detail::get_path(THRUST_PATH)));
    std::set<std::string> include_paths;
    if (library_path.length() > 0) {
        include_paths.insert(library_path);
    }
    if (thrust_path.length() > 0) {
        include_paths.insert(thrust_path);
    }
    return std::shared_ptr<library>(
        new library(std::move(exported_fns),
                    std::set<std::string>{std::string(THRUST_FILE)},
                    std::move(include_paths)));
}


}
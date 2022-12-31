#include <map>
#include <vector>
#include <boost/json.hpp>
#include <boost/assign.hpp>
#include <iostream>

// Only list permissions whose value is true
std::map<boost::json::string, bool> user_perms = {};
std::map<boost::json::string, bool> manager_perms = boost::assign::map_list_of ("modify_announcement", true) ("modify_movie", true) ("view_user", true) ("modify_user", true)("modify_room", true);
std::map<boost::json::string, bool> admin_perms = boost::assign::map_list_of ("modify_announcement", true) ("modify_movie", true) ("view_user", true) ("modify_user", true)("modify_room", true);

std::map<boost::json::string, std::map<boost::json::string, bool>> m = boost::assign::map_list_of ("user", user_perms) ("manager", manager_perms) ("admin", admin_perms);

boost::json::object get_permission_for_session(boost::json::object session) {
    if (!session.count("user")) {
        return {};
    }
    boost::json::object user = session["user"].as_object();
    boost::json::string roal = user["roal"].as_string();
    boost::json::object o;
    for (auto& [key, value] : m[roal]) {
        o[key] = value;
    }
    return o;
}

bool check_permission(const boost::json::string& user_type, const boost::json::string& permission) {
    if (m.find(user_type) == m.end()) {
        return false;
    }
    if (m[user_type].find(permission) == m[user_type].end()) {
        std::cout << "[Debug] Permission not found " << permission << std::endl;
        return false;
    }
    return m[user_type][permission];
}

std::optional<boost::json::object> check_session_permission(boost::json::object session, boost::json::string permission) {
    if (!session.count("user")) {
        return {{{"success", false}, {"message", "Not logged in"}}};
    }
    boost::json::object user = session["user"].as_object();
    boost::json::string roal = user["roal"].as_string();
    if (!check_permission(roal, permission)) {
        return {{{"success", false}, {"message", "Permission denied"}}};
    }
    return std::nullopt;
}
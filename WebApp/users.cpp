#include <vector>
#include "handlers.h"
#include "rendering.h"

bserv::db_relation_to_object orm_user{
    bserv::make_db_field<int>("id"),
    bserv::make_db_field<std::string>("username"),
    bserv::make_db_field<std::string>("password"),
    bserv::make_db_field<bool>("is_superuser"),
    bserv::make_db_field<std::string>("first_name"),
    bserv::make_db_field<std::string>("last_name"),
    bserv::make_db_field<std::string>("email"),
    bserv::make_db_field<bool>("is_active"),
    bserv::make_db_field<std::string>("roal"),
};

std::optional<boost::json::object> get_user(
    bserv::db_transaction& tx,
    const boost::json::string& username) {
    bserv::db_result r =
        tx.exec("select * from auth_user where username = ?", username);
    lginfo << r.query();  // this is how you log info
    return orm_user.convert_to_optional(r);
}

std::optional<boost::json::object> get_user_by_id(bserv::db_transaction& tx,
                                                  const int id) {
    bserv::db_result r = tx.exec("select * from auth_user where id = ?", id);
    lginfo << r.query();  // this is how you log info
    return orm_user.convert_to_optional(r);
}

boost::json::object user_register(bserv::request_type& request,
                                  // the json object is obtained from the
                                  // request body, as well as the url parameters
                                  boost::json::object&& params,
                                  std::shared_ptr<bserv::db_connection> conn) {
    if (request.method() != boost::beast::http::verb::post) {
        throw bserv::url_not_found_exception{};
    }
    if (params.count("username") == 0) {
        return {{"success", false}, {"message", "Username is required"}};
    }
    if (params.count("password") == 0) {
        return {{"success", false}, {"message", "Password is required"}};
    }
    auto username = params["username"].as_string();
    bserv::db_transaction tx{conn};
    auto opt_user = get_user(tx, username);
    if (opt_user.has_value()) {
        return {{"success", false}, {"message", "Username exists"}};
    }
    auto password = params["password"].as_string();
    bserv::db_result r = tx.exec(
        "insert into ? "
        "(?, password, is_superuser, "
        "first_name, last_name, email, is_active, roal) values "
        "(?, ?, ?, ?, ?, ?, ?, ?)",
        bserv::db_name("auth_user"), bserv::db_name("username"), username,
        bserv::utils::security::encode_password(password.c_str()), false,
        get_or_empty(params, "first_name"), get_or_empty(params, "last_name"),
        get_or_empty(params, "email"), true, get_or_empty(params, "roal"));
    lginfo << r.query();
    tx.commit();  // you must manually commit changes
    return {{"success", true}, {"message", "User registered"}};
}

boost::json::object user_modify(bserv::request_type& request,
                                // the json object is obtained from the request
                                // body, as well as the url parameters
                                boost::json::object&& params,
                                std::shared_ptr<bserv::db_connection> conn,
                                std::shared_ptr<bserv::session_type> session_ptr,
                                int user_id) {
    std::optional<boost::json::object> p;
    if ((p = check_session_permission(*session_ptr, "modify_user")) !=
        std::nullopt) {
        return p.value();
    }

    if (request.method() != boost::beast::http::verb::post) {
        throw bserv::url_not_found_exception{};
    }
    bserv::db_transaction tx{conn};
    auto opt_user = get_user_by_id(tx, user_id);
    if (!opt_user.has_value()) {
        return {{"success", false}, {"message", "User does not exist"}};
    }
    auto password = get_or_empty(params, "password");
    bserv::db_result r = tx.exec(
        "update ? "
        "set username = COALESCE(NULLIF(?, ''), username), "
        "password = COALESCE(NULLIF(?, ''), password), "
        "first_name = COALESCE(NULLIF(?, ''), first_name), "
        "last_name = COALESCE(NULLIF(?, ''), last_name), "
        "email = COALESCE(NULLIF(?, ''), email), "
        "roal = COALESCE(NULLIF(?, ''), roal) "
        "where id = ?",
        bserv::db_name("auth_user"), get_or_empty(params, "username"),
        password.empty()
            ? password
            : bserv::utils::security::encode_password(password.c_str()),
        get_or_empty(params, "first_name"), get_or_empty(params, "last_name"),
        get_or_empty(params, "email"), get_or_empty(params, "roal"), user_id);
    lginfo << r.query();
    tx.commit();  // you must manually commit changes
    return {{"success", true}, {"message", "User modified"}};
}

boost::json::object user_login(
    bserv::request_type& request,
    boost::json::object&& params,
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr) {
    if (request.method() != boost::beast::http::verb::post) {
        throw bserv::url_not_found_exception{};
    }
    if (params.count("username") == 0) {
        return {{"success", false}, {"message", "`username` is required"}};
    }
    if (params.count("password") == 0) {
        return {{"success", false}, {"message", "`password` is required"}};
    }
    auto username = params["username"].as_string();
    bserv::db_transaction tx{conn};
    auto opt_user = get_user(tx, username);
    if (!opt_user.has_value()) {
        return {{"success", false}, {"message", "invalid username/password"}};
    }
    auto& user = opt_user.value();
    if (!user["is_active"].as_bool()) {
        return {{"success", false}, {"message", "invalid username/password"}};
    }
    auto password = params["password"].as_string();
    auto encoded_password = user["password"].as_string();
    if (!bserv::utils::security::check_password(password.c_str(),
                                                encoded_password.c_str())) {
        return {{"success", false}, {"message", "invalid username/password"}};
    }
    bserv::session_type& session = *session_ptr;
    session["user"] = user;
    return {{"success", true}, {"message", "Login successful"}};
}

boost::json::object find_user(std::shared_ptr<bserv::db_connection> conn,
                              const std::string& username) {
    bserv::db_transaction tx{conn};
    auto user = get_user(tx, username.c_str());
    if (!user.has_value()) {
        return {{"success", false},
                {"message", "The requested user does not exist"}};
    }
    user.value().erase("id");
    user.value().erase("password");
    return {{"success", true}, {"user", user.value()}};
}

boost::json::object user_logout(
    std::shared_ptr<bserv::session_type> session_ptr) {
    bserv::session_type& session = *session_ptr;
    if (session.count("user")) {
        session.erase("user");
    }
    return {{"success", true}, {"message", "Logout successful"}};
}

std::nullopt_t form_login(bserv::request_type& request,
                          bserv::response_type& response,
                          boost::json::object&& params,
                          std::shared_ptr<bserv::db_connection> conn,
                          std::shared_ptr<bserv::session_type> session_ptr) {
    lgdebug << params << std::endl;
    auto context = user_login(request, std::move(params), conn, session_ptr);
    lginfo << "login: " << context << std::endl;
    return redirect_to_movies(conn, session_ptr, response, 1,
                              std::move(context));
}

std::nullopt_t form_logout(std::shared_ptr<bserv::db_connection> conn,
                           std::shared_ptr<bserv::session_type> session_ptr,
                           bserv::response_type& response) {
    auto context = user_logout(session_ptr);
    lginfo << "logout: " << context << std::endl;
    return redirect_to_movies(conn, session_ptr, response, 1,
                              std::move(context));
}

std::nullopt_t redirect_to_users(
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr,
    bserv::response_type& response,
    int page_id,
    boost::json::object&& context) {
    std::optional<boost::json::object> p;
    if ((p = check_session_permission(*session_ptr, "modify_user")) !=
        std::nullopt) {
        throw std::logic_error("Permission denied");
    }

    lgdebug << "view users: " << page_id << std::endl;
    bserv::db_transaction tx{conn};
    bserv::db_result db_res = tx.exec("select count(*) from auth_user;");
    lginfo << db_res.query();
    std::size_t total_users = (*db_res.begin())[0].as<std::size_t>();
    lgdebug << "total users: " << total_users << std::endl;
    int total_pages = (int)total_users / 10;
    if (total_users % 10 != 0)
        ++total_pages;
    lgdebug << "total pages: " << total_pages << std::endl;
    db_res = tx.exec("select * from auth_user limit 10 offset ?;",
                     (page_id - 1) * 10);
    lginfo << db_res.query();
    auto users = orm_user.convert_to_vector(db_res);
    boost::json::array json_users;
    for (auto& user : users) {
        json_users.push_back(user);
    }
    boost::json::object pagination;
    if (total_pages != 0) {
        pagination["total"] = total_pages;
        if (page_id > 1) {
            pagination["previous"] = page_id - 1;
        }
        if (page_id < total_pages) {
            pagination["next"] = page_id + 1;
        }
        int lower = page_id - 3;
        int upper = page_id + 3;
        if (page_id - 3 > 2) {
            pagination["left_ellipsis"] = true;
        } else {
            lower = 1;
        }
        if (page_id + 3 < total_pages - 1) {
            pagination["right_ellipsis"] = true;
        } else {
            upper = total_pages;
        }
        pagination["current"] = page_id;
        boost::json::array pages_left;
        for (int i = lower; i < page_id; ++i) {
            pages_left.push_back(i);
        }
        pagination["pages_left"] = pages_left;
        boost::json::array pages_right;
        for (int i = page_id + 1; i <= upper; ++i) {
            pages_right.push_back(i);
        }
        pagination["pages_right"] = pages_right;
        context["pagination"] = pagination;
    }
    context["users"] = json_users;
    context["permission"] = get_permission_for_session(*session_ptr);
    return index("users.html", session_ptr, response, context);
}

std::nullopt_t view_users(std::shared_ptr<bserv::db_connection> conn,
                          std::shared_ptr<bserv::session_type> session_ptr,
                          bserv::response_type& response,
                          const std::string& page_num) {
    int page_id = std::stoi(page_num);
    boost::json::object context;
    return redirect_to_users(conn, session_ptr, response, page_id,
                             std::move(context));
}

std::nullopt_t form_add_user(bserv::request_type& request,
                             bserv::response_type& response,
                             boost::json::object&& params,
                             std::shared_ptr<bserv::db_connection> conn,
                             std::shared_ptr<bserv::session_type> session_ptr) {
    boost::json::object context =
        user_register(request, std::move(params), conn);
    return redirect_to_users(conn, session_ptr, response, 1,
                             std::move(context));
}

std::nullopt_t form_modify_user(
    bserv::request_type& request,
    bserv::response_type& response,
    boost::json::object&& params,
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr,
    const std::string& user_id) {
    std::optional<boost::json::object> p;
    int usr_id = std::stoi(user_id);
    boost::json::object context =
        user_modify(request, std::move(params), conn, session_ptr, usr_id);
    return redirect_to_users(conn, session_ptr, response, 1,
                             std::move(context));
}
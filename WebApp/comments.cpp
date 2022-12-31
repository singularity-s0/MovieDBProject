#include <vector>
#include "handlers.h"
#include "rendering.h"

bserv::db_relation_to_object orm_comment{
    bserv::make_db_field<int>("comment_id"),
    bserv::make_db_field<int>("id"),
    bserv::make_db_field<int>("movie_id"),
    bserv::make_db_field<int>("rating"),
    bserv::make_db_field<std::string>("content"),
};

std::nullopt_t manage_comments(
    bserv::request_type& request,
    bserv::response_type& response,
    boost::json::object&& params,
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr,
    const std::string& movie_id) {
    std::optional<boost::json::object> p;
    if ((p = check_session_permission(*session_ptr, "modify_movie")) !=
        std::nullopt) {
        throw std::runtime_error("Permission denied");
    }

    boost::json::object context;
    try {
        int mov_id = std::stoi(movie_id);
        bserv::db_transaction tx{conn};
        auto opt_movie = get_movie_by_id(tx, mov_id);
        if (!opt_movie.has_value()) {
            throw std::exception{};
        }
        context["movie"] = opt_movie.value();
    } catch (const std::exception& e) {
        throw std::runtime_error("Invalid movie id");
    }

    bserv::db_transaction tx{conn};
    bserv::db_result db_res =
        tx.exec("select * from comments where movie_id = ?;", movie_id);
    lginfo << db_res.query();
    auto comments = orm_comment.convert_to_vector(db_res);
    boost::json::array json_comments;
    for (auto& comment : comments) {
        json_comments.push_back(comment);
    }
    context["comments"] = json_comments;

    boost::json::array placeholder;
    boost::json::object placeholder_obj;
    placeholder_obj["id"] = 1;
    placeholder_obj["username"] = "user";
    placeholder.push_back(placeholder_obj);
    context["users"] = placeholder;

    context["permission"] = get_permission_for_session(*session_ptr);
    return index("managecomments.html", session_ptr, response, context);
}

std::nullopt_t form_add_comment(
    bserv::request_type& request,
    bserv::response_type& response,
    boost::json::object&& params,
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr,
    const std::string& movie_id) {
    if (request.method() != boost::beast::http::verb::post) {
        throw bserv::url_not_found_exception{};
    }

    std::optional<boost::json::object> p;
    if ((p = check_session_permission(*session_ptr, "modify_comment")) !=
        std::nullopt) {
        throw std::runtime_error("Permission denied");
    }

    // All parameters must not be empty
    if (get_or_empty(params, "id").empty() ||
        get_or_empty(params, "rating").empty() ||
        get_or_empty(params, "content").empty()) {
        throw std::runtime_error("Invalid parameters");
    }

    bserv::db_transaction tx{conn};
    try {
        bserv::db_result r = tx.exec(
            "insert into comments (id, movie_id, rating, content) "
            "values (?, ?, ?, ?);",
            get_stoi_or_zero(params, "id"), 
            std::stoi(movie_id),
            get_stoi_or_zero(params, "rating"),
            get_or_empty(params, "content"));
        lginfo << r.query();
        tx.commit();
    } catch (const std::exception& e) {
        throw std::runtime_error(e.what());
    }

    return manage_comments(request, response, std::move(params), conn,
                             session_ptr, movie_id);
}

std::nullopt_t form_modify_comment(
    bserv::request_type& request,
    bserv::response_type& response,
    boost::json::object&& params,
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr,
    const std::string& comment_id) {
    if (request.method() != boost::beast::http::verb::post) {
        throw bserv::url_not_found_exception{};
    }

    std::optional<boost::json::object> p;
    if ((p = check_session_permission(*session_ptr, "modify_comment")) !=
        std::nullopt) {
        throw std::runtime_error("Permission denied");
    }

    bserv::db_transaction tx{conn};
    bserv::db_result r = tx.exec(
        "update comments set id = COALESCE(NULLIF(?, 0), id), "
        "rating = COALESCE(NULLIF(?, 0), rating),"
        "content = COALESCE(NULLIF(?, ''), content) "
        "where comment_id = ?;",
        get_stoi_or_zero(params, "id"), 
        get_stoi_or_zero(params, "rating"), 
        get_or_empty(params, "content"), 
        std::stoi(comment_id));
    lginfo << r.query();
    tx.commit();
    return manage_comments(request, response, std::move(params), conn,
                             session_ptr, get_or_empty(params, "movie_id"));
}
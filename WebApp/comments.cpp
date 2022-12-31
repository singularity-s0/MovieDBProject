#include <vector>
#include "handlers.h"
#include "rendering.h"

extern bserv::db_relation_to_object orm_ticket;

bserv::db_relation_to_object orm_comment{
    bserv::make_db_field<int>("comment_id"),
    bserv::make_db_field<int>("id"), // user id
    bserv::make_db_field<int>("movie_id"),
    bserv::make_db_field<int>("rating"),
    bserv::make_db_field<std::string>("content"),
};

std::nullopt_t redirect_to_comments(
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr,
    bserv::response_type& response,
    boost::json::object&& context,
    const std::string& movie_id) {
    // std::optional<boost::json::object> p;
    // if ((p = check_session_permission(*session_ptr, "modify_movie")) !=
    //     std::nullopt) {
    //     throw std::runtime_error("Permission denied");
    // }

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

    context["permission"] = get_permission_for_session(*session_ptr);
    return index("comments.html", session_ptr, response, context);
}

boost::json::object add_comment(
    bserv::request_type& request,
    boost::json::object&& params,
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr,
    const std::string& movie_id) {
    if (request.method() != boost::beast::http::verb::post) {
        throw bserv::url_not_found_exception{};
    }

    std::optional<boost::json::object> p;
    if ((p = check_session_permission(*session_ptr, "add_comment")) !=
        std::nullopt) {
        return p.value();
    }

    boost::json::object session = *session_ptr;
    boost::json::object user = session["user"].as_object();
    int user_id = user["id"].as_int64();

    // All parameters must not be empty
    if (get_or_empty(params, "rating").empty() ||
        get_or_empty(params, "content").empty()) {
        return {{"success", false},
                {"message", "Rating and content must not be empty"}};
    } else if (get_stoi_or_zero(params, "rating") < 0 ||
               get_stoi_or_zero(params, "rating") > 5) {
        return {{"success", false}, {"message", "Rating must be between 0 and 5"}};
    }

    int mov_id;
    try {
        mov_id = std::stoi(movie_id);
    } catch (const std::exception& e) {
        return {{"success", false}, {"message", "Invalid movie id"}};
    }

    bserv::db_transaction tx{conn};
    try {
        // Check if user has already commented on this movie
        bserv::db_result r = tx.exec(
            "select * from comments where id = ? and movie_id = ?;",
            user_id,
            mov_id);
        lginfo << r.query();
        auto comments = orm_comment.convert_to_vector(r);
        if (!comments.empty()) {
            return {{"success", false}, {"message", "You have already commented"}};
        }
        // Check if user has ticket for this movie
        r = tx.exec(
            "select * from tickets, screenings where user_id = ? and refunded=false and "
            "tickets.screening_id = screenings.screening_id and movie_id = ?;",
            user_id,
            mov_id);
        lginfo << r.query();
        auto tickets = orm_ticket.convert_to_vector(r);
        if (tickets.empty()) {
            return {{"success", false}, {"message", "You must buy a ticket first to comment"}};
        }

        r = tx.exec(
            "insert into comments (id, movie_id, rating, content) "
            "values (?, ?, ?, ?);",
            user_id, 
            mov_id,
            get_stoi_or_zero(params, "rating"),
            get_or_empty(params, "content"));
        lginfo << r.query();
        // Update movie avg rating
        r = tx.exec(
            "update movies set avg_rating = (select avg(rating) from comments where movie_id = ?) "
            "where movie_id = ?;",
            mov_id, mov_id);
        lginfo << r.query();
        tx.commit();
    } catch (const std::exception& e) {
        lgerror << e.what();
        return {{"success", false}, {"message", e.what()}};
    }
    return {{"success", true}, {"message", "Comment added"}};
}

std::nullopt_t form_add_comment(
    bserv::request_type& request,
    bserv::response_type& response,
    boost::json::object&& params,
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr,
    const std::string& movie_id) {
    boost::json::object context = add_comment(
        request, std::move(params), conn, session_ptr, movie_id);
    return redirect_to_comments(conn, session_ptr, response, std::move(context), movie_id);
}

boost::json::object modify_comment(bserv::request_type& request,
                                 boost::json::object&& params,
                                 std::shared_ptr<bserv::db_connection> conn,
                                 std::shared_ptr<bserv::session_type> session_ptr,
                                 const std::string& comment_id) {
    if (request.method() != boost::beast::http::verb::post) {
        throw bserv::url_not_found_exception{};
    }

    boost::json::object session = *session_ptr;
    boost::json::object user = session["user"].as_object();
    int user_id = user["id"].as_int64();
    int cmt_id;
    try {
        cmt_id = std::stoi(comment_id);
    } catch (const std::exception& e) {
        return {{"success", false}, {"message", "Invalid comment id"}};
    }

    // Skip permission check if user is owner of comment
    bserv::db_transaction tx{conn};
    bserv::db_result r = tx.exec("select * from comments where id = ? and comment_id=?;",
                               user_id, cmt_id);
    lginfo << r.query();
    auto comments = orm_comment.convert_to_vector(r);
    if (comments.empty()) {
        std::optional<boost::json::object> p;
        if ((p = check_session_permission(*session_ptr, "modify_comment")) !=
            std::nullopt) {
            return p.value();
        }
    }
    int movie_id = comments[0]["movie_id"].as_int64();

    r = tx.exec(
        "update comments set "
        "rating = COALESCE(NULLIF(?, 0), rating),"
        "content = COALESCE(NULLIF(?, ''), content) "
        "where comment_id = ?;", 
        get_stoi_or_zero(params, "rating"), 
        get_or_empty(params, "content"), 
        cmt_id);
    lginfo << r.query();

    // Update movie avg rating
    r = tx.exec(
        "update movies set avg_rating = (select avg(rating) from comments where movie_id = ?) "
        "where movie_id = ?;",
        movie_id, movie_id);
    lginfo << r.query();

    tx.commit();
    return {{"success", true}, {"message", "Comment modified"}, {"movie_id", movie_id}};
}

std::nullopt_t form_modify_comment(
    bserv::request_type& request,
    bserv::response_type& response,
    boost::json::object&& params,
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr,
    const std::string& comment_id) {
    boost::json::object context = modify_comment(
        request, std::move(params), conn, session_ptr, comment_id);
    std::string movie_id = std::to_string(context["movie_id"].as_int64());
    return redirect_to_comments(conn, session_ptr, response, std::move(context), std::move(movie_id));
}

std::nullopt_t view_comments(std::shared_ptr<bserv::db_connection> conn,
                           std::shared_ptr<bserv::session_type> session_ptr,
                           bserv::response_type& response, const std::string& movie_id) {
    boost::json::object context;
    return redirect_to_comments(conn, session_ptr, response,
                              std::move(context), movie_id);
}
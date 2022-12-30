#include <vector>
#include "handlers.h"
#include "rendering.h"

bserv::db_relation_to_object orm_screening{
    bserv::make_db_field<int>("screening_id"),
    bserv::make_db_field<int>("room_id"),
    bserv::make_db_field<int>("movie_id"),
    bserv::make_db_field<std::string>("time"),
    bserv::make_db_field<std::string>("price"),
    bserv::make_db_field<std::string>("showing_date"),
};

std::nullopt_t manage_movie_page(
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
        tx.exec("select * from screenings where movie_id = ?;", movie_id);
    lginfo << db_res.query();
    auto screenings = orm_screening.convert_to_vector(db_res);
    boost::json::array json_screenings;
    for (auto& screening : screenings) {
        json_screenings.push_back(screening);
    }
    context["screenings"] = json_screenings;

    boost::json::array placeholder;
    boost::json::object placeholder_obj;
    placeholder_obj["room_id"] = 1;
    placeholder_obj["room_name"] = "Room 1";
    placeholder.push_back(placeholder_obj);
    context["rooms"] = placeholder;

    context["permission"] = get_permission_for_session(*session_ptr);
    return index("managemovie.html", session_ptr, response, context);
}

std::nullopt_t form_add_screening(
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
    if ((p = check_session_permission(*session_ptr, "modify_movie")) !=
        std::nullopt) {
        throw std::runtime_error("Permission denied");
    }

    // All parameters must not be empty
    if (get_or_empty(params, "room_id").empty() ||
        get_or_empty(params, "time").empty() ||
        get_or_empty(params, "price").empty() ||
        get_or_empty(params, "date").empty()) {
        throw std::runtime_error("Invalid parameters");
    }

    bserv::db_transaction tx{conn};
    try {
        bserv::db_result r = tx.exec(
            "insert into screenings (room_id, movie_id, time, price, "
            "showing_date) "
            "values (?, ?, ?, ?, ?);",
            get_stoi_or_zero(params, "room_id"), std::stoi(movie_id),
            get_or_empty(params, "time"), get_or_empty(params, "price"),
            get_or_empty(params, "date"));
        lginfo << r.query();
        tx.commit();
    } catch (const std::exception& e) {
        throw std::runtime_error(e.what());
    }

    return manage_movie_page(request, response, std::move(params), conn,
                             session_ptr, movie_id);
}

std::nullopt_t form_modify_screening(
    bserv::request_type& request,
    bserv::response_type& response,
    boost::json::object&& params,
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr,
    const std::string& screening_id) {
    if (request.method() != boost::beast::http::verb::post) {
        throw bserv::url_not_found_exception{};
    }

    std::optional<boost::json::object> p;
    if ((p = check_session_permission(*session_ptr, "modify_movie")) !=
        std::nullopt) {
        throw std::runtime_error("Permission denied");
    }

    bserv::db_transaction tx{conn};
    bserv::db_result r = tx.exec(
        "update screenings set room_id = COALESCE(NULLIF(?, 0), room_id), "
        "time = COALESCE(NULLIF(?, ''), time), "
        "price = COALESCE(NULLIF(?, ''), price), "
        "showing_date = COALESCE(NULLIF(?, ''), showing_date) "
        "where screening_id = ?;",
        get_stoi_or_zero(params, "room_id"), get_or_empty(params, "time"),
        get_or_empty(params, "price"), get_or_empty(params, "date"),
        std::stoi(screening_id));
    lginfo << r.query();
    tx.commit();
    return manage_movie_page(request, response, std::move(params), conn,
                             session_ptr, get_or_empty(params, "movie_id"));
}
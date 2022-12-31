#include <vector>
#include "handlers.h"
#include "rendering.h"
extern bserv::db_relation_to_object orm_screening_room;

bserv::db_relation_to_object orm_screening{
    bserv::make_db_field<int>("screening_id"),
    bserv::make_db_field<int>("room_id"),
    bserv::make_db_field<int>("movie_id"),
    bserv::make_db_field<std::string>("time"),
    bserv::make_db_field<std::string>("price"),
    bserv::make_db_field<std::string>("showing_date"),
};

bserv::db_relation_to_object orm_last_insert_id{
    bserv::make_db_field<int>("screening_id"),
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

    // Get rooms
    db_res = tx.exec("select * from screening_rooms;");
    lginfo << db_res.query();
    auto screening_rooms = orm_screening_room.convert_to_vector(db_res);
    boost::json::array json_screening_rooms;
    for (auto& screening_room : screening_rooms) {
        json_screening_rooms.push_back(screening_room);
    }
    context["rooms"] = json_screening_rooms;

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
            "values (?, ?, ?, ?, ?) returning screening_id;",
            get_stoi_or_zero(params, "room_id"), std::stoi(movie_id),
            get_or_empty(params, "time"), get_or_empty(params, "price"),
            get_or_empty(params, "date"));
        lginfo << r.query();
        // Get the last inserted id
        auto last_insert_id = orm_last_insert_id.convert_to_vector(r);
        if (last_insert_id.size() != 1) {
            throw std::runtime_error("Invalid last insert id");
        }
        int screening_id = last_insert_id[0]["screening_id"].as_int64();

        // Also insert seats for this screening
        // The amount of seats inserted is equal to thr room capacity
        r = tx.exec(
            "select * from screening_rooms where room_id = ?;",
            get_stoi_or_zero(params, "room_id"));
        lginfo << r.query();
        auto room = orm_screening_room.convert_to_vector(r);
        if (room.size() != 1) {
            throw std::runtime_error("Invalid room id");
        }
        int room_capacity = room[0]["capacity"].as_int64();
        for (int i = 0; i < room_capacity; i++) {
            // User_id should be null
            // Seat name should be #i
            r = tx.exec(
                "insert into seats (screening_id, seat_name) "
                "values (?, ?);",
                screening_id, "#" + std::to_string(i+1));
            lginfo << r.query();
        }
        
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
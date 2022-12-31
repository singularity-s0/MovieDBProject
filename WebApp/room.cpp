#include <vector>
#include "handlers.h"
#include "rendering.h"

bserv::db_relation_to_object orm_screening_room{
    bserv::make_db_field<int>("room_id"),
    bserv::make_db_field<std::string>("room_name"),
    bserv::make_db_field<int>("theater_id"),
    bserv::make_db_field<std::string>("capacity"),
};


std::optional<boost::json::object> get_room(
    bserv::db_transaction& tx,
    const boost::json::string& room_name) {
    bserv::db_result r =
        tx.exec("select * from screening_rooms where room_name = ?", room_name);
    lginfo << r.query();
    return orm_screening_room.convert_to_optional(r);
}

std::optional<boost::json::object> get_room_by_id(bserv::db_transaction& tx,
                                                   const int room_id) {
    bserv::db_result r =
        tx.exec("select * from screening_rooms where room_id = ?", room_id);
    lginfo << r.query();
    return orm_screening_room.convert_to_optional(r);
}

boost::json::object room_register(
    bserv::request_type& request,
    boost::json::object&& params,
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr) {
    if (request.method() != boost::beast::http::verb::post) {
        throw bserv::url_not_found_exception{};
    }

    std::optional<boost::json::object> p;
    if ((p = check_session_permission(*session_ptr, "modify_room")) !=
        std::nullopt) {
        return p.value();
    }

    auto room_name = params["room_name"].as_string();
    bserv::db_transaction tx{conn};
    auto opt_room = get_room(tx, room_name);
    if (opt_room.has_value()) {
        return {{"success", false}, {"message", "Screening room name exists"}};
    }
    try {
        bserv::db_result r = tx.exec(
            "insert into ? "
            "(room_name, theater_id, "
            "capacity) values "
            "(?, ?, ?)",
            bserv::db_name("screening_rooms"), room_name, 
            get_stoi_or_zero(params, "theater_id"),
            get_stoi_or_zero(params, "capacity"));
        lginfo << r.query();
        tx.commit();
    } catch (const std::exception& e) {
        return {{"success", false}, {"message", e.what()}};
    }
    return {{"success", true}, {"message", "Screening room registered"}};
}

boost::json::object room_modify(bserv::request_type& request,
                                 // the json object is obtained from the request
                                 // body, as well as the url parameters
                                 boost::json::object&& params,
                                 std::shared_ptr<bserv::db_connection> conn,
                                 std::shared_ptr<bserv::session_type> session_ptr,
                                 int room_id) {
    if (request.method() != boost::beast::http::verb::post) {
        throw bserv::url_not_found_exception{};
    }

    std::optional<boost::json::object> p;
    if ((p = check_session_permission(*session_ptr, "modify_room")) !=
        std::nullopt) {
        return p.value();
    }

    bserv::db_transaction tx{conn};
    auto opt_room = get_room_by_id(tx, room_id);
    if (!opt_room.has_value()) {
        return {{"success", false}, {"message", "Screening room does not exist"}};
    }
    auto password = get_or_empty(params, "password");
    bserv::db_result r = tx.exec(
        "update ? "
        "set room_name = COALESCE(NULLIF(?, ''), room_name), "
        "theater_id = COALESCE(NULLIF(?, 0), theater_id), "
        "capacity = COALESCE(NULLIF(?, 0), capacity) "
        "where room_id = ?",
        bserv::db_name("screening_rooms"), get_or_empty(params, "room_name"),
        get_stoi_or_zero(params, "theater_id"),
        get_stoi_or_zero(params, "capacity"),
        room_id);
    lginfo << r.query();
    tx.commit();  // you must manually commit changes
    return {{"success", true}, {"message", "Screening room modified"}};
}

boost::json::object find_room(std::shared_ptr<bserv::db_connection> conn,
                               std::shared_ptr<bserv::session_type> session_ptr,
                               const std::string& room_name) {
    bserv::db_transaction tx{conn};
    auto room = get_room(tx, room_name.c_str());
    if (!room.has_value()) {
        return {{"success", false},
                {"message", "Requested screening room does not exist"}};
    }
    room.value().erase("room_id");
    return {{"success", true}, {"room", room.value()}};
}

std::nullopt_t redirect_to_rooms(
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr,
    bserv::response_type& response,
    int page_id,
    boost::json::object&& context) {
    lgdebug << "view rooms: " << page_id << std::endl;
    bserv::db_transaction tx{conn};
    bserv::db_result db_res = tx.exec("select count(*) from screening_rooms;");
    lginfo << db_res.query();
    std::size_t total_rooms = (*db_res.begin())[0].as<std::size_t>();
    lgdebug << "total rooms: " << total_rooms << std::endl;
    int total_pages = (int)total_rooms / 10;
    if (total_rooms % 10 != 0)
        ++total_pages;
    lgdebug << "total pages: " << total_pages << std::endl;
    db_res =
        tx.exec("select * from screening_rooms limit 10 offset ?;", (page_id - 1) * 10);
    lginfo << db_res.query();
    auto rooms = orm_screening_room.convert_to_vector(db_res);
    boost::json::array json_rooms;
    for (auto& room : rooms) {
        json_rooms.push_back(room);
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
    context["rooms"] = json_rooms;
    context["permission"] = get_permission_for_session(*session_ptr);
    return index("screening_rooms.html", session_ptr, response, context);
}

std::nullopt_t view_rooms(std::shared_ptr<bserv::db_connection> conn,
                           std::shared_ptr<bserv::session_type> session_ptr,
                           bserv::response_type& response,
                           const std::string& page_num) {
    int page_id = std::stoi(page_num);
    boost::json::object context;
    return redirect_to_rooms(conn, session_ptr, response, page_id,
                              std::move(context));
}

std::nullopt_t form_add_room(
    bserv::request_type& request,
    bserv::response_type& response,
    boost::json::object&& params,
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr) {
    boost::json::object context =
        room_register(request, std::move(params), conn, session_ptr);
    return redirect_to_rooms(conn, session_ptr, response, 1,
                              std::move(context));
}

std::nullopt_t form_modify_room(
    bserv::request_type& request,
    bserv::response_type& response,
    boost::json::object&& params,
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr,
    const std::string& room_id) {
    int rom_id = std::stoi(room_id);
    boost::json::object context =
        room_modify(request, std::move(params), conn, session_ptr, rom_id);
    return redirect_to_rooms(conn, session_ptr, response, 1,
                              std::move(context));
}
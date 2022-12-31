#include <vector>
#include "handlers.h"
#include "rendering.h"

#include <iostream>

extern bserv::db_relation_to_object orm_screening;

bserv::db_relation_to_object orm_ticket{
    bserv::make_db_field<int>("ticket_id"),
    bserv::make_db_field<int>("user_id"),
    bserv::make_db_field<int>("screening_id"),
    bserv::make_db_field<int>("seat_id"),
    bserv::make_db_field<int>("price"),
    bserv::make_db_field<bool>("refunded"),
};

bserv::db_relation_to_object orm_seats{
    bserv::make_db_field<int>("seat_id"),
    bserv::make_db_field<int>("screening_id"),
    bserv::make_db_field<std::string>("seat_name"),
    bserv::make_db_field<std::optional<int>>("user_id"),
};

std::nullopt_t buy_ticket_page(
    bserv::request_type& request,
    bserv::response_type& response,
    boost::json::object&& params,
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr,
    const std::string& movie_id) {
    if(!check_logged_in(*session_ptr)) {
        throw std::runtime_error("Not logged in");
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
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Invalid movie id");
    }

    auto selected_date = get_or_empty(params, "date");
    auto selected_time = get_or_empty(params, "time");
    auto selected_room = get_or_empty(params, "room");
    auto selected_seat = get_or_empty(params, "seat");

    bserv::db_transaction tx{conn};
    // Get available screening times
    bserv::db_result db_res = tx.exec("select * from screenings where screenings.movie_id=?;", movie_id);
    lginfo << db_res.query();
    auto screenings = orm_screening.convert_to_vector(db_res);
    boost::json::array screening_dates, screening_times;
    screening_dates.push_back("");
    screening_times.push_back("");
    for (auto& screening : screenings) {
        boost::json::string showing_date = screening["showing_date"].as_string();
        if(!array_contains(screening_dates, showing_date)) {
            screening_dates.push_back(showing_date);
        }
        if (showing_date == selected_date) {
            boost::json::string showing_time = screening["time"].as_string();
            if (!array_contains(screening_times, showing_time)) {
                screening_times.push_back(showing_time);
            }
            if (showing_time == selected_time) {
                int screening_id = screening["screening_id"].as_int64();
                int price = screening["price"].as_int64();
                context["screening_id"] = screening_id;
                context["price"] = price;
            }
        }
    }
    context["selected_time"] = selected_time;
    context["selected_date"] = selected_date;
    context["times"] = screening_times;
    context["dates"] = screening_dates;
    context["allow_submit"] = false;
    context["selected_seat"] = selected_seat;

    boost::json::array seats;
    seats.push_back("");
    // Get available seats
    if (context.count("screening_id")) {
        int screening_id = context["screening_id"].as_int64();
        bserv::db_result db_res = tx.exec("select * from seats where seats.screening_id=? and seats.user_id is NULL;", screening_id);
        lginfo << db_res.query();
        for (auto& seat : orm_seats.convert_to_vector(db_res)) {
            auto seat_name = seat["seat_name"].as_string();
            seats.push_back(seat_name);
        }
    }
    context["seats"] = seats;

    if (context.count("screening_id") && !selected_seat.empty()) {
        context["allow_submit"] = true;
    }

    context["permission"] = get_permission_for_session(*session_ptr);
    return index("buyticket.html", session_ptr, response, context);
}

boost::json::object buy_ticket(
    bserv::request_type& request,
    boost::json::object&& params,
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr) {
    if (request.method() != boost::beast::http::verb::post) {
        throw bserv::url_not_found_exception{};
    }
    if (!check_logged_in(*session_ptr)) {
        return {{"success", false}, {"message", "Not logged in"}};
    }

    boost::json::object session = *session_ptr;
    boost::json::object user = session["user"].as_object();
    int user_id = user["id"].as_int64();

    boost::json::string screening_id_str = params["screening_id"].as_string();
    auto screening_id = std::stoi(screening_id_str.c_str());
    auto seat_name = params["seat"].as_string();
    bserv::db_transaction tx{conn};
    bserv::db_result r =
        tx.exec("select * from seats where screening_id = ? and seat_name = ?;", screening_id, seat_name);
    lginfo << r.query();
    auto seats = orm_seats.convert_to_vector(r);
    if (seats.size() != 1) {
        return {{"success", false}, {"message", "Invalid seat"}};
    }
    auto seat_id = seats[0]["seat_id"].as_int64();

    // Get price
    r = tx.exec("select * from screenings where screening_id = ?;", screening_id);
    lginfo << r.query();
    auto screenings = orm_screening.convert_to_vector(r);
    if (screenings.size() != 1) {
        return {{"success", false}, {"message", "Invalid screening id"}};
    }
    auto price = screenings[0]["price"].as_int64();

    try {
        bserv::db_result r = tx.exec(
            "insert into tickets (user_id, screening_id, seat_id, price, refunded) values (?, ?, ?, ?, false);",
            user_id,
            screening_id,
            seat_id,
            price);
        // Also set the user_id of seat to the ticket holder
        r = tx.exec("update seats set user_id = ? where seat_id = ?;", user_id, seat_id);
        // Also update box office
        r = tx.exec("update movies set box_office = box_office + ?, num_participants = num_participants + 1 where movie_id = ?;", price, screenings[0]["movie_id"].as_int64());
        tx.commit();
    } catch (const std::exception& e) {
        return {{"success", false}, {"message", e.what()}};
    }
    return {{"success", true}, {"message", "Ticket bought"}};
}

boost::json::object refund_ticket(
    bserv::request_type& request,
    boost::json::object&& params,
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr,
    const std::string& ticket_id) {
    if (request.method() != boost::beast::http::verb::post) {
        throw bserv::url_not_found_exception{};
    }
    if (!check_logged_in(*session_ptr)) {
        return {{"success", false}, {"message", "You must log in to refund tickets"}};
    }
    boost::json::object session = *session_ptr;
    boost::json::object user = session["user"].as_object();
    int user_id = user["id"].as_int64();

    bserv::db_transaction tx{conn};
    // Verify that the ticket belongs to the user
    bserv::db_result r = tx.exec("select * from tickets where ticket_id = ?;", ticket_id);
    lginfo << r.query();
    auto tickets = orm_ticket.convert_to_vector(r);
    if (tickets.size() != 1) {
        return {{"success", false}, {"message", "Invalid ticket id"}};
    }
    auto ticket = tickets[0];
    if (ticket["user_id"].as_int64() != user_id) {
        return {{"success", false}, {"message", "You do not own this ticket"}};
    } else if (ticket["refunded"].as_bool()) {
        return {{"success", false}, {"message", "This ticket has already been refunded"}};
    }
    // Set ticket to refunded
    r = tx.exec("update tickets set refunded = true where ticket_id = ?;", ticket_id);
    // Also set the seat to free
    r = tx.exec("update seats set user_id = NULL where seat_id = ?;", ticket["seat_id"].as_int64());
    // Also update box office
    r = tx.exec("update movies set box_office = box_office - ?, num_participants = num_participants - 1 where movie_id = ?;", ticket["price"].as_int64(), ticket["movie_id"].as_int64());
    tx.commit();
    return {{"success", true}, {"message", "Ticket refunded"}};
}

std::nullopt_t form_buy_ticket(
    bserv::request_type& request,
    bserv::response_type& response,
    boost::json::object&& params,
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr) {
    boost::json::object context =
        buy_ticket(request, std::move(params), conn, session_ptr);
    return redirect_to_mycenter(conn, session_ptr, response,
                              std::move(context));
}

std::nullopt_t form_refund_ticket(
    bserv::request_type& request,
    bserv::response_type& response,
    boost::json::object&& params,
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr,
    const std::string& ticket_id) {
    boost::json::object context =
        refund_ticket(request, std::move(params), conn, session_ptr, ticket_id);
    return redirect_to_mycenter(conn, session_ptr, response,
                              std::move(context));
}
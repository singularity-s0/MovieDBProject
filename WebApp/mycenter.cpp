#include <vector>
#include "handlers.h"
#include "rendering.h"

extern bserv::db_relation_to_object orm_ticket;
extern bserv::db_relation_to_object orm_screening;

bserv::db_relation_to_object orm_full_ticket_info{
    bserv::make_db_field<int>("ticket_id"),
    bserv::make_db_field<std::string>("moviename"),
    bserv::make_db_field<std::string>("username"),
    bserv::make_db_field<std::string>("showing_date"),
    bserv::make_db_field<std::string>("time"),
    bserv::make_db_field<std::string>("room"),
    bserv::make_db_field<std::string>("seat_name"),
    bserv::make_db_field<int>("price"),
    bserv::make_db_field<bool>("refunded"),
};

std::nullopt_t redirect_to_mycenter(
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr,
    bserv::response_type& response,
    boost::json::object&& context) {
    // Get current user
    auto session = *session_ptr;
    if (!session.count("user")) {
        throw std::runtime_error("Not logged in");
    }
    boost::json::object user = session["user"].as_object();
    int uid = user["id"].as_int64();

    bserv::db_transaction tx{conn};
    // Get full ticket info
    bserv::db_result db_res = tx.exec("select tickets.ticket_id as ticket_id, movies.moviename, users.username as username, screenings.showing_date as showing_date, screenings.time as time, room.room_name as room, seats.seat_name as seat_name, tickets.price as price, tickets.refunded as refunded from tickets, auth_user as users, screenings, seats, screening_rooms as room, movies where tickets.user_id=? and tickets.user_id=users.id and tickets.screening_id=screenings.screening_id and tickets.seat_id=seats.seat_id and room.room_id=screenings.room_id and movies.movie_id=screenings.movie_id;", uid);
    lginfo << db_res.query();
    auto tickets = orm_full_ticket_info.convert_to_vector(db_res);
    boost::json::array json_tickets;
    boost::json::string space = " ";
    for (auto& ticket : tickets) {
        ticket["time"] = ticket["showing_date"].as_string().append(space).append(ticket["time"].as_string());
        json_tickets.push_back(ticket);
    }
    context["tickets"] = json_tickets;
    context["permission"] = get_permission_for_session(*session_ptr);
    return index("mycenter.html", session_ptr, response, context);
}

std::nullopt_t view_mycenter(std::shared_ptr<bserv::db_connection> conn,
                             std::shared_ptr<bserv::session_type> session_ptr,
                             bserv::response_type& response) {
    boost::json::object context;
    return redirect_to_mycenter(conn, session_ptr, response,
                                std::move(context));
}
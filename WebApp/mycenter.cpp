#include <vector>
#include "handlers.h"
#include "rendering.h"

extern bserv::db_relation_to_object orm_ticket;

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
    bserv::db_result db_res = tx.exec("select * from tickets where tickets.user_id=?;", uid);
    lginfo << db_res.query();
    auto tickets = orm_ticket.convert_to_vector(db_res);
    boost::json::array json_tickets;
    for (auto& ticket : tickets) {
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
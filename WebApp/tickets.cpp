#include <vector>
#include "handlers.h"
#include "rendering.h"

bserv::db_relation_to_object orm_ticket{
    bserv::make_db_field<int>("ticket_id"),
    bserv::make_db_field<int>("id"),
    bserv::make_db_field<int>("screening_id"),
    bserv::make_db_field<int>("time"),
    bserv::make_db_field<int>("price"),
    bserv::make_db_field<int>("seat_num"),
};

std::nullopt_t buy_ticket_page(
    bserv::request_type& request,
    bserv::response_type& response,
    boost::json::object&& params,
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr,
    const std::string& movie_id) {
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

    context["permission"] = get_permission_for_session(*session_ptr);
    return index("buyticket.html", session_ptr, response, context);
}
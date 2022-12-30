#include <vector>
#include "handlers.h"
#include "rendering.h"

bserv::db_relation_to_object orm_movie{
    bserv::make_db_field<int>("movie_id"),
    bserv::make_db_field<std::string>("moviename"),
    bserv::make_db_field<std::string>("starname"),
    bserv::make_db_field<std::string>("detail"),
    bserv::make_db_field<int>("running_time"),
    bserv::make_db_field<std::string>("type"),
    bserv::make_db_field<int>("avg_rating"),
    bserv::make_db_field<std::string>("poster"),
    bserv::make_db_field<int>("box_office"),
    bserv::make_db_field<int>("num_participants"),
    bserv::make_db_field<int>("release_date"),
    bserv::make_db_field<std::string>("box_office_unit"),
    bserv::make_db_field<std::string>("foreign_name"),
    bserv::make_db_field<std::string>("location"),
};

std::optional<boost::json::object> get_movie(
    bserv::db_transaction& tx,
    const boost::json::string& moviename) {
    bserv::db_result r =
        tx.exec("select * from movies where moviename = ?", moviename);
    lginfo << r.query();
    return orm_movie.convert_to_optional(r);
}

std::optional<boost::json::object> get_movie_by_id(bserv::db_transaction& tx,
                                                  const int movie_id) {
    bserv::db_result r = tx.exec("select * from movies where movie_id = ?", movie_id);
    lginfo << r.query(); 
    return orm_movie.convert_to_optional(r);
}

boost::json::object movie_register(bserv::request_type& request,
                                   boost::json::object&& params,
                                   std::shared_ptr<bserv::db_connection> conn) {
    if (request.method() != boost::beast::http::verb::post) {
        throw bserv::url_not_found_exception{};
    }
    if (params.count("moviename") == 0) {
        return {{"success", false}, {"message", "Movie name is required"}};
    }

    auto moviename = params["moviename"].as_string();
    bserv::db_transaction tx{conn};
    auto opt_movie = get_movie(tx, moviename);
    if (opt_movie.has_value()) {
        return {{"success", false}, {"message", "Movie name exists"}};
    }
    try {
        bserv::db_result r = tx.exec(
            "insert into ? "
            "(moviename, starname, detail, running_time, type, avg_rating, poster,"
            "box_office, num_participants, release_date, box_office_unit,"
            "foreign_name, location) values "
            "(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
            bserv::db_name("movies"), moviename,
            get_or_empty(params, "starname"), get_or_empty(params, "detail"),
            get_stoi_or_zero(params, "running_time"),
            get_or_empty(params, "type"),
            get_stoi_or_zero(params, "avg_rating"),
            get_or_empty(params, "poster"),
            get_stoi_or_zero(params, "box_office"),
            get_stoi_or_zero(params, "num_participants"),
            get_stoi_or_zero(params, "release_date"),
            get_or_empty(params, "box_office_unit"),
            get_or_empty(params, "foreign_name"),
            get_or_empty(params, "location"));
        lginfo << r.query();
        tx.commit();
    } catch (const std::exception& e) {
        return {{"success", false}, {"message", e.what()}};
    }
    return {{"success", true}, {"message", "Movie registered"}};
}

boost::json::object movie_modify(bserv::request_type& request,
                                // the json object is obtained from the request
                                // body, as well as the url parameters
                                boost::json::object&& params,
                                std::shared_ptr<bserv::db_connection> conn,
                                int movie_id) {
    if (request.method() != boost::beast::http::verb::post) {
        throw bserv::url_not_found_exception{};
    }
    bserv::db_transaction tx{conn};
    auto opt_movie = get_movie_by_id(tx, movie_id);
    if (!opt_movie.has_value()) {
        return {{"success", false}, {"message", "Movie does not exist"}};
    }
    auto password = get_or_empty(params, "password");
    bserv::db_result r = tx.exec(
        "update ? "
        "set moviename = COALESCE(NULLIF(?, ''), moviename), "
        "starname = COALESCE(NULLIF(?, ''), starname), "
        "detail = COALESCE(NULLIF(?, ''), detail), "
        "running_time = COALESCE(NULLIF(?, ''), running_time), "
        "type = COALESCE(NULLIF(?, ''), type), "
        "avg_rating = COALESCE(NULLIF(?, ''), avg_rating) "
        "poster = COALESCE(NULLIF(?, ''), poster) "
        "box_office = COALESCE(NULLIF(?, ''), box_office) "
        "num_participants = COALESCE(NULLIF(?, ''), num_participants) "
        "release_date = COALESCE(NULLIF(?, ''), release_date) "
        "box_office_unit = COALESCE(NULLIF(?, ''), box_office_unit) "
        "foreign_name = COALESCE(NULLIF(?, ''), foreign_name) "
        "location = COALESCE(NULLIF(?, ''), location) "
        "where movie_id = ?",
        bserv::db_name("movies"), get_or_empty(params, "moviename"),
        get_or_empty(params, "starname"), get_or_empty(params, "detail"),
        get_or_empty(params, "running_time"), get_or_empty(params, "type"),
        get_or_empty(params, "avg_rating"),get_or_empty(params, "poster"), 
        get_or_empty(params, "box_office"), get_or_empty(params, "num_participants"),
        get_or_empty(params, "release_date"), get_or_empty(params, "box_office_unit"), 
        get_or_empty(params, "foreign_name"),get_or_empty(params, "location"), movie_id);
    lginfo << r.query();
    tx.commit();  // you must manually commit changes
    return {{"success", true}, {"message", "Movie modified"}};
}

boost::json::object find_movie(std::shared_ptr<bserv::db_connection> conn,
                               const std::string& moviename) {
    bserv::db_transaction tx{conn};
    auto movie = get_movie(tx, moviename.c_str());
    if (!movie.has_value()) {
        return {{"success", false},
                {"message", "Requested movie does not exist"}};
    }
    movie.value().erase("movie_id");
    return {{"success", true}, {"movie", movie.value()}};
}

std::nullopt_t redirect_to_movies(
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr,
    bserv::response_type& response,
    int page_id,
    boost::json::object&& context) {
    lgdebug << "view movies: " << page_id << std::endl;
    bserv::db_transaction tx{conn};
    bserv::db_result db_res = tx.exec("select count(*) from movies;");
    lginfo << db_res.query();
    std::size_t total_movies = (*db_res.begin())[0].as<std::size_t>();
    lgdebug << "total movies: " << total_movies << std::endl;
    int total_pages = (int)total_movies / 10;
    if (total_movies % 10 != 0)
        ++total_pages;
    lgdebug << "total pages: " << total_pages << std::endl;
    db_res =
        tx.exec("select * from movies limit 10 offset ?;", (page_id - 1) * 10);
    lginfo << db_res.query();
    auto movies = orm_movie.convert_to_vector(db_res);
    boost::json::array json_movies;
    for (auto& movie : movies) {
        json_movies.push_back(movie);
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
    context["movies"] = json_movies;
    return index("index.html", session_ptr, response, context);
}

std::nullopt_t view_movies(std::shared_ptr<bserv::db_connection> conn,
                           std::shared_ptr<bserv::session_type> session_ptr,
                           bserv::response_type& response,
                           const std::string& page_num) {
    int page_id = std::stoi(page_num);
    boost::json::object context;
    return redirect_to_movies(conn, session_ptr, response, page_id,
                              std::move(context));
}

std::nullopt_t form_add_movie(
    bserv::request_type& request,
    bserv::response_type& response,
    boost::json::object&& params,
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr) {
    boost::json::object context =
        movie_register(request, std::move(params), conn);
    return redirect_to_movies(conn, session_ptr, response, 1,
                              std::move(context));
}

std::nullopt_t form_modify_movie(
    bserv::request_type& request,
    bserv::response_type& response,
    boost::json::object&& params,
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr,
    const std::string& movie_id) {
    int mov_id = std::stoi(movie_id);
    boost::json::object context =
        movie_modify(request, std::move(params), conn, mov_id);
    return redirect_to_movies(conn, session_ptr, response, 1,
                             std::move(context));
}
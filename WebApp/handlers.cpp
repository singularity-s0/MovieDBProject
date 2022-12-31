#include "handlers.h"
#include <vector>
#include "rendering.h"

// register an orm mapping (to convert the db query results into
// json objects).
// the db query results contain several rows, each has a number of
// fields. the order of `make_db_field<Type[i]>(name[i])` in the
// initializer list corresponds to these fields (`Type[0]` and
// `name[0]` correspond to field[0], `Type[1]` and `name[1]`
// correspond to field[1], ...). `Type[i]` is the type you want
// to convert the field value to, and `name[i]` is the identifier
// with which you want to store the field in the json object, so
// if the returned json object is `obj`, `obj[name[i]]` will have
// the type of `Type[i]` and store the value of field[i].

bserv::db_relation_to_object orm_theater{
    bserv::make_db_field<int>("theater_id"),
    bserv::make_db_field<std::string>("theater_name"),
    bserv::make_db_field<std::string>("theater_address"),
};

bool array_contains(const boost::json::array& arr, const boost::json::string& str) {
    for (const auto& item : arr) {
        if (item.as_string() == str) {
            return true;
        }
    }
    return false;
}

std::string get_or_empty(boost::json::object& obj, const std::string& key) {
    return obj.count(key) ? obj[key].as_string().c_str() : "";
}

int get_stoi_or_zero(boost::json::object& obj, const std::string& key) {
    auto str = get_or_empty(obj, key);
	return str.empty() ? 0 : std::stoi(str);
}

double get_stod_or_zero(boost::json::object& obj, const std::string& key) {
    auto str = get_or_empty(obj, key);
	return str.empty() ? 0 : std::stod(str);
}

boost::json::object send_request(std::shared_ptr<bserv::session_type> session,
                                 std::shared_ptr<bserv::http_client> client_ptr,
                                 boost::json::object&& params) {
    // post for response:
    // auto res = client_ptr->post(
    //     "localhost", "8080", "/echo", {{"msg", "request"}}
    // );
    // return {{"response", boost::json::parse(res.body())}};
    // -------------------------------------------------------
    // - if it takes longer than 30 seconds (by default) to
    // - get the response, this will raise a read timeout
    // -------------------------------------------------------
    // post for json response (json value, rather than json
    // object, is returned):
    auto obj = client_ptr->post_for_value("localhost", "8080", "/echo",
                                          {{"request", params}});
    if (session->count("cnt") == 0) {
        (*session)["cnt"] = 0;
    }
    (*session)["cnt"] = (*session)["cnt"].as_int64() + 1;
    return {{"response", obj}, {"cnt", (*session)["cnt"]}};
}

boost::json::object echo(boost::json::object&& params) {
    return {{"echo", params}};
}

// websocket
std::nullopt_t ws_echo(std::shared_ptr<bserv::session_type> session,
                       std::shared_ptr<bserv::websocket_server> ws_server) {
    ws_server->write_json((*session)["cnt"]);
    while (true) {
        try {
            std::string data = ws_server->read();
            ws_server->write(data);
        } catch (bserv::websocket_closed&) {
            break;
        }
    }
    return std::nullopt;
}

std::nullopt_t serve_static_files(bserv::response_type& response,
                                  const std::string& path) {
    return serve(response, path);
}

std::nullopt_t index(const std::string& template_path,
                     std::shared_ptr<bserv::session_type> session_ptr,
                     bserv::response_type& response,
                     boost::json::object& context) {
    bserv::session_type& session = *session_ptr;
    if (session.contains("movie")) {
        context["movie"] = session["movie"];
    }
    if (session.contains("user")) {
        context["user"] = session["user"];
    }
    return render(response, template_path, context);
}

std::nullopt_t index_page(std::shared_ptr<bserv::session_type> session_ptr,
                          bserv::response_type& response) {
    boost::json::object context;
    return index("index.html", session_ptr, response, context);
}
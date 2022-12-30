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
bserv::db_relation_to_object orm_user{
	bserv::make_db_field<int>("id"),
	bserv::make_db_field<std::string>("username"),
	bserv::make_db_field<std::string>("password"),
	bserv::make_db_field<bool>("is_superuser"),
	bserv::make_db_field<std::string>("first_name"),
	bserv::make_db_field<std::string>("last_name"),
	bserv::make_db_field<std::string>("email"),
	bserv::make_db_field<bool>("is_active"),
	bserv::make_db_field<std::string>("roal"),
};

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

bserv::db_relation_to_object orm_theater{
	bserv::make_db_field<int>("theater_id"),
	bserv::make_db_field<std::string>("theater_name"),
	bserv::make_db_field<std::string>("theater_address"),
};

bserv::db_relation_to_object orm_comment{
	bserv::make_db_field<int>("comment_id"),
	bserv::make_db_field<int>("id"),
	bserv::make_db_field<int>("movie_id"),
	bserv::make_db_field<std::string>("content"),
};

bserv::db_relation_to_object orm_screening_room{
	bserv::make_db_field<int>("room_id"),
	bserv::make_db_field<std::string>("room_name"),
	bserv::make_db_field<int>("theater_id"),
	bserv::make_db_field<std::string>("capacity"),
};

bserv::db_relation_to_object orm_screening{
	bserv::make_db_field<int>("screening_id"),
	bserv::make_db_field<int>("room_id"),
	bserv::make_db_field<int>("movie_id"),
	bserv::make_db_field<int>("time"),
	bserv::make_db_field<int>("price"),
	bserv::make_db_field<int>("showing_date"),
};

bserv::db_relation_to_object orm_ticket{
	bserv::make_db_field<int>("ticket_id"),
	bserv::make_db_field<int>("id"),
	bserv::make_db_field<int>("screening_id"),
	bserv::make_db_field<int>("time"),
	bserv::make_db_field<int>("price"),
	bserv::make_db_field<int>("seat_num"),
};

std::optional<boost::json::object> get_user(
	bserv::db_transaction& tx,
	const boost::json::string& username) {
	bserv::db_result r = tx.exec(
		"select * from auth_user where username = ?", username);
	lginfo << r.query(); // this is how you log info
	return orm_user.convert_to_optional(r);
}

std::optional<boost::json::object> get_user_by_id(
	bserv::db_transaction& tx,
	const int id) {
	bserv::db_result r = tx.exec(
		"select * from auth_user where id = ?", id);
	lginfo << r.query(); // this is how you log info
	return orm_user.convert_to_optional(r);
}

std::string get_or_empty(
	boost::json::object& obj,
	const std::string& key) {
	return obj.count(key) ? obj[key].as_string().c_str() : "";
}

// if you return a json object, the serialization
// is performed automatically.
boost::json::object user_register(
	bserv::request_type& request,
	// the json object is obtained from the request body,
	// as well as the url parameters
	boost::json::object&& params,
	std::shared_ptr<bserv::db_connection> conn) {
	if (request.method() != boost::beast::http::verb::post) {
		throw bserv::url_not_found_exception{};
	}
	if (params.count("username") == 0) {
		return {
			{"success", false},
			{"message", "`username` is required"}
		};
	}
	if (params.count("password") == 0) {
		return {
			{"success", false},
			{"message", "`password` is required"}
		};
	}
	auto username = params["username"].as_string();
	bserv::db_transaction tx{ conn };
	auto opt_user = get_user(tx, username);
	if (opt_user.has_value()) {
		return {
			{"success", false},
			{"message", "`Username` exists"}
		};
	}
	auto password = params["password"].as_string();
	bserv::db_result r = tx.exec(
		"insert into ? "
		"(?, password, is_superuser, "
		"first_name, last_name, email, is_active, roal) values "
		"(?, ?, ?, ?, ?, ?, ?, ?)", bserv::db_name("auth_user"),
		bserv::db_name("username"),
		username,
		bserv::utils::security::encode_password(
			password.c_str()), false,
		get_or_empty(params, "first_name"),
		get_or_empty(params, "last_name"),
		get_or_empty(params, "email"), true,
		get_or_empty(params, "roal"));
	lginfo << r.query();
	tx.commit(); // you must manually commit changes
	return {
		{"success", true},
		{"message", "User registered"}
	};
}

boost::json::object user_modify(
	bserv::request_type& request,
	// the json object is obtained from the request body,
	// as well as the url parameters
	boost::json::object&& params,
	std::shared_ptr<bserv::db_connection> conn,
	int user_id) {
	if (request.method() != boost::beast::http::verb::post) {
		throw bserv::url_not_found_exception{};
	}
	bserv::db_transaction tx{ conn };
	auto opt_user = get_user_by_id(tx, user_id);
	if (!opt_user.has_value()) {
		return {
			{"success", false},
			{"message", "User does not exist"}
		};
	}
	auto password = get_or_empty(params, "password");
	bserv::db_result r = tx.exec(
		"update ? "
		"set username = COALESCE(NULLIF(?, ''), username), "
		"password = COALESCE(NULLIF(?, ''), password), "
		"first_name = COALESCE(NULLIF(?, ''), first_name), "
		"last_name = COALESCE(NULLIF(?, ''), last_name), "
		"email = COALESCE(NULLIF(?, ''), email), "
		"roal = COALESCE(NULLIF(?, ''), roal) "
		"where id = ?", bserv::db_name("auth_user"),
		get_or_empty(params, "username"),
		password.empty() ? password : bserv::utils::security::encode_password(
			password.c_str()),
		get_or_empty(params, "first_name"),
		get_or_empty(params, "last_name"),
		get_or_empty(params, "email"),
		get_or_empty(params, "roal"),
		user_id);
	lginfo << r.query();
	tx.commit(); // you must manually commit changes
	return {
		{"success", true},
		{"message", "User modified"}
	};
}

boost::json::object user_login(
	bserv::request_type& request,
	boost::json::object&& params,
	std::shared_ptr<bserv::db_connection> conn,
	std::shared_ptr<bserv::session_type> session_ptr) {
	if (request.method() != boost::beast::http::verb::post) {
		throw bserv::url_not_found_exception{};
	}
	if (params.count("username") == 0) {
		return {
			{"success", false},
			{"message", "`username` is required"}
		};
	}
	if (params.count("password") == 0) {
		return {
			{"success", false},
			{"message", "`password` is required"}
		};
	}
	auto username = params["username"].as_string();
	bserv::db_transaction tx{ conn };
	auto opt_user = get_user(tx, username);
	if (!opt_user.has_value()) {
		return {
			{"success", false},
			{"message", "invalid username/password"}
		};
	}
	auto& user = opt_user.value();
	if (!user["is_active"].as_bool()) {
		return {
			{"success", false},
			{"message", "invalid username/password"}
		};
	}
	auto password = params["password"].as_string();
	auto encoded_password = user["password"].as_string();
	if (!bserv::utils::security::check_password(
		password.c_str(), encoded_password.c_str())) {
		return {
			{"success", false},
			{"message", "invalid username/password"}
		};
	}
	bserv::session_type& session = *session_ptr;
	session["user"] = user;
	return {
		{"success", true},
		{"message", "login successfully"}
	};
}

boost::json::object find_user(
	std::shared_ptr<bserv::db_connection> conn,
	const std::string& username) {
	bserv::db_transaction tx{ conn };
	auto user = get_user(tx, username.c_str());
	if (!user.has_value()) {
		return {
			{"success", false},
			{"message", "requested user does not exist"}
		};
	}
	user.value().erase("id");
	user.value().erase("password");
	return {
		{"success", true},
		{"user", user.value()}
	};
}

boost::json::object user_logout(
	std::shared_ptr<bserv::session_type> session_ptr) {
	bserv::session_type& session = *session_ptr;
	if (session.count("user")) {
		session.erase("user");
	}
	return {
		{"success", true},
		{"message", "logout successfully"}
	};
}

boost::json::object send_request(
	std::shared_ptr<bserv::session_type> session,
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
	auto obj = client_ptr->post_for_value(
		"localhost", "8080", "/echo", { {"request", params} }
	);
	if (session->count("cnt") == 0) {
		(*session)["cnt"] = 0;
	}
	(*session)["cnt"] = (*session)["cnt"].as_int64() + 1;
	return { {"response", obj}, {"cnt", (*session)["cnt"]} };
}

boost::json::object echo(
	boost::json::object&& params) {
	return { {"echo", params} };
}

// websocket
std::nullopt_t ws_echo(
	std::shared_ptr<bserv::session_type> session,
	std::shared_ptr<bserv::websocket_server> ws_server) {
	ws_server->write_json((*session)["cnt"]);
	while (true) {
		try {
			std::string data = ws_server->read();
			ws_server->write(data);
		}
		catch (bserv::websocket_closed&) {
			break;
		}
	}
	return std::nullopt;
}


std::nullopt_t serve_static_files(
	bserv::response_type& response,
	const std::string& path) {
	return serve(response, path);
}

std::nullopt_t index(
	const std::string& template_path,
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

std::nullopt_t index_page(
	std::shared_ptr<bserv::session_type> session_ptr,
	bserv::response_type& response) {
	boost::json::object context;
	return index("index.html", session_ptr, response, context);
}

std::nullopt_t form_login(
	bserv::request_type& request,
	bserv::response_type& response,
	boost::json::object&& params,
	std::shared_ptr<bserv::db_connection> conn,
	std::shared_ptr<bserv::session_type> session_ptr) {
	lgdebug << params << std::endl;
	auto context = user_login(request, std::move(params), conn, session_ptr);
	lginfo << "login: " << context << std::endl;
	return redirect_to_movies(conn, session_ptr, response, 1, std::move(context));
}

std::nullopt_t form_logout(
	std::shared_ptr<bserv::db_connection> conn,
	std::shared_ptr<bserv::session_type> session_ptr,
	bserv::response_type& response) {
	auto context = user_logout(session_ptr);
	lginfo << "logout: " << context << std::endl;
	return redirect_to_movies(conn, session_ptr, response, 1, std::move(context));
}

std::nullopt_t redirect_to_users(
	std::shared_ptr<bserv::db_connection> conn,
	std::shared_ptr<bserv::session_type> session_ptr,
	bserv::response_type& response,
	int page_id,
	boost::json::object&& context) {
	lgdebug << "view users: " << page_id << std::endl;
	bserv::db_transaction tx{ conn };
	bserv::db_result db_res = tx.exec("select count(*) from auth_user;");
	lginfo << db_res.query();
	std::size_t total_users = (*db_res.begin())[0].as<std::size_t>();
	lgdebug << "total users: " << total_users << std::endl;
	int total_pages = (int)total_users / 10;
	if (total_users % 10 != 0) ++total_pages;
	lgdebug << "total pages: " << total_pages << std::endl;
	db_res = tx.exec("select * from auth_user limit 10 offset ?;", (page_id - 1) * 10);
	lginfo << db_res.query();
	auto users = orm_user.convert_to_vector(db_res);
	boost::json::array json_users;
	for (auto& user : users) {
		json_users.push_back(user);
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
		}
		else {
			lower = 1;
		}
		if (page_id + 3 < total_pages - 1) {
			pagination["right_ellipsis"] = true;
		}
		else {
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
	context["users"] = json_users;
	return index("users.html", session_ptr, response, context);
}

std::nullopt_t view_users(
	std::shared_ptr<bserv::db_connection> conn,
	std::shared_ptr<bserv::session_type> session_ptr,
	bserv::response_type& response,
	const std::string& page_num) {
	int page_id = std::stoi(page_num);
	boost::json::object context;
	return redirect_to_users(conn, session_ptr, response, page_id, std::move(context));
}

std::nullopt_t form_add_user(
	bserv::request_type& request,
	bserv::response_type& response,
	boost::json::object&& params,
	std::shared_ptr<bserv::db_connection> conn,
	std::shared_ptr<bserv::session_type> session_ptr) {
	boost::json::object context = user_register(request, std::move(params), conn);
	return redirect_to_users(conn, session_ptr, response, 1, std::move(context));
}

std::nullopt_t form_modify_user(
	bserv::request_type& request,
	bserv::response_type& response,
	boost::json::object&& params,
	std::shared_ptr<bserv::db_connection> conn,
	std::shared_ptr<bserv::session_type> session_ptr,
	const std::string& user_id) {
	int usr_id = std::stoi(user_id);
	boost::json::object context = user_modify(request, std::move(params), conn, usr_id);
	return redirect_to_users(conn, session_ptr, response, 1, std::move(context));
}

std::optional<boost::json::object> get_movie(
	bserv::db_transaction& tx,
	const boost::json::string& moviename) {
	bserv::db_result r = tx.exec(
		"select * from movies where moviename = ?", moviename);
	lginfo << r.query(); 
	return orm_movie.convert_to_optional(r);
}

boost::json::object movie_register(
	bserv::request_type& request,
	boost::json::object&& params,
	std::shared_ptr<bserv::db_connection> conn) {
	if (request.method() != boost::beast::http::verb::post) {
		throw bserv::url_not_found_exception{};
	}
	if (params.count("moviename") == 0) {
		return {
			{"success", false},
			{"message", "`moviename` is required"}
		};
	}
	
	auto moviename = params["moviename"].as_string();
	bserv::db_transaction tx{ conn };
	auto opt_movie = get_movie(tx, moviename);
	if (opt_movie.has_value()) {
		return {
			{"success", false},
			{"message", "`moviename` existed"}
		};
	}

	bserv::db_result r = tx.exec(
		"insert into ? "
		"(?, movie_id,  "
		"starname, detail, duration, type, avg_rating, poster,"
		"box_office, num_participants, release_date, box_office_unit"
		"foreign_name, location) values "
		"(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", bserv::db_name("movies"),
		bserv::db_name("moviename"),
		moviename, 
		get_or_empty(params, "movie_id"),
		get_or_empty(params, "starname"),
		get_or_empty(params, "detail"),
		get_or_empty(params, "duration"),
		get_or_empty(params, "type"),
		get_or_empty(params, "avg_rating"),
		get_or_empty(params, "poster"),
		get_or_empty(params, "box_office"),
		get_or_empty(params, "num_participants"),
		get_or_empty(params, "release_date"),
		get_or_empty(params, "box_office_unit"),
		get_or_empty(params, "foreign_name"), 
		get_or_empty(params, "location"));
	lginfo << r.query();
	tx.commit(); // you must manually commit changes
	return {
		{"success", true},
		{"message", "movie registered"}
	};
}

boost::json::object find_movie(
	std::shared_ptr<bserv::db_connection> conn,
	const std::string& moviename) {
	bserv::db_transaction tx{ conn };
	auto movie = get_movie(tx, moviename.c_str());
	if (!movie.has_value()) {
		return {
			{"success", false},
			{"message", "requested movie does not exist"}
		};
	}
	movie.value().erase("movie_id");
	return {
		{"success", true},
		{"movie", movie.value()}
	};
}

std::nullopt_t redirect_to_movies(
	std::shared_ptr<bserv::db_connection> conn,
	std::shared_ptr<bserv::session_type> session_ptr,
	bserv::response_type& response,
	int page_id,
	boost::json::object&& context) {
	lgdebug << "view movies: " << page_id << std::endl;
	bserv::db_transaction tx{ conn };
	bserv::db_result db_res = tx.exec("select count(*) from movies;");
	lginfo << db_res.query();
	std::size_t total_movies = (*db_res.begin())[0].as<std::size_t>();
	lgdebug << "total movies: " << total_movies << std::endl;
	int total_pages = (int)total_movies / 10;
	if (total_movies % 10 != 0) ++total_pages;
	lgdebug << "total pages: " << total_pages << std::endl;
	db_res = tx.exec("select * from movies limit 10 offset ?;", (page_id - 1) * 10);
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
		}
		else {
			lower = 1;
		}
		if (page_id + 3 < total_pages - 1) {
			pagination["right_ellipsis"] = true;
		}
		else {
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

std::nullopt_t view_movies(
	std::shared_ptr<bserv::db_connection> conn,
	std::shared_ptr<bserv::session_type> session_ptr,
	bserv::response_type& response,
	const std::string& page_num) {
	int page_id = std::stoi(page_num);
	boost::json::object context;
	return redirect_to_movies(conn, session_ptr, response, page_id, std::move(context));
}

std::nullopt_t form_add_movie(
	bserv::request_type& request,
	bserv::response_type& response,
	boost::json::object&& params,
	std::shared_ptr<bserv::db_connection> conn,
	std::shared_ptr<bserv::session_type> session_ptr) {
	boost::json::object context = movie_register(request, std::move(params), conn);
	return redirect_to_movies(conn, session_ptr, response, 1, std::move(context));
}

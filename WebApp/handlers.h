#pragma once

#include <boost/json.hpp>

#include <memory>
#include <optional>
#include <string>

#include "bserv/common.hpp"

boost::json::object get_permission_for_session(boost::json::object session);
bool check_permission(const boost::json::string& user_type, const boost::json::string& permission);
std::optional<boost::json::object> check_session_permission(boost::json::object session, boost::json::string permission);
                      
std::string get_or_empty(boost::json::object& obj, const std::string& key);

int get_stoi_or_zero(boost::json::object& obj, const std::string& key);

std::nullopt_t index(const std::string& template_path,
                     std::shared_ptr<bserv::session_type> session_ptr,
                     bserv::response_type& response,
                     boost::json::object& context);

std::nullopt_t index_page(std::shared_ptr<bserv::session_type> session_ptr,
                          bserv::response_type& response);

boost::json::object user_register(bserv::request_type& request,
                                  boost::json::object&& params,
                                  std::shared_ptr<bserv::db_connection> conn);

boost::json::object user_login(
    bserv::request_type& request,
    boost::json::object&& params,
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr);

boost::json::object find_user(std::shared_ptr<bserv::db_connection> conn,
                              const std::string& username);

boost::json::object user_logout(
    std::shared_ptr<bserv::session_type> session_ptr);

boost::json::object send_request(std::shared_ptr<bserv::session_type> session,
                                 std::shared_ptr<bserv::http_client> client_ptr,
                                 boost::json::object&& params);

boost::json::object echo(boost::json::object&& params);

std::nullopt_t form_register(bserv::request_type& request,
                             bserv::response_type& response,
                             boost::json::object&& params,
                             std::shared_ptr<bserv::db_connection> conn,
                             std::shared_ptr<bserv::session_type> session_ptr);

boost::json::object movie_register(
    bserv::request_type& request,
    boost::json::object&& params,
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr);

boost::json::object find_movie(std::shared_ptr<bserv::db_connection> conn,
                               std::shared_ptr<bserv::session_type> session_ptr,
                               const std::string& moviename);

std::optional<boost::json::object> get_movie(
    bserv::db_transaction& tx,
    const boost::json::string& moviename);

std::optional<boost::json::object> get_movie_by_id(bserv::db_transaction& tx,
                                                   const int movie_id);

// websocket
std::nullopt_t ws_echo(std::shared_ptr<bserv::session_type> session,
                       std::shared_ptr<bserv::websocket_server> ws_server);

std::nullopt_t serve_static_files(bserv::response_type& response,
                                  const std::string& path);

std::nullopt_t index_page(std::shared_ptr<bserv::session_type> session_ptr,
                          bserv::response_type& response);

std::nullopt_t form_login(bserv::request_type& request,
                          bserv::response_type& response,
                          boost::json::object&& params,
                          std::shared_ptr<bserv::db_connection> conn,
                          std::shared_ptr<bserv::session_type> session_ptr);

std::nullopt_t form_logout(std::shared_ptr<bserv::db_connection> conn,
                           std::shared_ptr<bserv::session_type> session_ptr,
                           bserv::response_type& response);

std::nullopt_t view_users(std::shared_ptr<bserv::db_connection> conn,
                          std::shared_ptr<bserv::session_type> session_ptr,
                          bserv::response_type& response,
                          const std::string& page_num);

std::nullopt_t form_add_user(bserv::request_type& request,
                             bserv::response_type& response,
                             boost::json::object&& params,
                             std::shared_ptr<bserv::db_connection> conn,
                             std::shared_ptr<bserv::session_type> session_ptr);

std::nullopt_t form_modify_user(
    bserv::request_type& request,
    bserv::response_type& response,
    boost::json::object&& params,
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr,
    const std::string& user_id);

std::nullopt_t view_movies(std::shared_ptr<bserv::db_connection> conn,
                           std::shared_ptr<bserv::session_type> session_ptr,
                           bserv::response_type& response,
                           const std::string& page_num);

std::nullopt_t form_add_movie(bserv::request_type& request,
                              bserv::response_type& response,
                              boost::json::object&& params,
                              std::shared_ptr<bserv::db_connection> conn,
                              std::shared_ptr<bserv::session_type> session_ptr);

std::nullopt_t form_modify_movie(
    bserv::request_type& request,
    bserv::response_type& response,
    boost::json::object&& params,
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr,
    const std::string& movie_id);

std::nullopt_t redirect_to_movies(
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr,
    bserv::response_type& response,
    int page_id,
    boost::json::object&& context);

std::nullopt_t buy_ticket_page(
    bserv::request_type& request,
    bserv::response_type& response,
    boost::json::object&& params,
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr,
    const std::string& movie_id);

std::nullopt_t manage_movie_page(
    bserv::request_type& request,
    bserv::response_type& response,
    boost::json::object&& params,
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr,
    const std::string& movie_id);

std::nullopt_t form_add_screening(
    bserv::request_type& request,
    bserv::response_type& response,
    boost::json::object&& params,
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr,
    const std::string& movie_id);

std::nullopt_t form_modify_screening(
    bserv::request_type& request,
    bserv::response_type& response,
    boost::json::object&& params,
    std::shared_ptr<bserv::db_connection> conn,
    std::shared_ptr<bserv::session_type> session_ptr,
    const std::string& screening_id);
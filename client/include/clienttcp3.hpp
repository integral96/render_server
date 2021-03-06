#pragma once

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/core/noncopyable.hpp>
#include <boost/bind.hpp>

#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

using namespace boost;

template<typename T>
using BytesArray = boost::array<T, 1024>;

using Callback = boost::function<void (size_t request_id, const std::string& response, const system::error_code & ec)>;

struct Session {
  Session(asio::io_service& ios, const std::string& raw_ip_address, unsigned short port_num,
          const std::string& request, size_t id, Callback callback)
      : m_sock{ios}, m_ep{asio::ip::address::from_string(raw_ip_address), port_num},
        m_request{request}, m_id{id}, m_callback{callback}, m_was_cancelled{false} {}

  asio::ip::tcp::socket m_sock;
  asio::ip::tcp::endpoint m_ep;


  std::string m_request;

  asio::streambuf m_response_buf;
  std::string m_response;


  system::error_code m_ec;
  size_t m_id;

  Callback m_callback;

  bool m_was_cancelled;
  std::mutex m_cancel_guard;


};

class AsyncTCPClient : public boost::noncopyable
{

private:
  asio::io_service m_ios;
  std::map<int, std::shared_ptr<Session>> m_active_sessions;
  std::mutex m_active_sessions_guard;
  std::unique_ptr<asio::io_service::work> m_work;
  std::unique_ptr<std::thread> m_thread;

  bool mDoReconnect;
  bool mIsReconnecting;

   boost::asio::deadline_timer m_reConnectTimer;

public:
  AsyncTCPClient() :m_reConnectTimer(m_ios) {
    m_work.reset(new asio::io_service::work{m_ios});
    m_thread.reset(new std::thread{[this]() { m_ios.run(); }});
  }

  void StartClient(size_t duration_sec, const std::string& raw_ip_address,
                                unsigned short port_num, Callback callback, size_t request_id, const std::string& string) {

//    std::string request = "Есть контакт клиента, время соединения: " + std::to_string(duration_sec) + "\n";
    std::string request = string  + "\n";


    auto session = std::make_shared<Session>(m_ios, raw_ip_address, port_num, request, request_id, callback);
    session->m_sock.open(session->m_ep.protocol());

    std::unique_lock<std::mutex> lock{m_active_sessions_guard};
    m_active_sessions[request_id] = session;

    session->m_sock.async_connect(session->m_ep, boost::bind(&AsyncTCPClient::handle_connect, this, session, _1));

  }
  void handle_connect(std::shared_ptr<Session> session, const system::error_code& connect_ec) {
      if (connect_ec.value() != 0) {
        session->m_ec = connect_ec;
        onRequestComplete(session);
      }

      std::unique_lock<std::mutex> cancel_lock{session->m_cancel_guard};
      if (session->m_was_cancelled) {
              m_reConnectTimer.expires_from_now(boost::posix_time::seconds(30));
              m_reConnectTimer.async_wait(boost::bind(&AsyncTCPClient::do_reconnect, this, session, _1));
              onRequestComplete(session);
      }
      asio::async_write(session->m_sock, asio::buffer(session->m_request, session->m_request.length()),
                        boost::bind(&AsyncTCPClient::handle_write, this, session, _1, _2));
  }
  void handle_write(std::shared_ptr<Session> session, const system::error_code& write_ec, std::size_t  bytes_transferred ) {
      if (write_ec.value() != 0) {
        session->m_ec = write_ec;
        onRequestComplete(session);
      }

      std::unique_lock<std::mutex> cancel_lock{session->m_cancel_guard};
      if (session->m_was_cancelled) {
        onRequestComplete(session);
      }
      asio::async_read_until(session->m_sock, session->m_response_buf, '\n',
                     boost::bind(&AsyncTCPClient::handle_read_until, this, session, _1, _2));
  }
  void handle_read_until(std::shared_ptr<Session> session, const system::error_code& read_ec, std::size_t  bytes_transferred ) {
      if (read_ec.value() != 0) {
        session->m_ec = read_ec;
      } else {
        std::istream strm{&session->m_response_buf};
        std::getline(strm, session->m_response);
      }

      onRequestComplete(session);
  }

void do_reconnect(std::shared_ptr<Session> session, const system::error_code& connect_ec) {
    if (connect_ec.value() != 0) {
      session->m_ec = connect_ec;
      onRequestComplete(session);
    }
    session->m_sock.close();
    m_reConnectTimer.cancel();
    session->m_sock.async_connect(session->m_ep, boost::bind(&AsyncTCPClient::handle_connect, this, session, _1));
}


  void close() {

    m_work.reset(nullptr);

    m_thread->join();
  }

private:
  void onRequestComplete(std::shared_ptr<Session> session) {

    system::error_code ignored_ec;
    session->m_sock.shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);

    std::unique_lock<std::mutex> lock{m_active_sessions_guard};

      auto it = m_active_sessions.find(session->m_id);
      if (it != m_active_sessions.end()) {
        m_active_sessions.erase(it);
      }

    system::error_code ec;
    if (session->m_ec.value() == 0 && session->m_was_cancelled) {
      ec = asio::error::operation_aborted;
    } else {
      ec = session->m_ec;
    }

    session->m_callback(session->m_id, session->m_response, ec);
  }
};


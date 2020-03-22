#pragma once

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <atomic>
#include <cassert>
#include <memory>
#include <thread>
#include <iostream>

#include <matrix_initialised.hpp>


using namespace boost;

class Service {
private:
  std::shared_ptr<asio::ip::tcp::socket> m_sock;
  std::string m_response;
  asio::streambuf m_request;
  boost::array<char, 1024> buff;

public:
  Service(std::shared_ptr<asio::ip::tcp::socket> sock) : m_sock{sock} {}

  void readRequest() {
    asio::async_read_until(*m_sock.get(), m_request, '\n',
                           [this](const system::error_code& ec, std::size_t bytes_transferred) {
                              std::string result{asio::buffers_begin(m_request.data()), asio::buffers_end(m_request.data())};
                              std::cout << "Request from client => " << result << std::endl;
                              onRequestReceived(ec, bytes_transferred);
                           });
}

private:
  void onRequestReceived(const system::error_code& ec, std::size_t  bytes_transferred ) {
    if (ec.value() != 0) {
      std::cout << "Error occurred! Error code = {" << ec.value() << "}. Message: {}" <<  ec.message() <<std::endl;
      onFinish();
      return;
    }

    m_response = ProcessRequest(m_request);
    asio::async_write(*m_sock.get(), asio::buffer(m_response),
                      [this](const system::error_code& write_ec, std::size_t write_bytes_transferred) {

                        onResponseSent(write_ec, write_bytes_transferred);
                      });
//    m_response.erase();
  }

  void onResponseSent(const system::error_code& ec, std::size_t  bytes_transferred ) {
    if (ec.value() != 0) {
      std::cout << "Error occurred! Error code = {" << ec.value() << "}. Message: {}" <<  ec.message() <<std::endl;
    }
    onFinish();
  }

  void onFinish() { delete this; }

  std::string ProcessRequest(asio::streambuf& buf) {
        std::string line{asio::buffers_begin(buf.data()), asio::buffers_end(buf.data())};
    int i = 0;
        while (i != 1000000) i++;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    if(line != "Hello_servac\n") {
        try {
            int i = std::stoi(line);

            return  multiply_matrix(i) + "\n";

        } catch (std::invalid_argument const& ec) {
            std::cout << "Bad argument, only: [8-16-32-64]" << std::endl;
        }
    }
   else return "Hello_client\n";
  }
};

class Acceptor {
private:
  asio::io_service& m_ios;
  asio::ip::tcp::acceptor m_acceptor;
  std::atomic<bool> m_isStopped;

public:
  Acceptor(asio::io_service& ios, unsigned short port_num)
      : m_ios{ios},
        m_acceptor{m_ios, asio::ip::tcp::endpoint(asio::ip::address_v4::any(), port_num)},
        m_isStopped{false} {}


  void Start() {
    m_acceptor.listen();
    InitAccept();
  }


  void Stop() { m_isStopped.store(true); }

private:
  void InitAccept() {
    auto sock = std::make_shared<asio::ip::tcp::socket>(m_ios);

    m_acceptor.async_accept(*sock.get(), [this, sock](const system::error_code& ec) { onAccept(ec, sock); });
  }

  void onAccept(const system::error_code& ec, std::shared_ptr<asio::ip::tcp::socket> sock) {
    if (ec.value() == 0) {
      (new Service(sock))->readRequest();
    } else {
      std::cout << "Error occurred! Error code = {" << ec.value() << "}. Message: {}" <<  ec.message() <<std::endl;
    }

    if (!m_isStopped.load()) {
      InitAccept();
    } else {
      m_acceptor.close();
    }
  }
};

class Server {
private:
  asio::io_service m_ios;
  std::unique_ptr<asio::io_service::work> m_work;
  std::unique_ptr<Acceptor> acc;
  std::vector<std::unique_ptr<std::thread>> m_thread_pool;

public:
  Server() { m_work.reset(new asio::io_service::work(m_ios)); }

  void Start(unsigned short port_num, unsigned int thread_pool_size) {
    assert(thread_pool_size > 0);

    acc.reset(new Acceptor(m_ios, port_num));
    acc->Start();

    for (unsigned int i = 0; i < thread_pool_size; ++i) {
      std::unique_ptr<std::thread> th(new std::thread([this]() { m_ios.run(); }));
      m_thread_pool.push_back(std::move(th));
    }
  }

  void Stop() {
    acc->Stop();
    m_ios.stop();

    for (auto& th : m_thread_pool) {
      th->join();
    }
  }
};


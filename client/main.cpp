#include <iostream>
#include <string>

#include <boost/array.hpp>

//#include <clienttcp2.hpp>
#include <clienttcp3.hpp>

static auto lambda([](size_t request_id, const std::string& response, const system::error_code& ec) {
  if (ec.value() == 0) {
    std::cout << "Запрос #{" << request_id << "} Ответ от сервера: " << response << std::endl;
  } else if (ec == asio::error::operation_aborted) {
    std::cout << "Запрос #{" << request_id << " Zaniato bliat'." << std::endl;
  } else {
    std::cout << "Запрос #{" << request_id << "} failed bliat'! code = {" << ec.value() << "}. Messaga: "
                           << ec.message() << std::endl;
  }
});

int main(int argc, char const* argv[])
{
//    std::string_view host = argv[1];
    int port = std::stoi(argv[1]);
    std::string line;
            int i = 0;

    try {
      AsyncTCPClient client;
      auto start = std::chrono::system_clock::now();

      while(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count() < 80000) {
          std::this_thread::sleep_for(std::chrono::milliseconds(400));
          if(i == 0) {
              line =  "Hello_servac";

          }
          if(i > 0) {
              std::cout << "Enter number 8 or 16 or 32 or 64: ";
              std::cin >> line;
//                  if(line.length() < 1 || line.length() > 9 ) {
//                      std::cout << " failed bliat'! [1-9]" << std::endl;
//                      break;
//                  }
          }
          client.StartClient(500, "127.0.0.1", port, lambda, 1, line);
          line.erase();
           i++;



//      std::this_thread::sleep_for(std::chrono::seconds(1));

//      AsyncTCPClient client("127.0.0.1", port, lambda, 1);

//      client.StartClient(1);

//      std::this_thread::sleep_for(std::chrono::seconds(3));

      // client.StartClient(11, "127.0.0.1", port+1, lambda1, 2);

      // client.cancelRequest(1);

      // std::this_thread::sleep_for(std::chrono::seconds(5));

      // client.StartClient(12, "127.0.0.1", port+2, lambda1, 3);

      // std::this_thread::sleep_for(std::chrono::seconds(8));
       }
      client.close();

    } catch (boost::system::system_error& ec) {
        std::cout << "Error ocured! # " << ec.code() << " Messaga "
                  << ec.what() << std::endl;
    }


    return 0;
}

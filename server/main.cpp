#include <iostream>
#include <servertcp.hpp>

const unsigned int THREAD_POOL_SIZE = 2;

int main(int argc, char const* argv[])
{
//    std::string_view host = argv[1];
    int port = std::stoi(argv[1]);

    try {
        Server srv;

        unsigned int thread_pool_size = std::thread::hardware_concurrency() * 2;
        if (thread_pool_size == 0) thread_pool_size = THREAD_POOL_SIZE;

        srv.Start(port, thread_pool_size);

        std::this_thread::sleep_for(std::chrono::seconds(30));

        srv.Stop();
    } catch (boost::system::system_error& ec) {
        std::cout << "Error ocured! # " << ec.code() << " Messaga "
                  << ec.what() << std::endl;
    }
    return 0;
}
#pragma once


#include <iostream>
#include <functional>
#include <utility>
#include <boost/array.hpp>
#include <boost/timer/timer.hpp>
#include <boost/random.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/thread/future.hpp>

#include <matrix_mult.hpp>

template <size_t N = 3, size_t M = 3>
    using my_matrix = array2d<size_t, N, M, std::vector>;

struct matrix
{
    my_matrix<8, 8>     A_8;
    my_matrix<16, 16>   A_16;
    my_matrix<32, 32>   A_32;
    my_matrix<64, 64>   A_64;
    ///////
    my_matrix<8, 8>     B_8;
    my_matrix<16, 16>   B_16;
    my_matrix<32, 32>   B_32;
    my_matrix<64, 64>   B_64;
    ///////
    my_matrix<8, 8>     C_8;
    my_matrix<16, 16>   C_16;
    my_matrix<32, 32>   C_32;
    my_matrix<64, 64>   C_64;

};


boost::mutex mutex;

void init_multiply_matrix() {
    matrix mtrx;
    std::time_t now = std::time(0);
    boost::random::mt19937 gen{static_cast<std::uint32_t> (now)};
    boost::random::uniform_int_distribution<> dist{1, 3};

    std::vector<boost::thread> thrd;

        thrd.push_back(boost::thread([&mtrx, &gen, &dist]() mutable {

                    boost::lock_guard<boost::mutex> lock{mutex};
                    auto vec_8 = std::make_unique<std::vector<size_t>>();
                    for (size_t i = 0; i < 8*8; i++)
                        {
                            vec_8->push_back(dist(gen));
                        }
                    boost::timer::cpu_timer tmr;
                    mtrx.A_8.init_list(*vec_8);
                    mtrx.B_8.init_list(*vec_8);

                    mult_siml<8, 8>(mtrx.A_8, mtrx.B_8, mtrx.C_8);
                    std::cout <<"Simple\t8x8 " << tmr.format();
        }));

        thrd.push_back(boost::thread([&mtrx, &gen, &dist]() mutable {
                    boost::lock_guard<boost::mutex> lock{mutex};
                    auto vec_16 = std::make_unique<std::vector<size_t>>();
                    for (size_t i = 0; i < 16*16; i++)
                        {
                            vec_16->push_back(dist(gen));
                        }
                    boost::timer::cpu_timer tmr;
                    mtrx.A_16.init_list(*vec_16);
                    mtrx.B_16.init_list(*vec_16);

                    mult_siml<16, 16>(mtrx.A_16, mtrx.B_16, mtrx.C_16);
                    std::cout <<"Simple\t16x16 " << tmr.format();
        }));

        thrd.push_back(boost::thread([&mtrx, &gen, &dist]() mutable {
                    boost::lock_guard<boost::mutex> lock{mutex};
                    auto vec_32 = std::make_unique<std::vector<size_t>>();
                    for (size_t i = 0; i < 32*32; i++)
                        {
                            vec_32->push_back(dist(gen));
                        }
                    boost::timer::cpu_timer tmr;
                    mtrx.A_32.init_list(*vec_32);
                    mtrx.B_32.init_list(*vec_32);

                    mult_siml<32, 32>(mtrx.A_32, mtrx.B_32, mtrx.C_32);
                    std::cout <<"Simple\t32x32 " << tmr.format();
            }));

        thrd.push_back(boost::thread([&mtrx, &gen, &dist]() mutable {
                    boost::lock_guard<boost::mutex> lock{mutex};
                    auto vec_64 = std::make_unique<std::vector<size_t>>();
                    for (size_t i = 0; i < 64*64; i++)
                        {
                            vec_64->push_back(dist(gen));
                        }
                    boost::timer::cpu_timer tmr;
                    mtrx.A_64.init_list(*vec_64);
                    mtrx.B_64.init_list(*vec_64);

                    mult_siml<64, 64>(mtrx.A_64, mtrx.B_64, mtrx.C_64);
                    std::cout <<"Simple\t64x64 " << tmr.format();
            }));
            /////////META//////////////////////////////////////////

            thrd.push_back(boost::thread([&mtrx, &gen, &dist]() mutable {
                    boost::lock_guard<boost::mutex> lock{mutex};
                    auto vec_8 = std::make_unique<std::vector<size_t>>();
                    for (size_t i = 0; i < 8*8; i++)
                        {
                            vec_8->push_back(dist(gen));
                        }
                    boost::timer::cpu_timer tmr;
                    mtrx.A_8.init_list(*vec_8);
                    mtrx.B_8.init_list(*vec_8);

                    mult_meta<8, 8>(mtrx.A_8, mtrx.B_8, mtrx.C_8);
                    std::cout <<"Metaprog 8x8 " << tmr.format();
            }));

            thrd.push_back(boost::thread([&mtrx, &gen, &dist]() mutable {
                    boost::lock_guard<boost::mutex> lock{mutex};
                    auto vec_16 = std::make_unique<std::vector<size_t>>();
                    for (size_t i = 0; i < 16*16; i++)
                        {
                            vec_16->push_back(dist(gen));
                        }
                    boost::timer::cpu_timer tmr;
                    mtrx.A_16.init_list(*vec_16);
                    mtrx.B_16.init_list(*vec_16);

                    mult_meta<16, 16>(mtrx.A_16, mtrx.B_16, mtrx.C_16);
                    std::cout <<"Metaprog 16x16 " << tmr.format();
            }));

            thrd.push_back(boost::thread([&mtrx, &gen, &dist]() mutable {
                    boost::lock_guard<boost::mutex> lock{mutex};
                    auto vec_32 = std::make_unique<std::vector<size_t>>();
                    for (size_t i = 0; i < 32*32; i++)
                        {
                            vec_32->push_back(dist(gen));
                        }
                    boost::timer::cpu_timer tmr;
                    mtrx.A_32.init_list(*vec_32);
                    mtrx.B_32.init_list(*vec_32);

                    mult_meta<32, 32>(mtrx.A_32, mtrx.B_32, mtrx.C_32);
                    std::cout <<"Metaprog 32x32 " << tmr.format();
            }));

            thrd.push_back(boost::thread([&mtrx, &gen, &dist]() mutable {
                    boost::lock_guard<boost::mutex> lock{mutex};
                    auto vec_64 = std::make_unique<std::vector<size_t>>();
                    for (size_t i = 0; i < 64*64; i++)
                        {
                            vec_64->push_back(dist(gen));
                        }
                    boost::timer::cpu_timer tmr;
                    mtrx.A_64.init_list(*vec_64);
                    mtrx.B_64.init_list(*vec_64);

                    mult_meta<64, 64>(mtrx.A_64, mtrx.B_64, mtrx.C_64);
                    std::cout <<"Metaprog 64x64 " << tmr.format();
            }));

    for(auto& x : thrd) x.join();
}

std::string multiply_matrix(int i) {
    matrix mtrx;
    std::time_t now = std::time(0);
    boost::random::mt19937 gen{static_cast<std::uint32_t> (now)};
    boost::random::uniform_int_distribution<> dist{1, 3};

if(i == 8) {
    boost::lock_guard<boost::mutex> lock{mutex};
    auto vec_8 = std::make_unique<std::vector<size_t>>();
    for (size_t i = 0; i < 8*8; i++)
        {
            vec_8->push_back(dist(gen));
        }
    boost::timer::cpu_timer tmr;
    mtrx.A_8.init_list(*vec_8);
    mtrx.B_8.init_list(*vec_8);

    mult_siml<8, 8>(mtrx.A_8, mtrx.B_8, mtrx.C_8);
    return  "Simple\t8x8 " + tmr.format();
}
else if(i == 16) {
    boost::lock_guard<boost::mutex> lock{mutex};
    auto vec_16 = std::make_unique<std::vector<size_t>>();
    for (size_t i = 0; i < 16*16; i++)
        {
            vec_16->push_back(dist(gen));
        }
    boost::timer::cpu_timer tmr;
    mtrx.A_16.init_list(*vec_16);
    mtrx.B_16.init_list(*vec_16);

    mult_siml<16, 16>(mtrx.A_16, mtrx.B_16, mtrx.C_16);
    return  "Simple\t16x16 " + tmr.format();
}
else if(i == 32) {
    boost::lock_guard<boost::mutex> lock{mutex};
    auto vec_32 = std::make_unique<std::vector<size_t>>();
    for (size_t i = 0; i < 32*32; i++)
        {
            vec_32->push_back(dist(gen));
        }
    boost::timer::cpu_timer tmr;
    mtrx.A_32.init_list(*vec_32);
    mtrx.B_32.init_list(*vec_32);

    mult_siml<32, 32>(mtrx.A_32, mtrx.B_32, mtrx.C_32);
    return  "Simple\t32x32 " + tmr.format();
}
else if(i == 64) {
    boost::lock_guard<boost::mutex> lock{mutex};
    auto vec_64 = std::make_unique<std::vector<size_t>>();
    for (size_t i = 0; i < 64*64; i++)
        {
            vec_64->push_back(dist(gen));
        }
    boost::timer::cpu_timer tmr;
    mtrx.A_64.init_list(*vec_64);
    mtrx.B_64.init_list(*vec_64);

    mult_siml<64, 64>(mtrx.A_64, mtrx.B_64, mtrx.C_64);
    return  "Simple\t64x64 " + tmr.format();
}
else return  "Bad format! must be: 8-16-32-64";
}



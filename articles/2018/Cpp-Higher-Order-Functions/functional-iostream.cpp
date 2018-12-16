// Functional Transpose by BOT Man, 2018
// clang++ functional-rref.cpp -std=c++11 -Wall -o rref && ./rref

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

void EchoIO(std::ifstream& ifs, std::ofstream& ofs) {
  std::copy(std::istream_iterator<std::string>(ifs.is_open() ? ifs : std::cin),
            std::istream_iterator<std::string>(),
            std::ostream_iterator<std::string>(ofs.is_open() ? ofs : std::cout,
                                               "\n"));
}

int main(int argc, char* argv[]) {
  auto ifs = argc >= 2 ? std::ifstream(argv[1]) : std::ifstream();
  auto ofs = argc >= 3 ? std::ofstream(argv[2]) : std::ofstream();
  EchoIO(ifs, ofs);

#ifdef NOT_COMPILE
  EchoIO(argc >= 2 ? std::ifstream(argv[1]) : std::ifstream(),
         argc >= 3 ? std::ofstream(argv[2]) : std::ofstream());
#endif  // NOT_COMPILE

  auto echo_wrapper = [](std::ifstream&& ifs, std::ofstream&& ofs) -> void {
    EchoIO(ifs, ofs);
  };
  echo_wrapper(argc >= 2 ? std::ifstream(argv[1]) : std::ifstream(),
               argc >= 3 ? std::ofstream(argv[2]) : std::ofstream());
  return 0;
}

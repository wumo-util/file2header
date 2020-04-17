#include <iostream>
#include <fstream>
#include <cxxopts.hpp>
#include <regex>
#include <cmath>

int main(int argc, char **argv) {
  cxxopts::Options options("file2header", "helper to convert file to header");
  auto option = options.add_options();
  option("i,input", "input file", cxxopts::value<std::string>());
  option(
    "o,output", "output header file name without extension",
    cxxopts::value<std::string>());
  option("n,name", "variable name", cxxopts::value<std::string>());
  std::string input, output, name;
  try {
    auto result = options.parse(argc, argv);
    input = result["input"].as<std::string>();
    output = result["output"].as<std::string>();
    name = result["name"].as<std::string>();
  } catch(std::exception &e) {
    std::cerr << "error: " << e.what() << std::endl
              << "usage:" << argv[0] << " -i file -o header" << std::endl;
  }
  try {
    // Open the stream to 'lock' the file.
    std::ifstream f{input, std::ios::ate | std::ios::binary};
    if(!f.good()) throw std::runtime_error(input + ": " + std::strerror(errno));
    const auto size = size_t(f.tellg());
    f.seekg(0, std::ios::beg);

    std::vector<uint32_t> buffer;
    buffer.resize(std::ceil(float(size) / sizeof(uint32_t)));

    f.read(reinterpret_cast<char *>(buffer.data()), size);
    std::ofstream fout(output);
    std::regex escape("[^a-zA-Z0-9]");
    std::string cxxName;
    std::regex_replace(
      std::back_inserter(cxxName), name.begin(), name.end(), escape, "_");
    auto i = 0, line = 10;
    fout << "#pragma once" << std::endl
         << "const uint32_t " << cxxName << "[]={" << std::endl;
    for(auto &data: buffer) {
      fout << "0x" << std::hex << data << ",";
      if(i++ > 10) {
        fout << std::endl;
        i = 0;
      }
    }
    fout << "};" << std::endl;
  } catch(std::exception &e) { std::cerr << e.what() << std::endl; }
  return 0;
}

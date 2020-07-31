#include <iostream>
#include <fstream>
#include <cxxopts.hpp>
#include <regex>
#include <cmath>
#include <filesystem>

inline char separator() {
#ifdef _WIN32
  return '\\';
#else
  return '/';
#endif
}

bool endWith(std::string const &fullString, std::string const &ending) {
  if(fullString.length() >= ending.length())
    return 0 == fullString.compare(
                  fullString.length() - ending.length(), ending.length(), ending);
  else
    return false;
}

auto escape(std::string name) -> std::string {
  std::regex escape("[^a-zA-Z0-9]");
  std::string cxxName;
  std::regex_replace(std::back_inserter(cxxName), name.begin(), name.end(), escape, "_");
  return cxxName;
}

int main(int argc, char **argv) {
  cxxopts::Options options("file2header", "helper to convert file to header");
  auto option = options.add_options();
  option("i,input", "input file", cxxopts::value<std::string>());
  option("o,output", "output directory", cxxopts::value<std::string>());
  option("n,name", "variable name", cxxopts::value<std::string>());
  option(
    "s,namespace", "namespace name", cxxopts::value<std::string>()->default_value(""));
  option("r,relative", "relative path", cxxopts::value<std::string>()->default_value(""));

  std::string input, output, name, ns, relative;
  try {
    auto result = options.parse(argc, argv);
    input = result["input"].as<std::string>();
    output = result["output"].as<std::string>();
    name = result["name"].as<std::string>();
    ns = result["namespace"].as<std::string>();
    relative = result["relative"].as<std::string>();
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

    auto cxxName = escape(name);
    std::string nsBegin{""}, nsEnd{""};
    if(!ns.empty()) {
      std::stringstream ss;
      ss << "namespace " << escape(ns);
      std::filesystem::path path(relative);

      for(auto &p: path)
        ss << "::" << escape(p.string());
      ss << "{";
      nsBegin = ss.str();
      nsEnd = "}";
    }

    output = (endWith(output, "/") || endWith(output, "\\")) ? output :
                                                               output + separator();
    std::ofstream fheader(output + cxxName + ".hpp");
    std::ofstream fcpp(output + cxxName + ".cpp");

    fheader << "#pragma once" << std::endl
            << R"(
#if !defined(FILE2HEADER_EXPORTED)
  #if defined(_WIN32) && defined(FILE2HEADER_BUILD_SHARED_LIBRARY)
    #define FILE2HEADER_EXPORTED __declspec(dllimport)
  #else
    #define FILE2HEADER_EXPORTED
  #endif
#endif
)" << std::endl
            << "#include <cstdint>" << std::endl
            << "#if defined(__cpp_lib_span) && __cpp_lib_span >= 201902L && "
               "__has_include(<span>)"
            << std::endl
            << "#include <span>" << std::endl
            << std::endl
            << "#endif" << std::endl
            << nsBegin << std::endl
            << "const uint32_t " << cxxName << "_size=" << buffer.size() << ";"
            << std::endl
            << "FILE2HEADER_EXPORTED extern const uint32_t " << cxxName << "["
            << cxxName + "_size];" << std::endl
            << "#if defined(__cpp_lib_span) && __cpp_lib_span >= 201902L && "
               "__has_include(<span>)"
            << std::endl
            << "const std::span<const uint32_t> " << cxxName << "_span{" << cxxName
            << ", " << cxxName << "_size};" << std::endl
            << "#endif" << std::endl
            << nsEnd << std::endl;

    auto i = 0, line = 10;
    fcpp << "#include \"" << cxxName << ".hpp"
         << "\"" << std::endl
         << nsBegin << std::endl
         << "const uint32_t " << cxxName << "[]={" << std::endl;
    for(auto &data: buffer) {
      fcpp << "0x" << std::hex << data << ",";
      if(i++ > line) {
        fcpp << std::endl;
        i = 0;
      }
    }
    fcpp << "};" << std::endl << nsEnd << std::endl;
  } catch(std::exception &e) { std::cerr << e.what() << std::endl; }
  return 0;
}

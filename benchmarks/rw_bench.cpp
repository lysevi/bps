/**
running example:
rw_bench.exe -B 1000 --deep 10
*/
#include <cxxopts.hpp>
#include <iostream>
#include <rstore/rstore.h>

size_t B = 1000;
size_t deep = 10;

int main(int argc, char **argv) {
  cxxopts::Options options("Read/Write benchmark", "");
  options.allow_unrecognised_options();
  options.positional_help("[optional args]").show_positional_help();

  auto add_o = options.add_options();
  add_o("h,help", "Help");
  add_o("B", "B - minimal block size", cxxopts::value<size_t>(B));
  add_o("d,deep", "Tree deep", cxxopts::value<size_t>(deep));

  try {
    cxxopts::ParseResult result = options.parse(argc, argv);

    if (result["help"].as<bool>()) {
      std::cout << options.help() << std::endl;
      std::cout << "Usage example:" << std::endl;
      std::cout << " rw_bench.exe -B 1000 --deep 10" << std::endl;
      std::exit(0);
    }
  } catch (cxxopts::OptionException &ex) {
    std::cerr << ex.what() << std::endl;
    return -1;
  }

  std::cout << "B: " << B << std::endl;
  std::cout << "deep: " << deep << std::endl;

  rstore::Tree::Params params;
  params.B = B;

  rstore::Tree t(params);
  t.init();

  size_t k = 0;
  size_t v = 0;
  rstore::utils::ElapsedTime write_time;
  while (t.deep() < deep) {
    t.insert(rstore::slice_make_from(k), rstore::slice_make_from(v));
    t.find(rstore::slice_make_from(k));
    ++k;
    v += size_t(2);
  }
  auto elapsed_time = write_time.elapsed();
  std::cout << "Write:" << std::endl;
  std::cout << "\tcount:" << k << std::endl;
  std::cout << "\ttotal:" << elapsed_time << std::endl;
  std::cout << "\tmean:" << elapsed_time / k << std::endl;

  rstore::utils::ElapsedTime read_time;
  for (size_t i = 0; i < k; ++i) {
    t.find(rstore::slice_make_from(i));
  }
  elapsed_time = read_time.elapsed();
  std::cout << "Read:" << std::endl;
  std::cout << "\ttotal:" << elapsed_time << std::endl;
  std::cout << "\tmean:" << elapsed_time / k << std::endl;
}
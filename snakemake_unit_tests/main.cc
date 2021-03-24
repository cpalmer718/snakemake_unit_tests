/*!
  @file main.cc
  @brief main entry/exit for software. interprets command line arguments,
  dispatches tasks, exits
  @author Lightning Auriga
  @copyright Released under the MIT License. Copyright
  2021 Lightning Auriga
 */

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "boost/filesystem.hpp"
#include "snakemake_unit_tests/cargs.h"
#include "snakemake_unit_tests/rule_block.h"
#include "snakemake_unit_tests/snakemake_file.h"
#include "snakemake_unit_tests/solved_rules.h"
#include "snakemake_unit_tests/yaml_reader.h"

/*!
  @brief main program implementation
  @param argc number of command line entries, including program name
  @param argv array of command line entries
  @return exit code: 0 on success, nonzero otherwise
 */
int main(int argc, char **argv) {
  // parse command line input
  snakemake_unit_tests::cargs ap(argc, argv);
  snakemake_unit_tests::params p;
  // if help is requested or no flags specified
  if (ap.help() || argc == 1) {
    // print a help message and exist
    ap.print_help(std::cout);
    return 0;
  }
  p = ap.set_parameters();

  // parse the top-level snakefile and all include files (hopefully)
  snakemake_unit_tests::snakemake_file sf;
  sf.load_file(p.snakefile.filename().string(),
               p.snakefile.parent_path().string(), p.verbose);

  // as a debug step, report the parsed contents of the snakefile
  if (p.verbose) {
    sf.print_blocks(std::cout);
  }
  // parse the log file to determine the solved system of rules and outputs
  snakemake_unit_tests::solved_rules sr;
  sr.load_file(p.snakemake_log.string());

  // iterate over the solved rules, emitting them with modifiers as desired
  // TODO(lightning-auriga): refactor this function and clean it up
  sr.emit_tests(sf, p.output_test_dir, p.pipeline_run_dir, p.inst_dir,
                p.exclude_rules, p.added_files, p.added_directories);

  std::cout << "all done woo!" << std::endl;
  return 0;
}

/*!
 @file snakemake_file.h
 @brief representation of snakemake file as a sequence
 of rule blocks and code chunks
 @author Lightning Auriga
 @copyright Released under the MIT License.
 Copyright 2023 Lightning Auriga
 */

#ifndef SNAKEMAKE_UNIT_TESTS_SNAKEMAKE_FILE_H_
#define SNAKEMAKE_UNIT_TESTS_SNAKEMAKE_FILE_H_

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "boost/filesystem.hpp"
#include "snakemake_unit_tests/rule_block.h"

namespace snakemake_unit_tests {
class snakemake_file {
 public:
  /*!
    @brief default constructor
   */
  snakemake_file() {}
  /*!
    @brief copy constructor
    @param obj existing snakemake_file object
   */
  snakemake_file(const snakemake_file &obj) : _blocks(obj._blocks) {}
  /*!
    @brief destructor
   */
  ~snakemake_file() throw() {}

  /*!
   @brief load and parse a snakemake file
   @param filename name of file to load
   @param base_dir directory from which to base
   relative file paths
   @param verbose whether to emit verbose
   logging output

   designed to be called recursively to handle
   include statements
  */
  void load_file(const std::string &filename, const std::string &base_dir,
                 bool verbose);

  /*!
    @brief print block contents to stream
    @param out open stream to which to write data

    this function is primarily intended for debugging purposes at this time
   */
  void print_blocks(std::ostream &out) const;

  /*!
    @brief get const access to internal block representation
    @return const reference to internal block vector
   */
  const std::vector<rule_block> &get_blocks() const { return _blocks; }

  /*!
    @brief report all code blocks but a single requested rule to file
    @param rule_name string name of requested rule
    @param out open output stream to which to write data
   */
  void report_single_rule(const std::string &rule_name,
                          std::ostream &out) const;

 private:
  /*!
    @brief minimal contents of snakemake file as blocks of code
   */
  std::vector<rule_block> _blocks;
};
}  // namespace snakemake_unit_tests

#endif  // SNAKEMAKE_UNIT_TESTS_SNAKEMAKE_FILE_H_

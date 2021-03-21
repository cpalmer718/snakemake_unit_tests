/*!
 @file utilities.h
 @brief general utility functions
 @author Lightning Auriga
 @copyright Released under the MIT License.
 Copyright 2023 Lightning Auriga
 */

#ifndef SNAKEMAKE_UNIT_TESTS_UTILITIES_H_
#define SNAKEMAKE_UNIT_TESTS_UTILITIES_H_

#include <fstream>
#include <string>

namespace snakemake_unit_tests {
/*!
  @brief prune superfluous content from snakemake content line
  @param s line to be pruned down to minimal essential content
  @param input open file stream from which to pull multiline content, if needed
  @param line_number counter of line in file, for error reporting
  @return pruned line

  for the moment, this removes:

  - things that look like docstrings (""" content """)

  - comments (all text after unescaped '#')

  - trailing whitespace (tab or space)
 */
std::string remove_comments_and_docstrings(const std::string &s,
                                           std::ifstream *input,
                                           unsigned *line_number);
}  // namespace snakemake_unit_tests

#endif  // SNAKEMAKE_UNIT_TESTS_UTILITIES_H_

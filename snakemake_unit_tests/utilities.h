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
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "boost/regex.hpp"

namespace snakemake_unit_tests {
/*!
  \brief type indicator for possible open string structures
 */
typedef enum {
  single_tick,
  single_quote,
  triple_tick,
  triple_quote,
  none
} quote_type;

/*!
  \brief determine what a newly encountered ' or "
  means in the context of previously encountered marks
  @param current_line python line currently being processed
  @param active_quote_type if string or literal is currently open,
  the type of opening delimiter found
  @param parse_index index of character being processed in current line
  @param string_open whether there is a currently active single
  delimiter string
  @param literal_open whether there is a currently active triple
  delimiter literal
 */
void resolve_string_delimiter(const std::string &current_line,
                              quote_type *active_quote_type,
                              unsigned *parse_index, bool *string_open,
                              bool *literal_open);
/*!
  \brief add a processed line to a set of processed line, dealing
  with the possibility of consecutive string literals
  @param resolved_line line to be added to aggregated set
  @param aggregated_line previous lines ending with explicit
  line extensions that need to be added as well
  @param results currently existing set of resolved lines
 */
void concatenate_string_literals(const std::string &resolved_line,
                                 std::string *aggregated_line,
                                 std::vector<std::string> *results);
/*!
  \brief prune superfluous content from snakemake content line
  @param lines all loaded lines from file
  @return input data with comments and unassigned string literals pruned

  this modification of the function replaces the initial implementation
  of this function that only partially addressed the problem and
  was known to break in certain toxic corner cases
 */
std::vector<std::string> lexical_parse(const std::vector<std::string> &lines);
/*!
  @brief prune superfluous content from snakemake content line
  @param s line to be pruned down to minimal essential content
  @param loaded_lines currently processed snakemake file lines
  @param line_number counter of line in file, for error reporting
  @return pruned line

  for the moment, this removes:

  - things that look like docstrings (""" content """)

  - comments (all text after unescaped '#')

  - trailing whitespace (tab or space)
 */
std::string remove_comments_and_docstrings(
    const std::string &s, const std::vector<std::string> &loaded_lines,
    unsigned *line_number);
/*!
  @brief take a comma/space delimited list of filenames and break them up into a
  vector
  @param s input list; intended to be from snakemake log data
  @param target vector in which to store data
 */
void split_comma_list(const std::string &s, std::vector<std::string> *target);
}  // namespace snakemake_unit_tests

#endif  // SNAKEMAKE_UNIT_TESTS_UTILITIES_H_

/*!
 @file solved_rules.h
 @brief simple representation of parsed snakemake dag
 @author Lightning Auriga
 @copyright Released under the MIT License.
 Copyright 2023 Lightning Auriga
 */

#ifndef SNAKEMAKE_UNIT_TESTS_SOLVED_RULES_H_
#define SNAKEMAKE_UNIT_TESTS_SOLVED_RULES_H_

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "boost/regex.hpp"
#include "boost/smart_ptr.hpp"
#include "snakemake_unit_tests/snakemake_file.h"
#include "snakemake_unit_tests/utilities.h"

namespace snakemake_unit_tests {
/*!
  @class recipe
  @brief from the snakemake log, a simple description
  of how input(s) lead to output(s) via a rule
 */
class recipe {
 public:
  /*!
    @brief constructor
   */
  recipe()
      : _rule_name(""),
        _rule_is_checkpoint(false),
        _checkpoint_update(false),
        _log("") {}
  /*!
    @brief copy constructor
    @param obj existing recipe object
   */
  recipe(const recipe &obj)
      : _rule_name(obj._rule_name),
        _rule_is_checkpoint(obj._rule_is_checkpoint),
        _checkpoint_update(obj._checkpoint_update),
        _inputs(obj._inputs),
        _outputs(obj._outputs),
        _log(obj._log) {}
  /*!
    @brief destructor
   */
  ~recipe() throw() {}

  /*!
    @brief access rule name
    @return rule name
   */
  const std::string &get_rule_name() const { return _rule_name; }
  /*!
    @brief set rule name
    @param s new rule name
   */
  void set_rule_name(const std::string &s) { _rule_name = s; }
  /*!
    @brief if this rule is a checkpoint
    @return if this rule is a checkpoint
  */
  bool is_checkpoint() const { return _rule_is_checkpoint; }
  /*!
    @brief set checkpoint status
    @param b new setting for checkpoint indicator
   */
  void set_checkpoint(bool b) { _rule_is_checkpoint = b; }
  /*!
    @brief if this rule is affected by a checkpoint
    @return if this rule is affected by a checkpoint
  */
  bool is_checkpoint_update() const { return _checkpoint_update; }
  /*!
    @brief set checkpoint affectedness status
    @param b new setting for checkpoint affectedness indicator
   */
  void set_checkpoint_update(bool b) { _checkpoint_update = b; }
  /*!
    @brief access list of input files
    @return vector storing all input filenames; may be empty
  */
  const std::vector<boost::filesystem::path> &get_inputs() const {
    return _inputs;
  }
  /*!
    @brief add an input filename
    @param s new input filename
   */
  void add_input(const std::string &s) { _inputs.push_back(s); }
  /*!
    @brief access list of output files
    @return vector storing all output filenames; shouldn't be empty
   */
  const std::vector<boost::filesystem::path> &get_outputs() const {
    return _outputs;
  }
  /*!
    @brief add an output filename
    @param s new output filename
   */
  void add_output(const std::string &s) { _outputs.push_back(s); }
  /*!
    @brief access log filename
    @return log filename, if given; else empty string
   */
  const std::string &get_log() const { return _log; }
  /*!
    @brief set log filename
    @param s new log filename
   */
  void set_log(const std::string &s) { _log = s; }
  /*!
    @brief clear all stored contents
   */
  void clear() {
    _rule_name = _log = "";
    _inputs.clear();
    _outputs.clear();
  }

 private:
  /*!
    @brief extracted name of rule from log file
   */
  std::string _rule_name;
  /*!
    @brief whether the rule is a checkpoint

    note that there are indications (28 Apr 2021) of checkpoints not functioning
    on cluster systems, so their use is somewhat dubious
   */
  bool _rule_is_checkpoint;
  /*!
    @brief whether this rule is affected by upstream checkpoints

    this is part of the snakemake logging system, but unfortunately doesn't
    exactly define what step(s) are implicated upstream
   */
  bool _checkpoint_update;
  /*!
    @brief snakemake solved input files to rule

    parsed from ", " delimited list from log output
   */
  std::vector<boost::filesystem::path> _inputs;
  /*!
    @brief snakemake solved output files to rule

    parsed from ", " delimited list from log output
   */
  std::vector<boost::filesystem::path> _outputs;
  /*!
    @brief snakemake solved log file for rule

    only exists if rule has log block, and nothing is
    currently done with this information even if present
   */
  std::string _log;
};
/*!
  @class solved_rules
  @brief store parsed simplified version of snakemake dag,
  focusing simply on which rule is used to generate which output
  from which input
 */
class solved_rules {
 public:
  /*!
    @brief constructor
   */
  solved_rules() {}
  /*!
    @brief copy constructor
    @param obj existing solved_rules object
   */
  solved_rules(const solved_rules &obj)
      : _recipes(obj._recipes), _output_lookup(obj._output_lookup) {}
  /*!
    @brief destructor
   */
  ~solved_rules() throw() {}
  /*!
    @brief load solved recipes from a snakemake log file
    @param filename name of snakemake logfile to parse
   */
  void load_file(const std::string &filename);
  /*!
    @brief emit tests from parsed snakemake information
    @param sf snakemake_file object with rule definitions corresponding
    to loaded log data
    @param output_test_dir output directory for tests (e.g. '.tests/')
    @param pipeline_top_dir parent directory of snakemake pipeline used to
    generate corresponding log file (e.g.: X for X/workflow/Snakefile)
    @param pipeline_run_dir directory in which pipeline was run, relative to
    pipeline_top_dir. if you run the pipeline in the top level directory,
    this should simply be '.'
    @param inst_dir directory in snakemake_unit_tests repo containing
    installation files (when conda mode is enabled, this will default to
    $CONDA_PREFIX/share/snakemake_unit_tests/inst)
    @param exclude_rules map of rules to skip tests for
    @param added_files vector of additional files to add to test workspaces
    @param added_directories vector of additional directories to add to test
    workspaces
    @param update_snakefiles controls whether to print snakefiles
    @param update_added_content controls whether to copy added files and
    directories
    @param update_inputs controls whether to copy rule inputs
    @param update_outputs controls whether to copy rule outputs
    @param update_pytest controls whether to copy pytest infrastructure
  */
  void emit_tests(const snakemake_file &sf,
                  const boost::filesystem::path &output_test_dir,
                  const boost::filesystem::path &pipeline_top_dir,
                  const boost::filesystem::path &pipeline_run_dir,
                  const boost::filesystem::path &inst_dir,
                  const std::map<std::string, bool> &exclude_rules,
                  const std::vector<boost::filesystem::path> &added_files,
                  const std::vector<boost::filesystem::path> &added_directories,
                  bool update_snakefiles, bool update_added_content,
                  bool update_inputs, bool update_outputs,
                  bool update_pytest) const;
  /*!
    @brief emit snakefile from parsed snakemake information
    @param sf snakemake_file object with rule definitions corresponding
    to loaded log data
    @param workspace_path top level of emitted workspace
    @param rec target rule for emission
    @param dependent_rulenames all rule names that should be included in
    the output
    @param requires_phony_all whether the file needs an all target injected.
    this should only be included at top level
    @return whether the rule was found in the snakefile or its dependencies
  */
  bool emit_snakefile(const snakemake_file &sf,
                      const boost::filesystem::path &workspace_path,
                      const boost::shared_ptr<recipe> &rec,
                      const std::map<std::string, bool> &dependent_rulenames,
                      bool requires_phony_all) const;
  /*!
    @brief create a test directory
    @param rec recipe/rule entry for which a workspace should be created
    @param sf snakemake_file object with rule definitions corresponding
    to loaded log data
    @param output_test_dir output directory for tests (e.g. '.tests/')
    @param test_parent_path '.tests/unit' by default
    @param pipeline_top_dir parent directory of snakemake pipeline used to
    generate corresponding log file (e.g.: X for X/workflow/Snakefile)
    @param pipeline_run_dir directory in which pipeline was run, relative to
    pipeline_top_dir. if you run the pipeline in the top level directory,
    this should simply be '.'
    @param test_inst_py snakemake_unit_tests installed test.py script
    installation files (when conda mode is enabled, this will default to
    $CONDA_PREFIX/share/snakemake_unit_tests/inst)
    @param exclude_rules map of rules to skip tests for
    @param added_files vector of additional files to add to test workspaces
    @param added_directories vector of additional directories to add to test
    workspaces
    @param update_snakefiles controls whether to print snakefiles
    @param update_added_content controls whether to copy added files and
    directories
    @param update_inputs controls whether to copy rule inputs
    @param update_outputs controls whether to copy rule outputs
    @param update_pytest controls whether to copy pytest infrastructure
  */
  void create_workspace(
      const boost::shared_ptr<recipe> &rec, const snakemake_file &sf,
      const boost::filesystem::path &output_test_dir,
      const boost::filesystem::path &test_parent_path,
      const boost::filesystem::path &pipeline_top_dir,
      const boost::filesystem::path &pipeline_run_dir,
      const boost::filesystem::path &test_inst_py,
      const std::map<std::string, bool> &exclude_rules,
      const std::vector<boost::filesystem::path> &added_files,
      const std::vector<boost::filesystem::path> &added_directories,
      bool update_snakefiles, bool update_added_content, bool update_inputs,
      bool update_outputs, bool update_pytest) const;
  /*!
    @brief create an empty workspace for python testing
    @param output_test_dir output directory for tests (e.g. '.tests/')
    @param pipeline_dir parent directory of snakemake pipeline used to generate
    corresponding log file (e.g.: X for X/workflow/Snakefile)
    @param added_files vector of additional files to add to test workspaces
    @param added_directories vector of additional directories to add to test
    workspaces
  */
  void create_empty_workspace(
      const boost::filesystem::path &output_test_dir,
      const boost::filesystem::path &pipeline_dir,
      const std::vector<boost::filesystem::path> &added_files,
      const std::vector<boost::filesystem::path> &added_directories) const;

  /*!
    @brief recursively remove empty workspace after python integration is
    complete
    @param output_test_dir output diretory for tests (e.g. '.tests/')
   */
  void remove_empty_workspace(
      const boost::filesystem::path &output_test_dir) const;

  /*!
    @brief copy files/folders enumerated in vector to a location
    @param contents files or folders to be copied
    @param source_prefix parent directory of source files/folders
    @param target_prefix directory destination of files/folders
    @param rule_name label for error reporting
   */
  void copy_contents(const std::vector<boost::filesystem::path> &contents,
                     const boost::filesystem::path &source_prefix,
                     const boost::filesystem::path &target_prefix,
                     const std::string &rule_name) const;

  /*!
    @brief report phony all target controlling test snakemake run
    @param out stream to which to write data
    @param targets phony targets for all rule
   */
  void report_phony_all_target(
      std::ostream &out,
      const std::vector<boost::filesystem::path> &targets) const;
  /*!
    @brief copy over pytest script with certain additions
    @param parent_dir parent directory of test workspace
    @param test_dir parent directory of all unit tests for the pipeline
    @param rule_name name of rule whose test is being emitted
    @param snakefile_relative_path relative path of snakefile in pipeline dir
    @param pipeline_run_dir relative path of snakemake execution within pipeline
    @param inst_test_py snakemake_unit_tests test.py script location
   */
  void report_modified_test_script(
      const boost::filesystem::path &parent_dir,
      const boost::filesystem::path &test_dir, const std::string &rule_name,
      const boost::filesystem::path &snakefile_relative_path,
      const boost::filesystem::path &pipeline_run_dir,
      const boost::filesystem::path &inst_test_py) const;

  /*!
    @brief determine unavoidable dependencies for query rule
    @param sf snakemake top level file with all rules
    @param rec query rule
    @param target full set of unavoidable dependencies
   */
  void aggregate_dependencies(
      const snakemake_file &sf, const boost::shared_ptr<recipe> &rec,
      std::map<boost::shared_ptr<recipe>, bool> *target) const;
  /*!
    @brief add rules and all dependencies starting from a particular leaf
    @param rec leaf to start adding things from
    @param already_added tracker for nodes already searched
    @param target storage for included nodes
   */
  void add_dag_from_leaf(
      const boost::shared_ptr<recipe> &rec,
      std::map<boost::shared_ptr<recipe>, bool> *already_added,
      std::map<boost::shared_ptr<recipe>, bool> *target) const;
  /*!
    @brief compute whether a rule has any checkpoint dependencies
    @param rec leaf to start scanning dependencies from
    @param target tracker for results
   */
  void compute_dependency_checkpoints(
      const boost::shared_ptr<recipe> &rec,
      std::map<boost::shared_ptr<recipe>, bool> *target) const;

 private:
  /*!
    @brief abstract set of solved recipe entries in a log file
   */
  std::vector<boost::shared_ptr<recipe> > _recipes;
  /*!
    @brief allow lookup of output->recipe for dependency resolution
   */
  std::map<boost::filesystem::path, boost::shared_ptr<recipe> > _output_lookup;
};
}  // namespace snakemake_unit_tests

#endif  // SNAKEMAKE_UNIT_TESTS_SOLVED_RULES_H_

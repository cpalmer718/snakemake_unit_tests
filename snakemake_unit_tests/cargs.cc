/*!
  \file cargs.cc
  \brief method implementation for command line argument parser class
  \copyright Released under the MIT License.
  Copyright 2023 Lightning Auriga
*/

#include "snakemake_unit_tests/cargs.h"

void snakemake_unit_tests::cargs::initialize_options() {
  _desc.add_options()(
      "config,c",
      boost::program_options::value<std::string>()->default_value(""),
      "config yaml file specifying default options for other flags")(
      "added-directories,d",
      boost::program_options::value<std::vector<std::string> >(),
      "optional set of relative directory paths that will be installed "
      "alongside tests")(
      "exclude-rules,e",
      boost::program_options::value<std::vector<std::string> >(),
      "optional set of rules to skip for testing")(
      "added-files,f",
      boost::program_options::value<std::vector<std::string> >(),
      "optional set of relative file paths that will be installed alongside "
      "tests")("help,h", "emit this help message")(
      "inst-dir,i", boost::program_options::value<std::string>(),
      "snakemake_unit_tests inst directory")(
      "snakemake-log,l", boost::program_options::value<std::string>(),
      "snakemake log file for run that needs unit tests")(
      "output-test-dir,o",
      boost::program_options::value<std::string>()->default_value(".tests"),
      "top-level output directory for all tests")(
      "pipeline-dir,p",
      boost::program_options::value<std::string>()->default_value(""),
      "top-level run directory for actual instance of pipeline (if not "
      "specified, "
      "will be computed as * assuming --snakefile is */workflow/Snakefile)")(
      "snakefile,s",
      boost::program_options::value<std::string>()->default_value(
          "workflow/Snakefile"),
      "snakefile used to run target pipeline")(
      "verbose,v", "emit verbose logging content; useful for debugging");
}

snakemake_unit_tests::params snakemake_unit_tests::cargs::set_parameters()
    const {
  params p;
  // start with config yaml
  p.config_filename = get_config_yaml();
  // if the user specified a configuration file
  if (!p.config_filename.string().empty()) {
    // if the file exists at all
    if (boost::filesystem::is_regular_file(p.config_filename)) {
      // attempt to load it into yaml-cpp Node
      try {
        p.config.load_file(p.config_filename.string());
      } catch (...) {
        // this is *probably* the issue, if the file definitely exists
        throw std::runtime_error("input config file \"" +
                                 p.config_filename.string() +
                                 "\" does not conform to yaml syntax");
      }
      // TODO(lightning-auriga): populate entries from yaml into params object
    }
  }
  // load command line options
  // deal with overriding options from the config yaml
  // verbosity: just accept CLI, this shouldn't be a config yaml option
  p.verbose = verbose();
  // output_test_dir: override if specified
  p.output_test_dir =
      override_if_specified(get_output_test_dir(), p.output_test_dir);
  // snakefile: override if specified
  p.snakefile = override_if_specified(get_snakefile(), p.snakefile);
  // pipeline_run_dir: override if specified, but then handle differently
  p.pipeline_run_dir =
      override_if_specified(get_pipeline_dir(), p.pipeline_run_dir);
  if (p.pipeline_run_dir.string().empty()) {
    // behavior: if not specified, select it as the directory above
    // wherever the snakefile is installed
    // so we're assuming Snakefile is "something/workflow/Snakefile",
    // and we're setting pipeline_run_dir to "something"
    p.pipeline_run_dir =
        p.snakefile.remove_trailing_separator().parent_path().parent_path();
  }
  // inst_dir: override if specified
  p.inst_dir = override_if_specified(get_inst_dir(), p.inst_dir);
  // snakemake_log: override if specified
  p.snakemake_log = override_if_specified(get_snakemake_log(), p.snakemake_log);
  // added_files: augment whatever is present in config.yaml
  add_contents<boost::filesystem::path>(get_added_files(), &p.added_files);
  // added_directories: augment whatever is present in config.yaml
  add_contents<boost::filesystem::path>(get_added_directories(),
                                        &p.added_directories);
  // exclude_rules: augment whatever is present in config.yaml
  add_contents<std::string>(get_exclude_rules(), &p.exclude_rules);
  // add "all" to exclusion list, always
  // it's ok if it dups with user specification, it's uniqued later
  p.exclude_rules.push_back("all");

  // consistency checks
  // verbose is fine regardless
  // output_test_dir: doesn't have trailing separator, but doesn't have to exist
  p.output_test_dir = p.output_test_dir.remove_trailing_separator();
  // snakefile: should exist, be regular file
  check_regular_file(p.snakefile, "", "snakefile");
  // pipeline_run_dir: should exist, be directory, no trailing separator
  check_and_fix_dir(&p.pipeline_run_dir, "", "pipeline-dir");
  // inst_dir: should exist, be directory
  check_and_fix_dir(&p.inst_dir, "", "inst-dir");
  //     should also contain two files: test.py and common.py
  try {
    check_regular_file("test.py", p.inst_dir, "inst-dir/test.py");
    check_regular_file("common.py", p.inst_dir, "inst-dir/common.py");
  } catch (...) {
    throw std::runtime_error(
        "inst directory \"" + p.inst_dir.string() +
        "\" exists, but"
        " doesn't appear to contain either 'common.py' or 'test.py',"
        " required infrastructure files from snakemake_unit_tests. "
        "If you've cloned and built snakemake_unit_tests_locally, "
        "you should provide /path/to/snakemake_unit_tests/inst for "
        "this option; otherwise, if using conda, you can provide "
        "$CONDA_PREFIX/share/snakemake_unit_tests/inst");
  }
  // snakemake_log: should exist, be a regular file
  check_regular_file(p.snakemake_log, "", "snakemake-log");
  // added_files: should be regular files, relative to pipeline run dir
  for (std::vector<boost::filesystem::path>::iterator iter =
           p.added_files.begin();
       iter != p.added_files.end(); ++iter) {
    check_regular_file(*iter, p.pipeline_run_dir, "added-files");
  }
  // added_directories: should be directories, relative to pipeline run dir
  for (std::vector<boost::filesystem::path>::iterator iter =
           p.added_directories.begin();
       iter != p.added_directories.end(); ++iter) {
    check_and_fix_dir(&(*iter), p.pipeline_run_dir, "added-directories");
  }

  // in theory, if they've made it this far, parameters are ready to go
  return p;
}

boost::filesystem::path snakemake_unit_tests::cargs::override_if_specified(
    const std::string &cli_entry,
    const boost::filesystem::path &params_entry) const {
  return cli_entry.empty() ? params_entry : boost::filesystem::path(cli_entry);
}

void snakemake_unit_tests::cargs::check_regular_file(
    const boost::filesystem::path &p, const boost::filesystem::path &prefix,
    const std::string &msg) const {
  boost::filesystem::path combined;
  // if the prefix is empty
  if (prefix.string().empty()) {
    // the candidate is ready for testing as-is
    combined = p;
  } else {
    // add it to the file candidate before testing
    combined = prefix / p;
  }
  // enforce it being a file
  // TODO(lightning-auriga): add an option for warning instead of exception?
  if (!boost::filesystem::is_regular_file(combined)) {
    throw std::logic_error("for \"" + msg + "\", provided path \"" +
                           combined.string() + "\" is not a regular file");
  }
}

void snakemake_unit_tests::cargs::check_and_fix_dir(
    boost::filesystem::path *p, const boost::filesystem::path &prefix,
    const std::string &msg) const {
  if (!p)
    throw std::runtime_error("null pointer provided to check_and_fix_dir");
  // remove trailing separator if needed
  *p = p->remove_trailing_separator();
  boost::filesystem::path combined;
  // if the prefix is empty
  if (prefix.string().empty()) {
    // the candidate is ready for testing as-is
    combined = *p;
  } else {
    // add it to the file candidate before testing
    combined = prefix / *p;
  }
  // enforce it being a directory
  // TODO(lightning-auriga): add an option for warning instead of exception?
  if (!boost::filesystem::is_directory(combined)) {
    throw std::logic_error("for \"" + msg + "\", provided path \"" +
                           combined.string() + "\" is not a directory");
  }
}

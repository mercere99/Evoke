/////////////////////////////////////////////////////////////////////////////////
//  This is an auto-generated file that defines a set of configuration options.
//  This file is read in mulitple times from config.h, each with different macro
//  definitions to generate correct, effecient code for the command below.
//
//  The available commands are:
//
//  EMP_CONFIG_GROUP(group name, group description string)
//   Start a new group of configuration options.  Group structure is preserved
//   when user-accessible configuration options are generated.
//
//  EMP_CONFIG_VAR(variable name, type, default value, description string)
//   Create a new setting in the cConfig object that can be easily accessed.
//
//  EMP_CONFIG_ALIAS(alias name)
//   Include an alias for the previous setting.  This command is useful to
//   maintain backward compatibility if names change in newer software versions.
//
//  EMP_CONFIG_CONST(variable name, type, fixed value, description string)
//   Create a new configuration constant that cannot be changed.  In practice,
//   allows broader optimizations in the code.

EMP_CONFIG_GROUP(DEFAULT_GROUP, "General Settings")
EMP_CONFIG_VAR(DEBUG_MODE, bool, 0, "Should we output debug information?"
EMP_CONFIG_VAR(RANDOM_SEED, int, 40, "Random number seed (0 for based on time)"

EMP_CONFIG_GROUP(TEST_GROUP, "These are settings with the sole purpose of testing cConfig.
For example, are multi-line descriptions okay?")
EMP_CONFIG_VAR(TEST_BOOL, bool, 0, "This is a bool value.\nAnd what happens\n  ...if we have multiple\n    lines?"
EMP_CONFIG_VAR(TEST_STRING, std::string, "default", "This is a string!"
EMP_CONFIG_CONST(TEST_CONST, int, 91, "This is an unchanging const!"
EMP_CONFIG_VAR(TEST_STRING_SPACE, std::string, "abc def   ghi", "This is a string with spaces."
EMP_CONFIG_VAR(MUTATION_RATE, float, 0.025000, "This is my mutation rate."


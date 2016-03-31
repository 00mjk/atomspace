/*
 * LoggerSCM.cc
 *
 * Copyright (C) 2015 OpenCog Foundation
 *
 * Author: Nil Geisweiller
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <opencog/util/Logger.h>
#include "SchemePrimitive.h"
#include "LoggerSCM.h"

using namespace opencog;

namespace opencog {

/// Set level, return previous level.
const std::string& LoggerSCM::do_logger_set_level(const std::string& level)
{
	static std::string prev_level;
	prev_level = Logger::get_level_string(logger().get_level());
	logger().set_level(Logger::get_level_from_string(level));
	return prev_level;
}

const std::string& LoggerSCM::do_logger_get_level(void)
{
	static std::string level_str;
	level_str = Logger::get_level_string(logger().get_level());
	return level_str;
}

/// Set logfile, return previous file.
const std::string& LoggerSCM::do_logger_set_filename(const std::string& filename)
{
	static std::string old_file;
	old_file = logger().get_filename();
	logger().set_filename(filename);
	return old_file;
}

const std::string& LoggerSCM::do_logger_get_filename()
{
	return logger().get_filename();
}

bool LoggerSCM::do_logger_set_stdout(bool enable)
{
	// bool previous_setting = logger().get_print_to_stdout_flag();
	bool previous_setting = enable;
	logger().set_print_to_stdout_flag(enable);
	return previous_setting;
}

bool LoggerSCM::do_logger_set_sync(bool enable)
{
	// bool previous_setting = logger().get_sync_flag();
	bool previous_setting = enable;
	logger().set_sync_flag(enable);
	return previous_setting;
}

bool LoggerSCM::do_logger_set_timestamp(bool enable)
{
	// bool previous_setting = logger().get_timestamp_flag();
	bool previous_setting = enable;
	logger().set_timestamp_flag(enable);
	return previous_setting;
}

void LoggerSCM::do_logger_error(const std::string& msg)
{
	logger().error(msg);
}

void LoggerSCM::do_logger_warn(const std::string& msg)
{
	logger().warn(msg);
}

void LoggerSCM::do_logger_info(const std::string& msg)
{
	logger().info(msg);
}

void LoggerSCM::do_logger_debug(const std::string& msg)
{
	logger().debug(msg);
}

void LoggerSCM::do_logger_fine(const std::string& msg)
{
	logger().fine(msg);
}

} /*end of namespace opencog*/

LoggerSCM::LoggerSCM() : ModuleWrap("opencog logger") {}

/// This is called while (opencog logger) is the current module.
/// Thus, all the definitions below happen in that module.
void LoggerSCM::init(void)
{
	define_scheme_primitive("cog-logger-set-level!",
		&LoggerSCM::do_logger_set_level, this, "logger");
	define_scheme_primitive("cog-logger-get-level",
		&LoggerSCM::do_logger_get_level, this, "logger");
	define_scheme_primitive("cog-logger-set-filename!",
		&LoggerSCM::do_logger_set_filename, this, "logger");
	define_scheme_primitive("cog-logger-get-filename",
		&LoggerSCM::do_logger_get_filename, this, "logger");
	define_scheme_primitive("cog-logger-set-stdout!",
		&LoggerSCM::do_logger_set_stdout, this, "logger");
	define_scheme_primitive("cog-logger-set-sync!",
		&LoggerSCM::do_logger_set_sync, this, "logger");
	define_scheme_primitive("cog-logger-set-timestamp!",
		&LoggerSCM::do_logger_set_timestamp, this, "logger");
	define_scheme_primitive("cog-logger-error-str",
		&LoggerSCM::do_logger_error, this, "logger");
	define_scheme_primitive("cog-logger-warn-str",
		&LoggerSCM::do_logger_warn, this, "logger");
	define_scheme_primitive("cog-logger-info-str",
		&LoggerSCM::do_logger_info, this, "logger");
	define_scheme_primitive("cog-logger-debug-str",
		&LoggerSCM::do_logger_debug, this, "logger");
	define_scheme_primitive("cog-logger-fine-str",
		&LoggerSCM::do_logger_fine, this, "logger");
}

void opencog_logger_init(void)
{
    static LoggerSCM logger_scm;
    logger_scm.module_init();
}

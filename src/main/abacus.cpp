#include <boost/make_shared.hpp>

#include "atlas/log/log.hpp"
#include "commandline/commandline.hpp"

#include "server.hpp"

int main(int argc, const char **argv)
{
    bool show_help = false;
    abacus::server::options options;
    std::vector<commandline::option> cmd_options{
        commandline::parameter("address", options.address, "Address to listen on"),
        commandline::parameter("port", options.port, "Port to listen on"),
        commandline::parameter("db", options.db_file, "Database file"),
        commandline::parameter("apollo-db", options.apollo_db_file, "Database file"),
        commandline::parameter("chronos-db", options.chronos_db_file, "Database file"),
        commandline::parameter("helios-db", options.helios_db_file, "Database file"),
        commandline::flag("help", show_help, "Show this help message")
    };
    commandline::parse(argc, argv, cmd_options);
    if(show_help)
    {
        commandline::print(argc, argv, cmd_options);
        return 0;
    }

    if(options.apollo_db_file == "")
        options.apollo_db_file = options.db_file;
    if(options.chronos_db_file == "")
        options.chronos_db_file = options.db_file;
    if(options.helios_db_file == "")
        options.helios_db_file = options.db_file;

    if(options.apollo_db_file == "")
    {
        atlas::log::error("main") << "apollo database file must be specified (either by --apollo-db or --db)";
        return 1;
    }
    if(options.chronos_db_file == "")
    {
        atlas::log::error("main") << "chronos database file must be specified (either by --chronos-db or --db)";
        return 1;
    }
    if(options.helios_db_file == "")
    {
        atlas::log::error("main") << "helios database file must be specified (either by --helios-db or --db)";
        return 1;
    }

    try
    {
        boost::shared_ptr<boost::asio::io_service> io =
                boost::make_shared<boost::asio::io_service>();
        abacus::server server(options, io);
        server.start();
        boost::asio::io_service::work work(*io);
        io->run();
        server.stop();
    }
    catch(const std::exception& e)
    {
        atlas::log::error("main") << e.what();
        commandline::print(argc, argv, cmd_options);
        return 1;
    }

    atlas::log::information("main") << "server exiting normally";
    return 0;
}


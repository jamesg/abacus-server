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
        commandline::flag("help", show_help, "Show this help message")
    };
    commandline::parse(argc, argv, cmd_options);
    if(show_help)
    {
        commandline::print(argc, argv, cmd_options);
        return 0;
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


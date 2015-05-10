#include "server.hpp"

#include "hades/connection.hpp"
#include "hades/mkstr.hpp"

#include "atlas/api/auth.hpp"
#include "atlas/http/server/router.hpp"
#include "atlas/http/server/install_static_file.hpp"
#include "atlas/http/server/static_file.hpp"
#include "atlas/http/server/static_files.hpp"
#include "atlas/jsonrpc/uri.hpp"
#include "atlas/log/log.hpp"

#include "apollo/router.hpp"
#include "helios/router.hpp"
//#include "api/api.hpp"
#include "apollo/db.hpp"
#include "helios/db/create.hpp"

abacus::server::server(
        const server::options& options,
        boost::shared_ptr<boost::asio::io_service> io_
        ) :
    m_io(io_),
    m_http_server(
        m_io,
        options.address.c_str(),
        options.port.c_str()
        ),
    m_mime_information(new atlas::http::mimetypes())
{
    if(!options.port.length())
        throw std::runtime_error("port number is required");
    if(!options.apollo_db_file.length())
        throw std::runtime_error("apollo database file is required");

    if(!options.helios_db_file.length())
        throw std::runtime_error("helios database file is required");

    m_apollo_connection.reset(new hades::connection(options.apollo_db_file));
    apollo::db::create(*m_apollo_connection);

    m_helios_connection.reset(new hades::connection(options.helios_db_file));
    helios::db::create(*m_helios_connection);

    boost::shared_ptr<atlas::http::router> apollo_router(new apollo::router(*m_apollo_connection));
    m_http_server.router().install(
        atlas::http::matcher("/apollo(.*)", 1),
        boost::bind(&atlas::http::router::serve, apollo_router, _1, _2, _3, _4)
        );

    boost::shared_ptr<atlas::http::router> helios_router(new helios::router(*m_helios_connection, m_io));
    m_http_server.router().install(
        atlas::http::matcher("/helios(.*)", 1),
        boost::bind(&atlas::http::router::serve, helios_router, _1, _2, _3, _4)
        );

    boost::shared_ptr<atlas::http::router> atlas_router(atlas::http::static_files());
    m_http_server.router().install(
        atlas::http::matcher("/atlas(.*)"),
        boost::bind(&atlas::http::router::serve, atlas_router, _1, _2, _3, _4)
        );

    atlas::log::information("abacus::server::server") << "server listening on port " <<
        options.port;
}

void abacus::server::start()
{
    atlas::log::information("abacus::server::start") << "running server";

    m_http_server.start();
}

void abacus::server::stop()
{
    atlas::log::information("abacus::server::stop") << "stopping server";
    m_http_server.stop();
}

boost::shared_ptr<boost::asio::io_service> abacus::server::io()
{
    return m_io;
}


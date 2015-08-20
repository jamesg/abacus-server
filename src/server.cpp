#include "server.hpp"

#include "hades/connection.hpp"
#include "hades/mkstr.hpp"

#include "atlas/api/auth.hpp"
#include "atlas/http/server/router.hpp"
#include "atlas/http/server/install_static_file.hpp"
#include "atlas/http/server/static_file.hpp"
#include "atlas/http/server/static_files.hpp"
#include "atlas/http/server/static_text.hpp"
#include "atlas/log/log.hpp"

#include "apollo/db.hpp"
#include "apollo/router.hpp"
#include "chronos/db.hpp"
#include "chronos/router.hpp"
#include "demeter/db.hpp"
#include "demeter/router.hpp"
#include "helios/router.hpp"
#include "helios/db/create.hpp"

#define ABACUS_DECLARE_STATIC_STRING(PREFIX) \
    extern "C" { \
        extern char abacus_binary_##PREFIX##_start; \
        extern char abacus_binary_##PREFIX##_end; \
        extern size_t abacus_binary_##PREFIX##_size; \
    }

#define ABACUS_STATIC_STD_STRING(PREFIX) \
    std::string(&abacus_binary_##PREFIX##_start, &abacus_binary_##PREFIX##_end)

ABACUS_DECLARE_STATIC_STRING(index_html)

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
    if(!options.db_file.length())
        throw std::runtime_error("database file is required");

    m_connection.reset(new hades::connection(options.db_file));
    apollo::db::create(*m_connection);
    chronos::db::create(*m_connection);
    demeter::db::create(*m_connection);
    helios::db::create(*m_connection);

    m_http_server.router().install(
            atlas::http::matcher("/", "GET"),
            boost::bind(
                &atlas::http::static_text,
                m_mime_information->content_type("html"),
                ABACUS_STATIC_STD_STRING(index_html),
                _1,
                _2,
                _3,
                _4
                )
            );

    m_http_server.router().install(
            atlas::http::matcher("/index.html", "GET"),
            boost::bind(
                &atlas::http::static_text,
                m_mime_information->content_type("html"),
                ABACUS_STATIC_STD_STRING(index_html),
                _1,
                _2,
                _3,
                _4
                )
            );


    boost::shared_ptr<atlas::http::router>
        apollo_router(new apollo::router(m_io, *m_connection));
    m_http_server.router().install(
        atlas::http::matcher("/apollo(.*)", 1),
        boost::bind(&atlas::http::router::serve, apollo_router, _1, _2, _3, _4)
        );

    boost::shared_ptr<atlas::http::router>
        chronos_router(new chronos::router(m_io, *m_connection));
    m_http_server.router().install(
        atlas::http::matcher("/chronos(.*)", 1),
        boost::bind(&atlas::http::router::serve, chronos_router, _1, _2, _3, _4)
        );

        boost::shared_ptr<atlas::http::router>
            helios_router(new helios::router(m_io, *m_connection));
        m_http_server.router().install(
            atlas::http::matcher("/helios(.*)", 1),
            boost::bind(&atlas::http::router::serve, helios_router, _1, _2, _3, _4)
            );

        boost::shared_ptr<atlas::http::router>
            demeter_router(new demeter::router(m_io, *m_connection));
        m_http_server.router().install(
            atlas::http::matcher("/demeter(.*)", 1),
            boost::bind(&atlas::http::router::serve, demeter_router, _1, _2, _3, _4)
            );

    boost::shared_ptr<atlas::http::router> atlas_router(atlas::http::static_files(m_io));
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

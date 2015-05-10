#ifndef ABACUS_SERVER_HPP
#define ABACUS_SERVER_HPP

#include <boost/asio.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "atlas/api/server.hpp"
#include "atlas/http/server/mimetypes.hpp"
#include "atlas/http/server/server.hpp"

namespace hades
{
    class connection;
}
namespace atlas
{
    namespace http
    {
        class mimetypes;
    }
}
namespace abacus
{
    class server
    {
    public:
        struct options
        {
            std::string address;
            std::string apollo_db_file;
            std::string helios_db_file;
            std::string port;
            options() :
                address("0.0.0.0")
            {
            }
        };

        server(const options&, boost::shared_ptr<boost::asio::io_service>);

        void start();
        void stop();

        boost::shared_ptr<boost::asio::io_service> io();
    private:
        boost::shared_ptr<boost::asio::io_service> m_io;

        boost::scoped_ptr<hades::connection> m_apollo_connection;
        boost::scoped_ptr<hades::connection> m_helios_connection;
        atlas::http::server m_http_server;
        boost::scoped_ptr<atlas::http::mimetypes> m_mime_information;
    };
}

#endif


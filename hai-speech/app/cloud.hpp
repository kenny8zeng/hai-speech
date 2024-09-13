#ifndef cloud_jksdfiojewioffodsifjweo7878
#define cloud_jksdfiojewioffodsifjweo7878

#include <atomic>
#include <functional>

#include <hude/base/log.hpp>
#include <hude/aio/context.hpp>
#include <hude/aio/http.hpp>

#include "configs/base.hpp"

namespace hai {
    /*********************************************************************
    * @brief ...
    */
    class websock_t
    {
    public:
        /*********************************************************************
        * @brief ...
        */
        enum { emERR_CONNECT = -10000 };

        /*********************************************************************
        * @brief ...
        */
        using http_t = hude::http::http_t<>;

    public:
        /*********************************************************************
        * @brief ...
        */
        websock_t( const configs_t& cfg ) :
            configs( cfg ),
            aio( http_t::aio() )
        {}

        /*********************************************************************
        * @brief ...
        */
        int service();

        /*********************************************************************
        * @brief ...
        */
        void dialog( std::string&, std::function< void( int, std::string* ) > );

        /*********************************************************************
        * @brief ...
        */
        void stop();

    private:
        const configs_t&                                 configs;
        http_t::aio_t                                    aio;
        http_t::http_listen_t                            listener;
        http_t::http_connect_t< http_t::evts_cli_wsk_t > connector_wsk;

    private:
        static const char* _template_request;

    };
} // namespace hai

#endif // cloud_jksdfiojewioffodsifjweo7878


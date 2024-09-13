/*********************************************************************
 * @author kenny
 * @details 请保持文件以UTF-8（无BOM）编码保存
 */

#include <csignal>
#include <cstdio>
#include <execinfo.h>
#include <iostream>
#include <memory>

#include <hude/base/log.hpp>
#include <hude/utils/clipp.hpp>
#include <hude/utils/json.hpp>
#include <thread>

#include "cloud.hpp"
#include "configs/base.hpp"
#include "speech/speech.hpp"

/*********************************************************************
* @brief ...
*/
namespace hai {
} // namespace hai

/*********************************************************************
 * @brief init log
 */
hude::logger_default_t _logger;

/*********************************************************************
 * @brief SIGINT handler
 */
void sigint_handler( int signum )
{
    if( signum == SIGINT ) { hai::speech_t::running( false ); }
}

/*********************************************************************
 * @brief crash handler
 */
void crash_report( int signum )
{
    constexpr int len = 1024;
    void *func[len];
    char **funs;

    signal(signum, SIG_DFL);

    auto size = backtrace(func, len);
    funs = (char **)backtrace_symbols(func, size);

    fprintf(stderr, "System error, Stack trace:\n");

    for(int i = 0; i < size; ++i)
    {
        fprintf(stderr, "#%d \t %s \n", i, funs[i]);
    }

    free(funs);
}

/*********************************************************************
 * @brief service
 */
int service( const hai::configs_t& cfg )
{
    // websocket client
    auto ws = hai::websock_t( cfg );

    // show result in cloud reply
    auto show_result = []( std::string& info ) {
        auto h = info.begin();
        auto b = info.begin();
        hude::utils::json_checker_t< std::string::iterator >::check( h, b, info.end() );

        auto result = hude::utils::json_t::parse( std::string(h, b) );
        auto answer = hude::utils::json_path_t::value< std::string >( result, "payload", "data", "token", "result" );

        llog_nte("<app>", "AI: %s", answer? answer->c_str() : "error." );
    };

    // Streaming events returned by the server
    auto on_stream = [=]( int err, std::string* info ) {
        if( err ) {
            llog_err("<app>", "cloud request error: %d", err );
        } else {
            llog_dug("<app>", "cloud relpy stream: %s", info->c_str() );
            show_result( *info );
        }
    };

    // Wake-Up event
    auto on_wakeup = []( std::string* info ) {
        llog_inf("<app>", "wakeup event, info: %s", info->c_str() );
    };

    // voice recognition event
    auto on_recognition = [=, &ws]( std::string* text ) {
        llog_nte("<app>", "USER: %s", text? text->c_str() : "nullptr" );

        if( text == nullptr ) return;

        llog_dug("<app>", "to sleep event, llm service request: %s", text->c_str() );

        ws.dialog( *text, on_stream );
    };

    // Background processing is a worker thread that initiates requests to the cloud for processing.
    auto thc = std::thread( [&ws] { ws.service(); } );

    // Blocking Processing Speech Recognition
    llog_war( "<app>", "begin..." );
    auto ret = hai::speech_t( cfg ).run( on_wakeup, on_recognition );
    llog_war( "<app>", "end." );

    // clean and exit
    ws.stop();
    thc.join();

    return ret;
}

/*********************************************************************
 * @brief main
 */
int main( int argc, char* argv[] )
{
    // SIGINT signal exit.
    signal(SIGINT, sigint_handler);
    // Invaild memory address
    signal( SIGSEGV, crash_report );
    // Abort signal
    signal( SIGABRT, crash_report );

    // default configs
    auto cfg = std::unique_ptr< hai::configs_t >( new hai::configs_t {
        .card = 0,
        .device = 0,
        .time = 0,
        .debug = 0,
        .port = 80,
        .alsa = {
            .channels = 1,
            .rate = 16000,
            .period_size = 1024,
            .period_count = 2,
            .format = PCM_FORMAT_S16_LE,
            .start_threshold = 0,
            .stop_threshold = 0,
            .silence_threshold = 0,
        },
        .host = "dev.ircopilot.com",
        .resdir = "/root/hai",
        .workdir = "/tmp/hai",
    });

    int format = 16;

    // parse format of command line
    auto cli = (
        clipp::option("-c", "--channels") & clipp::value("alsa channels", cfg->alsa.channels ),
        clipp::option("-r", "--rate") & clipp::value("alsa rate", cfg->alsa.rate ),
        clipp::option("-s", "--period_size") & clipp::value("alsa period_size", cfg->alsa.period_size ),
        clipp::option("-n", "--period_count") & clipp::value("alsa period_count", cfg->alsa.period_count ),
        clipp::option("-f", "--format") & clipp::value("alsa format: 16|32", format ),

        clipp::option("-C", "--card") & clipp::value("sound card number", cfg->card ),
        clipp::option("-D", "--device") & clipp::value("sound card device number", cfg->device ),
        clipp::option("-H", "--host") & clipp::value("service host", cfg->host ),
        clipp::option("-P", "--port") & clipp::value("service port", cfg->port ),
        clipp::option("-R", "--resdir") & clipp::value("resource path", cfg->resdir ),
        clipp::option("-W", "--workdir") & clipp::value("work path", cfg->workdir ),
        clipp::option("-S", "--session") & clipp::value("session id", cfg->session ),
        clipp::option("-L", "--local") & clipp::value("in which room?", cfg->local ),
        clipp::option("-T", "--time") & clipp::value("work time, n seconds", cfg->time ),
        clipp::option("-V", "--debug") & clipp::value("debug infometion details, 1 or 2", cfg->debug )
    );

    if( !clipp::parse( argc, argv, cli ) )
    {
        std::cout << clipp::make_man_page(cli, argv[0]);
        return -0xffffff;
    }

    // convert args
    {
        srand( time( nullptr ) );

        cfg->alsa.format = (format == 16)? PCM_FORMAT_S16_LE : PCM_FORMAT_S32_LE;

        if( cfg->session.empty() ) cfg->session = hude::format_t( "rid-%010u", uint32_t( rand() ) ).str();
    }

    // set log level
    {
        if( cfg->debug > 2 ) cfg->debug = 2;
        if( cfg->debug < 0 ) cfg->debug = 0;
        hude::log_debug_info_t::level( hude::log_level_t( hude::log_level_t::emINFO + cfg->debug ) );
    }

    return service( *cfg );
}

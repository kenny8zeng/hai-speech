
#ifndef recorder_jksdfiojewioffodsifjweo7878
#define recorder_jksdfiojewioffodsifjweo7878

#include <cstddef>
#include <stdio.h>
#include <vector>
#include <functional>
#include <atomic>

#include <hude/base/log.hpp>
#include <tinyalsa/pcm.h>

#include "configs/base.hpp"

namespace hai {
    /*********************************************************************
    * @brief Recording module implemented based on tinyalsa
    */
    class recorder_t
    {
    public:
        /*********************************************************************
        * @brief The type of data frame event received
        */
        using on_frames_t = std::function< int( buffer_t&, std::size_t, std::size_t ) >;

        /*********************************************************************
        * @brief Type of error filter callback function
        */
        using cb_filter_t = std::function< bool( int ) >;

    public:
        /*********************************************************************
        * @brief recording status, set false if exit.
        */
        static void running( bool v ) { auto& r = running_(); r = v; }

        /*********************************************************************
        * @brief Check recording status
        */
        static bool running() { auto& r = running_(); return r; }

    public:
        /*********************************************************************
        * @brief start recording thread.
        */
        int run( const configs_t&, on_frames_t, cb_filter_t = cb_filter_t() );

    private:
        /*********************************************************************
        * @brief Keep recording until running is set to exit status.
        */
        int _record( const configs_t&, on_frames_t );

        /*********************************************************************
        * @brief private state
        */
        static std::atomic_bool& running_()
        {
            static std::atomic_bool r = false;
            return r;
        }

    };

    /*********************************************************************
    * @brief ...
    */
    inline int recorder_t::run( const configs_t& configs, on_frames_t on_frames, cb_filter_t filter )
    {
        int ret = 0;

        if( !filter )
        {
            filter = []( int ret ) -> bool { return ret == 0; };
        }

        llog_dug( "<recorder>", "begin..." );

        running( true );

        auto _th = [=]( int& ret )
        {
            while( running() )
            {
                ret = _record( configs, on_frames );
                if( ! filter( ret ) ) break;
            }

            running( false );
        };

        auto th = std::thread( _th, std::ref( ret ) );

        // exit when wait 5s
        if( configs.time )
        {
            for( int i = 0; i < configs.time; ++i )
            {
                if( !running() ) break;
                std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
            }

            running( false );
        }

        th.join();
        llog_dug( "<recorder>", "end." );

        return ret;
    }

    /*********************************************************************
    * @brief ...
    */
    inline int recorder_t::_record( const configs_t& configs, on_frames_t on_frames )
    {
        static const unsigned int flags = PCM_IN;

        int ret = 0;

        auto r = [](struct pcm* p) { pcm_close( p ); };
        auto pcm = std::unique_ptr< struct pcm, decltype(r) >( 
            pcm_open(configs.card, configs.device, flags, &(configs.alsa)), r );

        if ( !pcm ) {
            llog_err("<app>", "failed to allocate memory for PCM, errno(%s)", strerror(errno));
            return -11001;
        }
        
        if ( !pcm_is_ready(pcm.get()) )
        {
            llog_err("<app>", "failed to open PCM, errno(%s)", strerror(errno));
            return -11002;
        }

        const unsigned int frame_size = pcm_frames_to_bytes(pcm.get(), 1);
        const unsigned int frames_per_sec = pcm_get_rate(pcm.get()) / 5;
        const unsigned int buffer_size = frame_size * frames_per_sec;

        auto buffer = buffer_t( buffer_size );

        while( running() )
        {
            if( buffer.size() != buffer_size ) buffer.resize( buffer_size );

            ret = pcm_readi(pcm.get(), &(buffer[0]), frames_per_sec);

            if( ret == frames_per_sec )
            {
                ret = on_frames( buffer, frame_size, frames_per_sec );
                if( ret ) return ret;
            } else {
                llog_err( "<app>", "read PCM size: %d", ret );
                return -11004;
            }
        }

        return 0;
    }

} // namespace hai

#endif // recorder_jksdfiojewioffodsifjweo7878
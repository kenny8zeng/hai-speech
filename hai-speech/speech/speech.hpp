#ifndef speech_jksdfiojewioffodsifjweo7878
#define speech_jksdfiojewioffodsifjweo7878

#include <cstddef>
#include <set>

#include <functional>
#include <hude/base/log.hpp>

#include "recorder.hpp"
#include "xfyun.hpp"

namespace hai {
    /*********************************************************************
    * @brief speech object
    */
    class speech_t
    {
    private:
        /*********************************************************************
        * @brief Define all cycle steps
        */
        enum status_t { emIDLE, emWATCH, emWAKEUP, emTRANSFORM };

        /*********************************************************************
        * @brief The event of voice wake-up
        */
        using on_wakeup_t = std::function< void( std::string* ) >;

        /*********************************************************************
        * @brief The event of voice recognition
        */
        using on_recognition_t = std::function< void( std::string* ) >;

    public:
        /*********************************************************************
        * @brief set the running state
        */
        static void running( bool v ) { recorder_t::running( v ); }

        /*********************************************************************
        * @brief get the running state
        */
        static bool running() { return recorder_t::running(); }

    public:
        /*********************************************************************
        * @brief ...
        */
        speech_t( const configs_t& cfg ) : configs_( cfg ) {}

        /*********************************************************************
        * @brief start work
        */
        int run( on_wakeup_t, on_recognition_t );

        /*********************************************************************
        * @brief ...
        */
        // const configs_t& configs() const { return configs_; }

    private:
        /*********************************************************************
        * @brief Data frame event handling
        */
        int on_frames( buffer_t&, size_t, size_t );

        /*********************************************************************
        * @brief get the current step
        */
        status_t status() const { return status_; }

        /*********************************************************************
        * @brief set the current step
        */
        void status( status_t s )
        {
            status_ = s;
            static const char* ts[] = { "emIDLE", "emWATCH", "emWAKEUP", "emTRANSFORM" };
            llog_dug( "<speech>", "change step: %s", ts[s] );
        }

    private:
        const configs_t&                       configs_;   // all configs
        status_t                               status_;    // current step
        recorder_t                             recorder_;  // Encapsulating recording module based on tinyalsa
        voice_t< voice_type_t::emWAKEUP >      wakeup_;    // Encapsulate voice wake-up module based on ifly
        voice_t< voice_type_t::emRECOGNITION > dictat_;    // Encapsulate voice dictation module based on ifly
    };

    /*********************************************************************
    * @brief ...
    */
    namespace pl = std::placeholders;

    /*********************************************************************
    * @brief ...
    */
    inline int speech_t::on_frames( buffer_t & buffer, size_t frame_size, size_t frame_count )
    {
        llog_tra( "<speech>", "  on_frames( buffer, %u, %u )", frame_size, frame_count );

        int ret = 0;

        const auto& f = buffer;

        switch ( status() ) {
        case emIDLE: // idle state
            dictat_.down();
            ret = wakeup_.up( configs_ );
            if( ret ) return ret;

            ret = wakeup_.write( &f, MSP_AUDIO_SAMPLE_FIRST );
            if( ret ) return ret;

            status( emWATCH );
            break;

        case emWATCH: // Monitor the status of wake-up voice
            ret = wakeup_.write( &f, MSP_AUDIO_SAMPLE_CONTINUE );
            break;

        case emWAKEUP: // Already awakened state
            wakeup_.down();
            ret = dictat_.up( configs_ );
            if( ret ) return ret;

            ret = dictat_.write( &f, MSP_AUDIO_SAMPLE_FIRST );
            if( ret ) return ret;

            status( emTRANSFORM );
            break;

        case emTRANSFORM: // Status of Speech Content Recognition
            ret = dictat_.write( &f, MSP_AUDIO_SAMPLE_CONTINUE );
            if( ret == voice_lib_t::emIAT_END ) { status( emIDLE ); ret = 0; }
            break;

        default: break;
        }

        return 0;
    }

    /*********************************************************************
    * @brief ...
    */
    inline int speech_t::run( on_wakeup_t on_wakeup, on_recognition_t on_recognition )
    {
        int ret = 0;

        // Set the status to be updated when a wakeup event is triggered
        wakeup_.set_event_finish( [=]( std::string* info ) {
            if( info ) { on_wakeup( info ); }

            status( emWAKEUP );
        });

        // Set up status updates when a speech recognition event is triggered
        dictat_.set_event_finish( [=]( std::string* text ) {
            if( text ) { on_recognition( text ); }

            status( emIDLE );
        });

        // Set initial state
        status( emIDLE );

        // SDK login
        ret = voice_lib_t::login( configs_ );
        if( ret ) return ret;

        // Ignore this error codes and let the task continue running.
        auto filter = []( int ret ) -> bool {
            static const std::set< int > codes{ 0, 10008, 10019 };
            return codes.find( ret ) != codes.cend();
        };

        // Start the recording thread. If there are data frames, the on_frames event will be triggered.
        ret = recorder_.run( configs_, std::bind( &speech_t::on_frames, this, pl::_1, pl::_2, pl::_3 ), filter );
        if( status() == emTRANSFORM ) ret = dictat_.write( nullptr, MSP_AUDIO_SAMPLE_LAST );

        // close speech wakeup and recognition
        dictat_.down();
        wakeup_.down();

        // SDK logout
        voice_lib_t::logout();

        return ret;
    }
}
#endif //speech_jksdfiojewioffodsifjweo7878

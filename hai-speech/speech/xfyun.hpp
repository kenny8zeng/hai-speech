#ifndef voice_jksdfiojewioffodsifjweo7878
#define voice_jksdfiojewioffodsifjweo7878

#include <cstring>

#include <functional>
#include <hude/base/log.hpp>

#include "configs/base.hpp"

#include "msp_types.h"
#include "msp_cmn.h"
#include "msp_errors.h"
#include "qisr.h"
#include "qivw.h"

namespace hai {
    /*********************************************************************
    * @brief voice library
    */
    struct voice_lib_t
    {
        /*********************************************************************
        * @brief ...
        */
        enum { emHINTS = 128, emIAT_END = 0x0f0011 };

        /*********************************************************************
        * @brief SDK login
        */
        static int login( const configs_t& cfg );

        /*********************************************************************
        * @brief SDK logout
        */
        static void logout();

        /*********************************************************************
        * @brief SDK upload userwords
        */
        static int userwords();
    };

    /*********************************************************************
    * @brief Voice task interface
    */
    template< typename _SUB >
    class voice_base_t
    {
    public:
        /*********************************************************************
        * @brief ...
        */
        using sub_t = _SUB;

        /*********************************************************************
        * @brief ...
        */
        using lib_t = voice_lib_t;

        /*********************************************************************
        * @brief The type of voice task completion event
        */
        using on_finish_t = std::function< void( std::string* ) >;

    public:
        /*********************************************************************
        * @brief ...
        */
        voice_base_t( on_finish_t cb ) :
            session_id( nullptr ),
            on_finish( cb )
        {
            memset( hints, 0, lib_t::emHINTS );
        }

        /*********************************************************************
        * @brief task begin
        */
        int up( const configs_t& cfg ) { return ((sub_t*)this)->up( cfg ); }

        /*********************************************************************
        * @brief task end
        */
        void down() { ((sub_t*)this)->down(); }

        /*********************************************************************
        * @brief write voice frames to task
        */
        int write( const buffer_t* frames, int stat ) { return ((sub_t*)this)->write( frames, stat ); }

        /*********************************************************************
        * @brief set finish event handler
        */
        void set_event_finish( on_finish_t cb ) { on_finish = cb; }

    protected:
        const char* session_id;
        char        hints[ lib_t::emHINTS ];
        on_finish_t on_finish;
    };

    /*********************************************************************
    * @brief The type of voice wake and speech recognition.
    */
    enum voice_type_t { emWAKEUP, emRECOGNITION };

    /*********************************************************************
    * @brief ...
    */
    template < voice_type_t _TYPE > class voice_t {};

    /*********************************************************************
    * @brief Voice wake-up task module
    */
    template <> class voice_t< emWAKEUP > : public voice_base_t< voice_t< emWAKEUP > >
    {
    public:
        enum { emTYPE = emWAKEUP };

        using this_t = voice_t< emWAKEUP >;

    public:
        /*********************************************************************
        * @brief ...
        */
        voice_t( on_finish_t cb = on_finish_t() ) : voice_base_t< this_t >( cb ) {}

        /*********************************************************************
        * @brief ...
        */
        ~voice_t() { this->down(); }

        /*********************************************************************
        * @brief task begin
        */
        int up( const configs_t& cfg );

        /*********************************************************************
        * @brief task end
        */
        void down();

        /*********************************************************************
        * @brief write voice frames to task
        */
        int write( const buffer_t* frames, int stat );
    };

    /*********************************************************************
    * @brief Speech recognition task module
    */
    template <> class voice_t< emRECOGNITION > : public voice_base_t< voice_t< emRECOGNITION > >
    {
    public:
        enum { emTYPE = emRECOGNITION };

        using this_t = voice_t< emRECOGNITION >;

    private:
        std::list< std::string > results;

    private:
        /*********************************************************************
        * @brief ...
        */
        void on_result( const char* ret )
        {
            results.emplace_back( ret );
        }

    public:
        /*********************************************************************
        * @brief ...
        */
        voice_t( on_finish_t cb = on_finish_t() ) : voice_base_t< this_t >( cb ) {}

        /*********************************************************************
        * @brief ...
        */
        ~voice_t() { this->down(); }

        /*********************************************************************
        * @brief task begin
        */
        int up( const configs_t& cfg );

        /*********************************************************************
        * @brief task end
        */
        void down();

        /*********************************************************************
        * @brief write voice frames to task
        */
        int write( const buffer_t* frames, int stat );

    };

    /*********************************************************************
    * @brief ...
    */
    inline int voice_t< emWAKEUP >::up( const configs_t& cfg )
    {
        if( session_id ) return 0;

        int ret = MSP_SUCCESS;

        static const char* ssb_param =
            "ivw_threshold=0:1450,sst=wakeup,"
            "ivw_res_path =fo|%s/wakeupresource.jet";

        session_id = QIVWSessionBegin(nullptr,
            hude::_format_t< 512 >( ssb_param, cfg.resdir.c_str() )->c_str(), &ret);
        if (ret != MSP_SUCCESS)
        {
            llog_err("<wakeup>", "QIVWSessionBegin failed! error code:%d",ret);
            snprintf(hints, lib_t::emHINTS, "QIVWSessionBegin error code:%d", ret);
            return -1;
        }

        auto cb = []( const char *sessionID, int msg, int param1, int param2,
            const void *info, void *userData ) -> int
        {
            auto thiz = (this_t*)userData;

            if (MSP_IVW_MSG_ERROR == msg) //唤醒出错消息
            {
                llog_war("<wakeup>", "WAKEUP ERROR: %d", param1);
            }
            else if (MSP_IVW_MSG_WAKEUP == msg) //唤醒成功消息
            {
                llog_dug("<wakeup>", "WAKEUP: %s", (const char*)info);

                std::string s( (const char*)info );
                thiz->on_finish( &s );
            }

            return 0;
        };

        ret = QIVWRegisterNotify(session_id, cb, (void*)this);
        if (ret != MSP_SUCCESS)
        {
            llog_err("<wakeup>", "QIVWRegisterNotify failed! error code:%d",ret);
            snprintf(hints, lib_t::emHINTS, "QIVWRegisterNotify errorCode=%d", ret);
            return -2;
        }

        snprintf(hints, lib_t::emHINTS, "normal");

        return 0;
    }

    /*********************************************************************
    * @brief ...
    */
    inline int voice_t< emWAKEUP >::write( const buffer_t* frames, int stat )
    {
        llog_tra( "<wakeup>", "    QIVWAudioWrite stat: %d", stat);

        int ret = QIVWAudioWrite( session_id, &((*frames)[0]), frames->size(), stat );
        if ( MSP_SUCCESS != ret )
        {
            llog_err( "<wakeup>", "      QIVWAudioWrite failed! error code:%d", ret);
            snprintf( hints, lib_t::emHINTS, "QIVWAudioWrite errorCode=%d", ret);
            return ret;
        }

        return 0;
    }

    /*********************************************************************
    * @brief ...
    */
    inline void voice_t< emWAKEUP >::down()
    {
        if (nullptr != session_id)
        {
            QIVWSessionEnd( session_id, hints );
            session_id = nullptr;
        }
    }

    /*********************************************************************
    * @brief ...
    */
    inline int voice_t< emRECOGNITION >::up( const configs_t& cfg )
    {
        if( session_id ) return 0;

        int ret = MSP_SUCCESS;

        static const char* session_begin_params =
            "sub = iat, domain = iat, language = zh_cn, "
            "accent = mandarin, sample_rate = %u, "
            "result_type = plain, result_encoding = utf8";

        session_id = QISRSessionBegin(nullptr, hude::_format_t< 512 >( session_begin_params, cfg.alsa.rate ).c_str(), &ret); //听写不需要语法，第一个参数为nullptr
        if (MSP_SUCCESS != ret)
        {
            llog_err("<recognition>", "QISRSessionBegin failed! error code:%d", ret);
            snprintf(hints, lib_t::emHINTS, "QISRSessionBegin error code:%d", ret);
            return ret;
        }

        snprintf(hints, lib_t::emHINTS, "normal");

        results.clear();

        return 0;
    }

    /*********************************************************************
    * @brief ...
    */
    inline void voice_t< emRECOGNITION >::down()
    {
        if (nullptr != session_id)
        {
            QISRSessionEnd( session_id, hints );
            session_id = nullptr;
        }
    }

    /*********************************************************************
    * @brief ...
    */
    inline int voice_t< emRECOGNITION >::write( const buffer_t* frames, int stat )
    {
        int ep_stat  = MSP_EP_LOOKING_FOR_SPEECH;  // 端点检测
        int rec_stat = MSP_REC_STATUS_SUCCESS;     // 识别状态
        int ret      = 0;

        llog_tra( "<recognition>", "    QISRAudioWrite  stat: %d", frames? int(4) : stat );

        if( frames == nullptr )
        {
            ret = QISRAudioWrite(session_id, nullptr, 0,
                MSP_AUDIO_SAMPLE_LAST, &ep_stat, &rec_stat);
        } else {
            ret = QISRAudioWrite(session_id, &((*frames)[0]), frames->size(),
                stat, &ep_stat, &rec_stat);
        }

        llog_tra( "<recognition>", "      QISRAudioWrite rets: %d, %d, %d", ret, ep_stat, rec_stat );

        if (MSP_SUCCESS != ret)
        {
            llog_err("<recognition>", "      QISRAudioWrite failed! error code:%d", ret);
            return ret;	
        }

        if( frames == nullptr )
        {
            while( MSP_REC_STATUS_COMPLETE != rec_stat )
            {
                const char *rslt = QISRGetResult(session_id, &rec_stat, 0, &ret);
                if (MSP_SUCCESS != ret)
                {
                    llog_err( "<recognition>", "    QISRGetResult failed! error code: %d", ret );
                    return ret;
                } else {
                    llog_dug( "<recognition>", "    QISRGetResult rec_stat: %d", rec_stat );
                }

                if (nullptr != rslt)
                {
                    llog_dug( "<recognition>", "      result: %s", rslt );
                    on_result( (const char*)rslt );
                } else {
                    llog_dug( "<recognition>", "      result: nullptr" );
                }
            }
        } else {
            if (MSP_REC_STATUS_SUCCESS == rec_stat) //已经有部分听写结果
            {
                const char *rslt = QISRGetResult(session_id, &rec_stat, 0, &ret);
                if (MSP_SUCCESS != ret)
                {
                    llog_err( "<recognition>", "    QISRGetResult failed! error code: %d", ret );
                    return ret;
                } else {
                    llog_dug( "<recognition>", "    QISRGetResult rec_stat: %d", rec_stat );
                }

                if (nullptr != rslt)
                {
                    llog_dug( "<recognition>", "      result: %s", rslt );
                    on_result( (const char*)rslt );
                } else {
                    llog_dug( "<recognition>", "      result: nullptr" );
                }
            }
        }

        if (MSP_EP_AFTER_SPEECH == ep_stat)
        {
            llog_dug( "<recognition>", "      QISRAudioWrite stop: MSP_EP_AFTER_SPEECH" );

            if( results.empty() )
            {
                this->on_finish( nullptr );
            } else {
                std::string txt;

                for( const auto& s : results )
                {
                    txt += s;
                }

                this->on_finish( txt.empty()? nullptr : &txt );
            }

            return lib_t::emIAT_END;
        }

        return 0;
    }

    /*********************************************************************
    * @brief ...
    */
    inline int voice_lib_t::login( const configs_t& cfg )
    {
        int ret = 0;
        
        /* Login first. the 1st arg is username, the 2nd arg is password
        * just set them as nullptr. the 3rd arg is login paramertes 
        * */
        static const char* login_params = "appid = f26b7fcc, work_dir = %s";
        ret = MSPLogin(nullptr, nullptr, hude::_format_t< 512 >( login_params, cfg.workdir.c_str() )->c_str() );
        if (MSP_SUCCESS != ret)	{
            llog_err("<app>", "MSPLogin failed , Error code %d.", ret);
            return -1;	
        }

        // ret = voice_lib_t::userwords();

        return ret ;
    }

    /*********************************************************************
    * @brief ...
    */
    inline void voice_lib_t::logout() { MSPLogout(); }

    /*********************************************************************
    * @brief ...
    */
    inline int voice_lib_t::userwords()
    {
        static const char* userwords =
            "{\"userword\":["
                "{\"words\":[\"小牛\",\"小牛小牛\"],\"name\":\"唤醒词\"},"
                "{\"words\":[\"房间\",\"客厅\",\"卧室\",\"厨房\",\"卫生间\",\"厕所\"],\"name\":\"位置\"},"
                "{\"words\":[\"调光灯\",\"照明灯\",\"插座\",\"风扇\",\"空调\",\"油烟机\",\"电视\"],\"name\":\"设备\"}"
            "]}";

        int ret = 0;
        MSPUploadData("userwords", (void*)userwords, strlen(userwords), "sub = uup, dtt = userword", &ret);
        if (MSP_SUCCESS != ret)
        {
            llog_err("<app>", "MSPUploadData failed ! errorCode: %d", ret);
            return -2;	
        }

        llog_dug("<app>", "Uploaded successfully");

        return ret;
    }
} // namespace hai

#endif // voice_jksdfiojewioffodsifjweo7878


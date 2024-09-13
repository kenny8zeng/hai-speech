#include <cstdint>
#include <cstdlib>
#include <limits>
#include <string>
#include <utility>

#include <hude/utils/json.hpp>
#include "cloud.hpp"

namespace hai {
    /*********************************************************************
    * @brief ...
    */
    int websock_t::service()
    {
        int result = 0;

        // 运行网络上下文的事件泵
        auto ret = aio->run( []{ llog_dug( "<websock>", "aio event loop start..." ); }, 500U, true );

        // 退出状态提示
        if( ret < 0 )
        {
            llog_err( "<websock>", "aio error( %d ): %s", ret, aio->error() );

            return ret;
        } else {
            llog_dug( "<websock>", "aio event loop end." );
        }

        return result;
    }

    /*********************************************************************
    * @brief ...
    */
    void websock_t::stop()
    {
        aio->cancel();
    }

    /*********************************************************************
    * @brief ...
    */
    void websock_t::dialog( std::string& question, std::function< void( int, std::string* ) > on_stream )
    {
        std::string text( question.size(), '\0' );
        std::swap( text, question );

        auto cbs_cli_wsk = [=]( std::string text )
        {
            int err = -1;

            http_t::evts_cli_wsk_t ecw
            {
                // on handler close event
                [=, &err]( http_t::user_t& )
                {
                    llog_tra( "<websock>", "websocket client on_close()" );
                    if( err ) on_stream( err, nullptr );
                },

                // on handler connected event
                [=]( const http_t::address_t& target, http_t::user_t& user, http_t::sender_wsk_t sender )
                {
                    llog_tra( "<websock>", "websocket client on_session( %s )", target.str().c_str() );

                    auto r = uint32_t( rand() );

                    r = std::numeric_limits< uint32_t >::max() - r;

                    auto _request = hude::format_t( websock_t::_template_request, r, configs.session.c_str(),
                        text.c_str(), (configs.local.empty())? "客厅" : configs.local.c_str() );

                    // auto frame = hude::utils::json_t::parse( _request.str() ).dump();
                    // llog_tra( "<websock>", "Send request: %s", frame.c_str() );

                    sender->send( _request.str() );
                },

                // on handler receive data event
                [=, &err]( http_t::user_t& user, http_t::sender_wsk_t sender,
                    const http_t::head_t& head, http_t::buffer_t& mesg )
                {
                    std::string message{ mesg.begin(), mesg.end() };

                    llog_tra( "<websock>", "websocket on_recv( %u ): %s", mesg.size(), message.c_str() );

                    err = 0;

                    if( on_stream ) on_stream( err, &message );
                }
            };

            connector_wsk( aio, configs.host.c_str(), configs.port, ecw, "/llm/stream" );

            auto ret = connector_wsk.state();

            if( !ret || ! *ret )
            {
                llog_err( "<websock>", "websocket connect error: %s", connector_wsk.error() );
                on_stream( emERR_CONNECT, nullptr );
            }
        };

        aio->task_commit( std::bind( cbs_cli_wsk, std::move( text ) ), 800U );
    }

    /*********************************************************************
    * @brief ...
    */
    const char* websock_t::_template_request = R"(
        {
          "topic": "llm/smarthome",
          "rid": "rid-%010u",
          "payload": {
            "page_id": "%s",
            "question": "%s",
            "local": "%s",
            "instruct": false,
            "home": {
              "model": {
                "001": {
                  "name": "switch",
                  "describe": "开关设备，可开关电源。",
                  "property": {
                    "power": {
                      "type": "bool",
                      "range": [
                        true,
                        false
                      ]
                    }
                  }
                },
                "002": {
                  "name": "LED-lamp",
                  "describe": "LED调光灯，用于照明，可开关，可调节亮度。",
                  "property": {
                    "power": {
                      "type": "bool",
                      "range": [
                        true,
                        false
                      ]
                    },
                    "level": {
                      "type": "uint",
                      "min": 1,
                      "max": 255
                    }
                  }
                },
                "003": {
                  "name": "fan",
                  "describe": "电风扇，通风散热，支持开关和风速调节。",
                  "property": {
                    "power": {
                      "type": "bool",
                      "range": [
                        true,
                        false
                      ]
                    },
                    "level": {
                      "type": "uint",
                      "min": 1,
                      "max": 4
                    }
                  }
                },
                "004": {
                  "name": "fan",
                  "describe": "空调，调节室温，支持开关和温度调节。",
                  "property": {
                    "power": {
                      "type": "bool",
                      "range": [
                        true,
                        false
                      ]
                    },
                    "level": {
                      "type": "uint",
                      "min": 15,
                      "max": 30
                    }
                  }
                },
                "005": {
                  "name": "extractor",
                  "describe": "油烟机，厨房油烟风力排污，支持开关和风速调节。",
                  "property": {
                    "power": {
                      "type": "bool",
                      "range": [
                        true,
                        false
                      ]
                    },
                    "level": {
                      "type": "uint",
                      "min": 1,
                      "max": 3
                    }
                  }
                }
              },
              "devices": [
                {
                  "id": "dev-1",
                  "name": "可调光照明灯",
                  "local": "客厅",
                  "device": {
                    "model": "002",
                    "state": {
                      "power": true,
                      "level": 128
                    }
                  }
                },
                {
                  "id": "dev-2",
                  "name": "照明灯",
                  "local": "卫生间",
                  "device": {
                    "model": "001",
                    "state": {
                      "power": false
                    }
                  }
                },
                {
                  "id": "dev-3",
                  "name": "照明灯",
                  "local": "厨房",
                  "device": {
                    "model": "001",
                    "state": {
                      "power": true
                    }
                  }
                },
                {
                  "id": "dev-4",
                  "name": "照明灯",
                  "local": "卧室",
                  "device": {
                    "model": "001",
                    "state": {
                      "power": false
                    }
                  }
                },
                {
                  "id": "dev-5",
                  "name": "电源插座",
                  "local": "客厅",
                  "device": {
                    "model": "001",
                    "state": {
                      "power": false
                    }
                  }
                },
                {
                  "id": "dev-6",
                  "name": "空调",
                  "local": "客厅",
                  "device": {
                    "model": "004",
                    "state": {
                      "power": false,
                      "level": 20
                    }
                  }
                },
                {
                  "id": "dev-7",
                  "name": "空调",
                  "local": "卧室",
                  "device": {
                    "model": "004",
                    "state": {
                      "power": false,
                      "level": 26
                    }
                  }
                },
                {
                  "id": "dev-8",
                  "name": "油烟机",
                  "local": "厨房",
                  "device": {
                    "model": "005",
                    "state": {
                      "power": true,
                      "level": 2
                    }
                  }
                }
              ],
              "layout": [
                "客厅",
                "卧室",
                "厨房",
                "卫生间"
              ]
            }
          }
        }
    )";
} // namespace hai

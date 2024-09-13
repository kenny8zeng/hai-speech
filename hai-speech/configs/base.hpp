#ifndef base_jksdfiojewioffodsifjweo7878
#define base_jksdfiojewioffodsifjweo7878

#include <string>
#include <cstdint>
#include <vector>

#include "tinyalsa/pcm.h"

namespace hai {
    /*********************************************************************
    * @brief ...
    */
    using buffer_t = std::vector< std::byte >;

    /*********************************************************************
    * @brief ...
    */
    using alsa_config_t = struct pcm_config;

    /*********************************************************************
    * @brief ...
    */
    struct configs_t {
        unsigned int    card;
        unsigned int    device;
        unsigned int    time;
        unsigned int    debug;
        uint16_t        port;
        alsa_config_t   alsa;
        std::string     host;
        std::string     resdir;
        std::string     workdir;
        std::string     session;
        std::string     local;
    };

} // namespace hai

#endif // base_jksdfiojewioffodsifjweo7878

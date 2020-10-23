/*
 * This file is 100% generated.  Any manual edits will likely be lost.
 *
 * This contains functions struct and other type definitions for shema in 
 * namespace dunedaq::ipm::viis to be serialized via nlohmann::json.
 */
#ifndef DUNEDAQ_IPM_VIISDM_NLJS_HPP
#define DUNEDAQ_IPM_VIISDM_NLJS_HPP


#include "ipm/viis/Structs.hpp"


#include <nlohmann/json.hpp>

namespace dunedaq::ipm::viis {

    using data_t = nlohmann::json;


    
    inline void to_json(data_t& j, const Conf& obj) {
        j["nIntsPerVector"] = obj.nIntsPerVector;
        j["queue_timeout_ms"] = obj.queue_timeout_ms;
    }
    
    inline void from_json(const data_t& j, Conf& obj) {
        if (j.contains("nIntsPerVector"))
            j.at("nIntsPerVector").get_to(obj.nIntsPerVector);    
        if (j.contains("queue_timeout_ms"))
            j.at("queue_timeout_ms").get_to(obj.queue_timeout_ms);    
    }
    
    // fixme: add support for MessagePack serializers (at least)

} // namespace dunedaq::ipm::viis

#endif // DUNEDAQ_IPM_VIISDM_NLJS_HPP
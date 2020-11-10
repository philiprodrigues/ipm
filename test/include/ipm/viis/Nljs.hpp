/*
 * This file is 100% generated.  Any manual edits will likely be lost.
 *
 * This contains functions struct and other type definitions for shema in 
 * namespace dunedaq::ipm::viis to be serialized via nlohmann::json.
 */
#ifndef DUNEDAQ_IPM_VIIS_NLJS_HPP
#define DUNEDAQ_IPM_VIIS_NLJS_HPP


#include "ipm/viis/Structs.hpp"


#include <nlohmann/json.hpp>

namespace dunedaq::ipm::viis {

    using data_t = nlohmann::json;


    
    inline void to_json(data_t& j, const Conf& obj) {
        j["nIntsPerVector"] = obj.nIntsPerVector;
        j["queue_timeout_ms"] = obj.queue_timeout_ms;
        j["topic"] = obj.topic;
        j["sender_type"] = obj.sender_type;
        j["connection_info"] = obj.connection_info;
    }
    
    inline void from_json(const data_t& j, Conf& obj) {
        if (j.contains("nIntsPerVector"))
            j.at("nIntsPerVector").get_to(obj.nIntsPerVector);    
        if (j.contains("queue_timeout_ms"))
            j.at("queue_timeout_ms").get_to(obj.queue_timeout_ms);    
        if (j.contains("topic"))
            j.at("topic").get_to(obj.topic);    
        if (j.contains("sender_type"))
            j.at("sender_type").get_to(obj.sender_type);    
        obj.connection_info = j.at("connection_info");
    }
    
    // fixme: add support for MessagePack serializers (at least)

} // namespace dunedaq::ipm::viis

#endif // DUNEDAQ_IPM_VIIS_NLJS_HPP
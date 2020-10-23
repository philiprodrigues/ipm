/*
 * This file is 100% generated.  Any manual edits will likely be lost.
 *
 * This contains struct and other type definitions for shema in 
 * namespace dunedaq::ipm::viir.
 */
#ifndef DUNEDAQ_IPM_VIIRDM_STRUCTS_HPP
#define DUNEDAQ_IPM_VIIRDM_STRUCTS_HPP

#include <cstdint>


namespace dunedaq::ipm::viir {

    // @brief Same as a size_t in gcc v8.2.0
    using Size_t = uint64_t;

    // @brief Same as an int in gcc v8.2.0
    using Int = int32_t;

    // @brief VectorIntIPMReceiverDAQModule Configuration
    struct Conf {

        // @brief Number of numbers
        Size_t nIntsPerVector;

        // @brief Milliseconds to wait on queue before timing out
        Int queue_timeout_ms;
    };

} // namespace dunedaq::ipm::viir

#endif // DUNEDAQ_IPM_VIIRDM_STRUCTS_HPP
/*
 * This file is 100% generated.  Any manual edits will likely be lost.
 *
 * This contains struct and other type definitions for shema in 
 * namespace dunedaq::ipm::viis.
 */
#ifndef DUNEDAQ_IPM_VIISDM_STRUCTS_HPP
#define DUNEDAQ_IPM_VIISDM_STRUCTS_HPP

#include <cstdint>


namespace dunedaq::ipm::viis {

    // @brief Same as a size_t in gcc v8.2.0
    using Size_t = uint64_t;

    // @brief Same as an int in gcc v8.2.0
    using Int = int32_t;

    // @brief VectorIntIPMSenderDAQModule Configuration
    struct Conf {

        // @brief Number of numbers
        Size_t nIntsPerVector;

        // @brief Milliseconds to wait on queue before timing out
        Int queue_timeout_ms;
    };

} // namespace dunedaq::ipm::viis

#endif // DUNEDAQ_IPM_VIISDM_STRUCTS_HPP
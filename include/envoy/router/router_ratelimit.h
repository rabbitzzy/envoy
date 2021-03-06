#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "envoy/http/filter.h"
#include "envoy/http/header_map.h"
#include "envoy/ratelimit/ratelimit.h"

namespace Envoy {
namespace Router {
/**
 * Base interface for generic rate limit action.
 */
class RateLimitAction {
public:
  virtual ~RateLimitAction() = default;

  /**
   * Potentially append a descriptor entry to the end of descriptor.
   * @param route supplies the target route for the request.
   * @param descriptor supplies the descriptor to optionally fill.
   * @param local_service_cluster supplies the name of the local service cluster.
   * @param headers supplies the header for the request.
   * @param remote_address supplies the trusted downstream address for the connection.
   * @return true if the RateLimitAction populated the descriptor.
   */
  virtual bool populateDescriptor(const RouteEntry& route, RateLimit::Descriptor& descriptor,
                                  const std::string& local_service_cluster,
                                  const Http::HeaderMap& headers,
                                  const Network::Address::Instance& remote_address) const PURE;
};

using RateLimitActionPtr = std::unique_ptr<RateLimitAction>;

/**
 * Rate limit configuration.
 */
class RateLimitPolicyEntry {
public:
  virtual ~RateLimitPolicyEntry() = default;

  /**
   * @return the stage value that the configuration is applicable to.
   */
  virtual uint64_t stage() const PURE;

  /**
   * @return runtime key to be set to disable the configuration.
   */
  virtual const std::string& disableKey() const PURE;

  /**
   * Potentially populate the descriptor array with new descriptors to query.
   * @param route supplies the target route for the request.
   * @param descriptors supplies the descriptor array to optionally fill.
   * @param local_service_cluster supplies the name of the local service cluster.
   * @param headers supplies the header for the request.
   * @param remote_address supplies the trusted downstream address for the connection.
   */
  virtual void populateDescriptors(const RouteEntry& route,
                                   std::vector<RateLimit::Descriptor>& descriptors,
                                   const std::string& local_service_cluster,
                                   const Http::HeaderMap& headers,
                                   const Network::Address::Instance& remote_address) const PURE;
};

/**
 * Rate limiting policy.
 */
class RateLimitPolicy {
public:
  virtual ~RateLimitPolicy() = default;

  /**
   * @return true if there is no rate limit policy for all stage settings.
   */
  virtual bool empty() const PURE;

  /**
   * @param stage the value for finding applicable rate limit configurations.
   * @return set of RateLimitPolicyEntry that are applicable for a stage.
   */
  virtual const std::vector<std::reference_wrapper<const RateLimitPolicyEntry>>&
  getApplicableRateLimit(uint64_t stage) const PURE;
};

} // namespace Router
} // namespace Envoy

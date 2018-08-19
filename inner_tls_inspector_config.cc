#include "envoy/registry/registry.h"
#include "envoy/server/filter_config.h"

#include "inner_tls_inspector.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace InnerTlsInspector {

/**
 * Config registration for the InnerTlsInspector filter. @see NamedNetworkFilterConfigFactory.
 */
class InnerTlsInspectorConfigFactory : public Server::Configuration::NamedNetworkFilterConfigFactory {
public:
  // NamedNetworkFilterConfigFactory
  Network::FilterFactoryCb createFilterFactory(const Json::Object&,
                                               Server::Configuration::FactoryContext&) override {
    return [](Network::FilterManager& filter_manager) -> void {
      filter_manager.addReadFilter(std::make_shared<InnerTlsInspectorFilter>());
    };
  }

  Network::FilterFactoryCb
  createFilterFactoryFromProto(const Protobuf::Message&,
                               Server::Configuration::FactoryContext&) override {
    return [](Network::FilterManager& filter_manager) -> void {
      filter_manager.addReadFilter(std::make_shared<InnerTlsInspectorFilter>());
    };
  }

  ProtobufTypes::MessagePtr createEmptyConfigProto() override {
    return ProtobufTypes::MessagePtr{new Envoy::ProtobufWkt::Empty()};
  }

  std::string name() override { return "inner_tls_inspector"; }
};

/**
 * Static registration for the InnerTlsInspector filter. @see RegisterFactory.
 */
static Registry::RegisterFactory<InnerTlsInspectorConfigFactory,
                                 Server::Configuration::NamedNetworkFilterConfigFactory>
    registered_;

} // namespace InnerTlsInspector
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy

#pragma once

#include "envoy/network/filter.h"

#include "common/common/logger.h"
#include "openssl/bytestring.h"
#include "openssl/ssl.h"


namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace InnerTlsInspector {

/**
 * Implementation of a basic InnerTlsInspector filter.
 */
class InnerTlsInspectorFilter : public Network::ReadFilter, Logger::Loggable<Logger::Id::filter> {
public:
  static constexpr size_t TLS_MAX_CLIENT_HELLO = 64 * 1024;


  InnerTlsInspectorFilter();

  bssl::UniquePtr<SSL> newSsl();

  // Network::ReadFilter
  Network::FilterStatus onData(Buffer::Instance& data, bool end_stream) override;
  Network::FilterStatus onNewConnection() override { return Network::FilterStatus::Continue; }
  void initializeReadFilterCallbacks(Network::ReadFilterCallbacks& callbacks) override {
    read_callbacks_ = &callbacks;
  }

private:
  Network::ReadFilterCallbacks* read_callbacks_{};
  void parseClientHello(const void* data, size_t len);
  uint32_t maxClientHelloSize() const { return TLS_MAX_CLIENT_HELLO; }
  void done(bool success);
  void onServername(absl::string_view name);

  bssl::UniquePtr<SSL_CTX> ssl_ctx_;
  bssl::UniquePtr<SSL> ssl_;
  uint64_t read_{0};
  bool clienthello_success_{false};
  static thread_local uint8_t buf_[TLS_MAX_CLIENT_HELLO];


};

} // namespace InnerTlsInspector
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy

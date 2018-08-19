#include "inner_tls_inspector.h"

#include "envoy/buffer/buffer.h"
#include "envoy/network/connection.h"

#include "common/common/assert.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace InnerTlsInspector {

InnerTlsInspectorFilter::InnerTlsInspectorFilter() :
    ssl_ctx_(SSL_CTX_new(TLS_with_buffers_method())),
    ssl_(newSsl()) {

    ENVOY_LOG(error, "InnerTlsInspectorFilter");
    SSL_CTX_set_options(ssl_ctx_.get(), SSL_OP_NO_TICKET);
    SSL_CTX_set_session_cache_mode(ssl_ctx_.get(), SSL_SESS_CACHE_OFF);
    SSL_CTX_set_select_certificate_cb(
          ssl_ctx_.get(), [](const SSL_CLIENT_HELLO* client_hello) -> ssl_select_cert_result_t {
            const uint8_t* data;
            size_t len;
            ENVOY_LOG(error, "SSL_CTX_set_select_certificate_cb");
            if (SSL_early_callback_ctx_extension_get(
                    client_hello, TLSEXT_TYPE_application_layer_protocol_negotiation, &data, &len)) {
              InnerTlsInspectorFilter* filter = static_cast<InnerTlsInspectorFilter*>(SSL_get_app_data(client_hello->ssl));

              if(filter != nullptr) {}
//              filter->onALPN(data, len);
            }
            return ssl_select_cert_success;
          });
    SSL_CTX_set_tlsext_servername_callback(
          ssl_ctx_.get(), [](SSL* ssl, int* out_alert, void*) -> int {
            InnerTlsInspectorFilter* filter = static_cast<InnerTlsInspectorFilter*>(SSL_get_app_data(ssl));
            ENVOY_LOG(error, "SSL_CTX_set_tlsext_servername_callback");
            if(filter != nullptr) {}
            filter->onServername(SSL_get_servername(ssl, TLSEXT_NAMETYPE_host_name));

            // Return an error to stop the handshake; we have what we wanted already.
            *out_alert = SSL_AD_USER_CANCELLED;
            return SSL_TLSEXT_ERR_ALERT_FATAL;
          });

    SSL_set_app_data(ssl_.get(), this);
    SSL_set_accept_state(ssl_.get());
}

thread_local uint8_t InnerTlsInspectorFilter::buf_[TLS_MAX_CLIENT_HELLO];


bssl::UniquePtr<SSL> InnerTlsInspectorFilter::newSsl() {
    ENVOY_LOG(error, "newSsl()");
    return bssl::UniquePtr<SSL>{SSL_new(ssl_ctx_.get())};
}

void InnerTlsInspectorFilter::parseClientHello(const void* data, size_t len) {
  // Ownership is passed to ssl_ in SSL_set_bio()
  bssl::UniquePtr<BIO> bio(BIO_new_mem_buf(data, len));

  // Make the mem-BIO return that there is more data
  // available beyond it's end
  BIO_set_mem_eof_return(bio.get(), -1);

  SSL_set_bio(ssl_.get(), bio.get(), bio.get());
  bio.release();

  int ret = SSL_do_handshake(ssl_.get());

  // This should never succeed because an error is always returned from the SNI callback.
  ASSERT(ret <= 0);
  switch (SSL_get_error(ssl_.get(), ret)) {
  case SSL_ERROR_WANT_READ:
    if (read_ == maxClientHelloSize()) {
      done(false);
    }
    break;
  case SSL_ERROR_SSL:
    if (clienthello_success_) {
//      cb_->socket().setDetectedTransportProtocol(TransportSockets::TransportSocketNames::get().Tls);
    }
    done(true);
    break;
  default:
    done(false);
    break;
  }
}


Network::FilterStatus InnerTlsInspectorFilter::onData(Buffer::Instance& data, bool end_stream) {
  ENVOY_CONN_LOG(error, "InnerTlsInspector: got {} bytes", read_callbacks_->connection(), data.length());
//  virtual void copyOut(size_t start, uint64_t size, void* data) const PURE;
  int len = (data.length() < TLS_MAX_CLIENT_HELLO) ? data.length() : TLS_MAX_CLIENT_HELLO ;
  data.copyOut(0, len, buf_);

  for(int a=0; a<len; a++) {
    std::cout << *(buf_+a);
  }
  std::cout << std::endl;


  if(end_stream) {}
//  read_callbacks_->connection().write(data, end_stream);
//  ASSERT(0 == data.length());
  return Network::FilterStatus::Continue;
}

void InnerTlsInspectorFilter::done(bool success) {
  ENVOY_LOG(error, "inner tls inspector: done: {}", success);
//  timer_.reset();
//  file_event_.reset();
//  cb_->continueFilterChain(success);
}

void InnerTlsInspectorFilter::onServername(absl::string_view name) {
  if (!name.empty()) {
//    cb_->socket().setRequestedServerName(name);
  }
  clienthello_success_ = true;
}

} // namespace InnerTlsInspector
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy

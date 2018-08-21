# Inner TLS Inspector filter

This filter is based on [envoy-filter-example](https://github.com/envoyproxy/envoy-filter-example)



## Building

To build the Envoy static binary:

1. Update envoy's submodule
    ```
    git submodule update --init
    ```

1. Run the build image
    ```
    ./envoy/ci/run_envoy_docker.sh bash`
    ```

1. Build envoy from inside the build image
    ```
    bazel build --verbose_failures=true //:envoy
    ```

1. Create mtls certificates
    ```
    ./create_certs.sh
    ```

## Running

1. Make an alias to envoy
    ```
    alias envoy=~/envoyproxy/envoy-inner-tls-inspector/.cache/bazel/_bazel_envoybuild/b570b5ccd0454dc9af9f65ab1833764d/execroot/envoy_filter_example/bazel-out/k8-fastbuild/bin/envoy
    ```

1. Run first instance of envoy
    ```
    envoy -c ./envoy_config.json --v2-config-only
    ```

1. From a new terminal, run a second instance of envoy
    ```
    envoy -c envoy_config2.json --v2-config-only --base-id 2
    ```

1. From a new terminal make a request to the first envoy
    ```
    curl -Ivk https://edition.cnn.com --resolve edition.cnn.com:443:127.0.0.1
    ```


1. In the second envoy's log, one can spot the inner SNI `edition.cnn.com` while the outer SNI is `envoy2.local`
    ```
    ***** InnerTlsInspectorFilter.SSL_CTX_set_tlsext_servername_callback: edition.cnn.com
    [2018-08-21T11:53:18.537Z] "- - -" 0 - 783 6641 332 - "-" "-" "-" "-" "151.101.1.67:443"
    ```

## How it works

`tls_inspector` is a listener filter. It overrides `OnAccept()` which makes periodic calls to `OnRead()`.
`OnRead()` fills a buffer from the underlying socket with the TLS handshake. The buffer is parsed by `parseClientHello()`.
https://github.com/envoyproxy/envoy/blob/master/source/extensions/filters/listener/tls_inspector/tls_inspector.cc


The new `inner_tls_inspector` is a network filter. It overrides `OnData()` (instead of `OnAccept()`).
Network filters don't have access to the underlying socket but to the connection object.
The idea is to fill a buffer using the data that is passed to `OnData()` and then parse it the same way `tls_inspector` does.
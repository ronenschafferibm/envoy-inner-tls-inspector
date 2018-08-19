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


1. Currently, the filter simply logs the incomind data. We can spot the inner SNI.
    ```
    [2018-08-19 17:40:13.003][25076][error][filter] external/envoy/source/extensions/filters/listener/tls_inspector/tls_inspector.cc:74] tls inspector: new connection accepted
    [2018-08-19 17:40:13.003][25076][error][filter] external/envoy/source/extensions/filters/listener/tls_inspector/tls_inspector.cc:149] tls inspector: recv: 170
    SSL_CTX_set_tlsext_servername_callback: envoy2.local
    [2018-08-19 17:40:13.003][25076][error][filter] external/envoy/source/extensions/filters/listener/tls_inspector/tls_inspector.cc:176] tls inspector: done: true
    [2018-08-19 17:40:13.004][25076][error][filter] inner_tls_inspector.cc:54] newSsl()
    [2018-08-19 17:40:13.004][25076][error][filter] inner_tls_inspector.cc:17] InnerTlsInspectorFilter
    [2018-08-19 17:40:13.072][25076][error][filter] inner_tls_inspector.cc:93] [C2] InnerTlsInspector: got 517 bytes
    �M@�����Ntd�t�n�/�!�_݁�
                                      �L��0�,�(�$��
    ����kjih9876�����2�.�*�&���=5��/�+�'�#��	����g@?>3210����EDCB�1�-�)�%�����A�
    �5edition.cnn.com
                         
    


     3t
                                           http/1.1�
    [2018-08-19 17:40:13.143][25076][error][filter] inner_tls_inspector.cc:93] [C2] InnerTlsInspector: got 126 bytes
    FBA��h�I��nh�P�R��
                              ٚ1�:�z�����w�^/Fs�E.�e�����D��ͷNy�(�^e�z٘�i��������=�mE���
    �E$쑴=Û\S'
    [2018-08-19 17:40:13.210][25076][error][filter] inner_tls_inspector.cc:93] [C2] InnerTlsInspector: got 109 bytes
    h�^e�z٘Ő����ҽ�rPyp�Ow'�a��2,c5�:O@���E�Պ/,&�q�q�>�Y��@Q�i��m�`vk�q� �����
    ��a����� Unﶲ�
    [2018-08-19 17:40:13.279][25076][error][filter] inner_tls_inspector.cc:93] [C2] InnerTlsInspector: got 31 bytes
    �^e�z٘ƌ�f�
    .�	�����	��ѣ
    [2018-08-19T14:40:13.004Z] "- - -" 0 - 783 6642 342 - "-" "-" "-" "-" "151.101.193.67:443"
    [2018-08-19 17:54:42.911][25071][info][main] external/envoy/source/server/drain_manager_impl.cc:63] shutting down parent after drain
    ```

## How it works

`tls_inspector` is a listener filter. It overrides `OnAccept()` which makes periodic calls to `OnRead()`.
`OnRead()` fills a buffer from the underlying socket with the TLS handshake. The buffer is parsed by `parseClientHello()`.
https://github.com/envoyproxy/envoy/blob/master/source/extensions/filters/listener/tls_inspector/tls_inspector.cc


The new `inner_tls_inspector` is a network filter. It overrides `OnData()` (instead of `OnAccept()`).
Network filters don't have access to the underlying socket but to the connection object.
The idea is to fill a buffer using the data that is passed to `OnData()` and then parse it the same way `tls_inspector` does.
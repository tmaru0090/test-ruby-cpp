# 一応rubyとsfmlの連携用

g++ -o main main.cc -I"/usr/include/ruby-3.0.0/"  "$(pkg-config --cflags --libs ruby)" "$(pkg-config --cflags --libs sfml-all)"
g++ -o main main.cc -I"/usr/include/ruby-3.0.0/"  "$(pkg-config --cflags --libs ruby)" "$(pkg-config --cflags --libs sfml-all)"

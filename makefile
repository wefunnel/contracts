all:
	make compile

compile:
	mkdir -p build/we_funnel
	#eosio-cpp -o build/funnel.wasm funnel.hpp funnel.cpp
	eosio-cpp -o build/we_funnel/we_funnel.wasm we_funnel.cpp

deploy:
	make compile
	cleos set contract wefunnel5555 build/we_funnel

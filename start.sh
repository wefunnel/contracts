#!/bin/sh

set -e

which nodeos > /dev/null || (echo "nodeos binary not found in PATH, exiting" && exit 1)

nodeos \
  --signature-provider EOS5D2X8LW4zxV93RMdrWoDQ2cpnYd3HxRBH7QuW76LxCPJKj6wZy=KEY:5KDs6nody8ZuxXio2ZPmiiSvJsNF5hmwZqAepDMj6VPqdqmmVrq \
  --plugin eosio::producer_plugin \
  --plugin eosio::producer_api_plugin \
  --plugin eosio::chain_plugin \
  --plugin eosio::chain_api_plugin \
  --plugin eosio::http_plugin \
  --plugin eosio::history_api_plugin \
  --plugin eosio::history_plugin \
  --producer-name seetheory111 \
  --http-server-address 0.0.0.0:8888 \
  --p2p-listen-endpoint 127.0.0.1:9010 \
  --access-control-allow-origin=* \
  --contracts-console \
  --http-validate-host=false \
  --verbose-http-errors \
  --enable-stale-production \
  --p2p-peer-address peer.jungle.alohaeos.com:9876 \
  --p2p-peer-address peer1-jungle.eosphere.io:9876 \
  --p2p-peer-address jungle.eossweden.org:59875
  #--http-validate-host=false

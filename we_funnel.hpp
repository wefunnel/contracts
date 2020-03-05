#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <string>
#include <eosio/crypto.hpp>

class [[eosio::contract]] we_funnel : public eosio::contract {

private:
  /**
   * A multi-index of funnels. A funnel is a single ip address with many tcp
   * routes.
   **/
  struct [[eosio::table]] funnel_info {
    eosio::name owner;
    uint64_t rate = 0;
    uint64_t active = 0;
    uint64_t start_time = 0;
    uint64_t last_updated =  0;
    uint64_t total_slots = 16;
    uint64_t available_slots = 16;
    // Perform tests for this
    uint64_t bytes_per_second = 0;

    auto primary_key() const { return owner.value; }
    uint64_t active_index() const { return active; }
    uint64_t rate_index() const { return rate; }
    uint64_t slots_index() const { return available_slots; }
  };

  typedef eosio::multi_index<eosio::name("funnels"), funnel_info,
    eosio::indexed_by<eosio::name("active"), eosio::const_mem_fun<funnel_info, uint64_t, &funnel_info::active_index>>,
    eosio::indexed_by<eosio::name("rate"), eosio::const_mem_fun<funnel_info, uint64_t, &funnel_info::rate_index>>,
    eosio::indexed_by<eosio::name("slots"), eosio::const_mem_fun<funnel_info, uint64_t, &funnel_info::slots_index>>
    > funnels;

  /**
   * A TCP route definition, incoming ip and outgoing hostname encrypted by the
   * client with the owner public key.
   **/
  struct [[eosio::table]] relay_route {
    uint64_t pkey;
    eosio::checksum256 id;
    eosio::name client;
    eosio::name owner;
    std::string incoming_uri_enc = "";
    std::string outgoing_uri_enc = "";
    std::string funnel_uri_enc = "";
    uint64_t start_time = 0;
    uint64_t rate = 0;

    auto primary_key() const { return pkey; }
    eosio::checksum256 id_index() const { return id; }
    uint64_t client_index() const { return client.value; }
    uint64_t owner_index() const { return owner.value; }
  };

  typedef eosio::multi_index<eosio::name("routes"), relay_route,
    eosio::indexed_by<eosio::name("id"), eosio::const_mem_fun<relay_route, eosio::checksum256, &relay_route::id_index>>,
    eosio::indexed_by<eosio::name("client"), eosio::const_mem_fun<relay_route, uint64_t, &relay_route::client_index>>,
    eosio::indexed_by<eosio::name("owner"), eosio::const_mem_fun<relay_route, uint64_t, &relay_route::owner_index>>
  > routes;

  funnels _funnels;
  routes _routes;

  // Incrementing used as route primary key
  uint64_t total_routes = 0;

  uint32_t total_relays = 0;
  uint32_t active_relays = 0;

  void freefunnel(eosio::name owner, eosio::name client, eosio::name caller);
  eosio::checksum256 route_id(eosio::name owner, eosio::name client);

public:
  /**
   * self - name of account contract is deployed on
   * first_receiver - name of account executing constructor
   **/
  we_funnel(eosio::name self, eosio::name first_receiver, eosio::datastream<const char *> ds);

  [[eosio::action]]
  void wipe();

  [[eosio::action]]
  void createfunnel(eosio::name owner, uint64_t rate, uint64_t slots);

  [[eosio::action]]
  void leasefunnel(eosio::name owner, eosio::name client);

  [[eosio::action]]
  void freeclient(eosio::name owner, eosio::name client);

  [[eosio::action]]
  void freeowner(eosio::name owner, eosio::name client);

  [[eosio::action]]
  void setrate(eosio::name username, uint64_t rate);

  [[eosio::action]]
  void setinouturis(eosio::name owner, eosio::name client, std::string incoming_uri_enc, std::string outgoing_uri_enc);

  [[eosio::action]]
  void setfunneluri(eosio::name owner, eosio::name client, std::string uri);

  [[eosio::action]]
  void printfunnel(eosio::name username);

};

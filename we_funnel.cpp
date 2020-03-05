#include "we_funnel.hpp"
#include <eosio/print.hpp>
#include <eosio/system.hpp>
#include <eosio/crypto.hpp>

using namespace eosio;
using namespace std;

we_funnel::we_funnel(name self, name first_receiver, datastream<const char*>ds):
  contract(self, first_receiver, ds),
  _funnels(self, self.value),
  _routes(self, self.value) {}

void we_funnel::wipe() {
  require_auth(get_self());
  for (auto itr = _funnels.begin(); itr != _funnels.end();) {
    itr = _funnels.erase(itr);
  }
  for (auto itr = _routes.begin(); itr != _routes.end();) {
    itr = _routes.erase(itr);
  }
}

void we_funnel::createfunnel(name owner, uint64_t rate, uint64_t slots) {
  require_auth(owner);
  auto funnel_iterator = _funnels.find(owner.value);
  if (funnel_iterator == _funnels.end()) {
    funnel_iterator = _funnels.emplace(owner, [&](auto& new_funnel) {
      new_funnel.owner = owner;
      new_funnel.rate = rate;
      new_funnel.total_slots = slots;
    });
  }
}

checksum256 we_funnel::route_id(name owner, name client) {
  string pair = client.to_string() + "-" + owner.to_string();
  return sha256(const_cast<char*>(pair.c_str()), pair.size());
}

void we_funnel::leasefunnel(name owner, name client) {
  require_auth(client);
  auto& funnel = _funnels.get(owner.value, "Funnel does not exist");
  // check(funnel.active, "Funnel is not active");
  check(funnel.available_slots >= 0, "Funnel does not have slots available");

  auto id_index = _routes.get_index<name("id")>();
  auto existing_route = id_index.find(route_id(owner, client));
  check(existing_route == id_index.end(), "Cannot have multiple slots in one funnel");

  _funnels.modify(funnel, client, [&](auto& modified_funnel) {
    modified_funnel.available_slots -= 1;
  });
  _routes.emplace(client, [&](auto& new_route) {
    new_route.pkey = total_routes;
    new_route.id = route_id(owner, client);
    new_route.client = client;
    new_route.owner = owner;
    new_route.rate = funnel.rate;
    // Set start time when funnel uri is uploaded
  });
  total_routes += 1;
}

void we_funnel::freeclient(name owner, name client) {
  require_auth(client);
  we_funnel::freefunnel(owner, client, client);
}

void we_funnel::freeowner(name owner, name client) {
  require_auth(owner);
  we_funnel::freefunnel(owner, client, owner);
}

void we_funnel::freefunnel(name owner, name client, name caller) {
  // Either owner or client can cancel
  auto& funnel = _funnels.get(owner.value, "Funnel does not exist");
  auto id_index = _routes.get_index<name("id")>();
  auto& route = id_index.get(route_id(owner, client), "Route does not exist");
  auto now = current_time_point().sec_since_epoch();
  auto seconds_used = now - route.start_time;
  auto cost = route.rate * seconds_used / 60;
  // Transfer tokens to funnel owner
  //
  _funnels.modify(funnel, caller, [&](auto& modified_funnel) {
    modified_funnel.available_slots += 1;
  });
  _routes.erase(route);
}

void we_funnel::setrate(name owner, uint64_t rate) {
  require_auth(owner);
  // returns an iterator
  auto& funnel = _funnels.get(owner.value, "Funnel does not exist");
  _funnels.modify(funnel, owner, [&](auto& modified_funnel) {
    modified_funnel.rate = rate;
  });
}

void we_funnel::setinouturis(
  name owner,
  name client,
  string incoming_uri_enc,
  string outgoing_uri_enc
) {
  auto& funnel = _funnels.get(owner.value, "Funnel does not exist");
  auto id_index = _routes.get_index<name("id")>();
  auto& route = id_index.get(route_id(owner, client), "Route does not exist");
  require_auth(client);
  _routes.modify(route, client, [&](auto& modified_route) {
    modified_route.outgoing_uri_enc = outgoing_uri_enc;
    modified_route.incoming_uri_enc = incoming_uri_enc;
  });
}

void we_funnel::setfunneluri(name owner, name client, string uri) {
  require_auth(owner);
  auto id_index = _routes.get_index<name("id")>();
  auto& route = id_index.get(route_id(owner, client), "Route does not exist");
  _routes.modify(route, owner, [&](auto& modified_route) {
    modified_route.funnel_uri_enc = uri;
    modified_route.start_time = current_time_point().sec_since_epoch();
  });
}

void we_funnel::printfunnel(eosio::name username) {
  auto& funnel = _funnels.get(username.value, "Funnel does not exist");
  print("Funnel rate: ", funnel.rate);
}

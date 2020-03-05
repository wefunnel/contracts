#include "ftoken.hpp"

using namespace eosio;
using namespace std;

ftoken(name self, name first_receiver, datastream<const char *>ds)
{}

[[eosio::on_notify("eosio.token::transfer")]]
void we_funnel::deposit(name from, name to, asset quantity, string memo) {
  if (from == get_self() || to != get_self()) {
    return;
  }
  auto eos_asset = asset(0, symbol("EOS", 4));

  check(quantity.amount > 0, "Positive amount required");
  check(
    quantity.symbol == funnel_symbol ||
    quantity.symbol.raw() == eos_asset.symbol.raw(),
    "Token must be EOS or FNL"
  );

  balance_table balance(get_self(), from.value);
  auto existing_balance = balance.find(funnel_symbol.raw());
  if (existing_balance != balance.end()) {
    balance.modify(existing_balance, get_self(), [&](auto &modified_balance) {
      if (quantity.symbol.raw() == eos_asset.symbol.raw()) {
        // exchange for FNL
        modified_balance.funds += asset(quantity.amount * 10, funnel_symbol);
      } else if (quantity.symbol == funnel_symbol) {
        // Exchange for EOS
        // new_balance.funds
      }
    });
  } else {
    balance.emplace(get_self(), [&](auto &new_balance) {
      if (quantity.symbol.raw() == eos_asset.symbol.raw()) {
        // exchange for FNL
        new_balance.funds = asset(quantity.amount * 10, funnel_symbol);
      } else if (quantity.symbol == funnel_symbol) {
        // Exchange for EOS
        // new_balance.funds
      }
    });
  }
}

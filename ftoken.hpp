#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

class [[eosio::contract]] ftoken : public eosio::contract {

private:
  const eosio::symbol token_symbol;

  struct [[eosio::table]] balance {
    eosio::asset funds;
    auto primary_key() const { return funds.symbol.raw(); }
  }

  typedef eosio::multi_index<eosio::name("balances"), balance> balance_table;

public:

  ftoken(
    eosio::name self,
    eosio::name first_receiver,
    eosio::datastream<const char *> ds
  );

  [[eosio::on_notify("eosio.token::transfer")]]
  void purchase_tokens(
    eosio::name from,
    eosio::name to,
    eosio::asset quantity,
    std::string memo
  );

  [[eosio::on_notify("eosio.token::transfer")]]
  void deposit(eosio::name from, eosio::name to, eosio::asset quantity, std::string memo);

}

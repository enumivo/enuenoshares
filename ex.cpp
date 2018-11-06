#include "ex.hpp"

#include <cmath>
#include <enulib/action.hpp>
#include <enulib/asset.hpp>
#include "enu.token.hpp"

using namespace enumivo;
using namespace std;

void ex::receivedenu(const currency::transfer &transfer) {
  if (transfer.to != _self) {
    return;
  }

  // get ENU balance
  double enu_balance = enumivo::token(N(enu.token)).
	   get_balance(N(enu.eno.mm), enumivo::symbol_type(ENU_SYMBOL).name()).amount;
  enu_balance = enu_balance/10000;

  // get ENO balance
  double eno_balance = enumivo::token(N(token.eurno)).
	   get_balance(N(enu.eno.mm), enumivo::symbol_type(ENO_SYMBOL).name()).amount;
  eno_balance = eno_balance/10000;

  // get total shares
  double shares = enumivo::token(N(shares.coin)).
	   get_supply(enumivo::symbol_type(ENUENO_SYMBOL).name()).amount;
  shares = shares/10000;

  double received = transfer.quantity.amount;
  received = received/10000;

  double a = eno_balance;
  double b = 2 * eno_balance * enu_balance;
  double c = - eno_balance * enu_balance * received;

  double eu = (sqrt((b*b)-(4*a*c)) - b)/(2*a);
  double ee = received - eu;

  double new_shares = shares * (ee/(eu+enu_balance));

  auto quantity = asset(10000*new_shares, ENUENO_SYMBOL);

  action(permission_level{_self, N(active)}, N(shares.coin), N(issue),
         std::make_tuple(transfer.from, quantity,
                         std::string("Issue new ENUENO shares")))
      .send();

  action(permission_level{_self, N(active)}, N(enu.token), N(transfer),
         std::make_tuple(_self, N(enu.eno.mm), transfer.quantity,
                         std::string("Invest ENUENO shares with ENU")))
      .send();
}

void ex::receivedeno(const currency::transfer &transfer) {
  if (transfer.to != _self) {
    return;
  }

  // get ENU balance
  double enu_balance = enumivo::token(N(enu.token)).
	   get_balance(N(enu.eno.mm), enumivo::symbol_type(ENU_SYMBOL).name()).amount;
  enu_balance = enu_balance/10000;

  // get ENO balance
  double eno_balance = enumivo::token(N(token.eurno)).
	   get_balance(N(enu.eno.mm), enumivo::symbol_type(ENO_SYMBOL).name()).amount;
  eno_balance = eno_balance/10000;

  // get total shares
  double shares = enumivo::token(N(shares.coin)).
	   get_supply(enumivo::symbol_type(ENUENO_SYMBOL).name()).amount;
  shares = shares/10000;

  double received = transfer.quantity.amount;
  received = received/10000;

  double a = enu_balance;
  double b = 2 * enu_balance * eno_balance;
  double c = - enu_balance * eno_balance * received;

  double ue = (sqrt((b*b)-(4*a*c)) - b)/(2*a);
  double uu = received - ue;

  double new_shares = shares * (uu/(ue+eno_balance));

  auto quantity = asset(10000*new_shares, ENUENO_SYMBOL);

  action(permission_level{_self, N(active)}, N(shares.coin), N(issue),
         std::make_tuple(transfer.from, quantity,
                         std::string("Issue new ENUENO shares")))
      .send();

  action(permission_level{_self, N(active)}, N(token.eurno), N(transfer),
         std::make_tuple(_self, N(enu.eno.mm), transfer.quantity,
                         std::string("Invest ENUENO shares with ENU")))
      .send();
}

void ex::receivedshares(const currency::transfer &transfer) {
  if (transfer.to != _self) {
    return;
  }

  // get ENU balance
  double enu_balance = enumivo::token(N(enu.token)).
	   get_balance(N(enu.eno.mm), enumivo::symbol_type(ENU_SYMBOL).name()).amount;
  enu_balance = enu_balance/10000;

  // get ENO balance
  double eno_balance = enumivo::token(N(token.eurno)).
	   get_balance(N(enu.eno.mm), enumivo::symbol_type(ENO_SYMBOL).name()).amount;
  eno_balance = eno_balance/10000;

  // get total shares
  double shares = enumivo::token(N(shares.coin)).
	   get_supply(enumivo::symbol_type(ENUENO_SYMBOL).name()).amount;
  shares = shares/10000;

  double received = transfer.quantity.amount;
  received = received/10000;

  double enoportion = eno_balance*(received/shares);
  auto eno = asset(10000*enoportion, ENO_SYMBOL);

  action(permission_level{N(enu.eno.mm), N(active)}, N(token.eurno), N(transfer),
         std::make_tuple(N(enu.eno.mm), transfer.from, eno,
                         std::string("Divest ENUENO shares for ENO")))
      .send();

  double enuportion = enu_balance*(received/shares);
  auto enu = asset(10000*enuportion, ENU_SYMBOL);

  action(permission_level{N(enu.eno.mm), N(active)}, N(enu.token), N(transfer),
         std::make_tuple(N(enu.eno.mm), transfer.from, enu,
                         std::string("Divest ENUENO shares for ENU")))
      .send();

  action(permission_level{_self, N(active)}, N(shares.coin), N(retire),
         std::make_tuple(transfer.quantity, std::string("Retire ENUENO shares")))
      .send();
}

void ex::apply(account_name contract, action_name act) {

  if (contract == N(enu.token) && act == N(transfer)) {
    auto transfer = unpack_action_data<currency::transfer>();

    enumivo_assert(transfer.quantity.symbol == ENU_SYMBOL,
                 "Must send ENU");
    receivedenu(transfer);
    return;
  }

  if (contract == N(token.eurno) && act == N(transfer)) {
    auto transfer = unpack_action_data<currency::transfer>();

    enumivo_assert(transfer.quantity.symbol == ENO_SYMBOL,
                 "Must send ENO");
    receivedeno(transfer);
    return;
  }

  if (contract == N(shares.coin) && act == N(transfer)) {
    auto transfer = unpack_action_data<currency::transfer>();

    enumivo_assert(transfer.quantity.symbol == ENUENO_SYMBOL,
                 "Must send ENUENO");
    receivedshares(transfer);
    return;
  }

  if (act == N(transfer)) {
    auto transfer = unpack_action_data<currency::transfer>();
    enumivo_assert(false, "Must send ENO or ENU or ENUENO");
    return;
  }

  if (contract != _self) return;

}

extern "C" {
[[noreturn]] void apply(uint64_t receiver, uint64_t code, uint64_t action) {
  ex enuenoshares(receiver);
  enuenoshares.apply(code, action);
  enumivo_exit(0);
}
}

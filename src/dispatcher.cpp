#include "dispatcher.hpp"
#include "Store.hpp"
#include "client.hpp"
#include "resp.hpp"

void dispatche(command &cmd, Client &client, store &myStore) {
  if (cmd.args.empty()) {
    client.write_buffer = client.write_buffer + "-ERR empty command\r\n";
  } else if (cmd.args[0] == "set" || cmd.args[0] == "SET") {
    std::vector<std::string> params(cmd.args.begin() + 1, cmd.args.end());

    evalSET(params, myStore, client.write_buffer);
  } else if (cmd.args[0] == "get" || cmd.args[0] == "GET") {
    std::vector<std::string> params(cmd.args.begin() + 1, cmd.args.end());
    evalGET(params, myStore, client.write_buffer);
  } else if (cmd.args[0] == "del" || cmd.args[0] == "DEL") {

    evalDEL(cmd.args[1], myStore, client.write_buffer);
  } else {
    client.write_buffer += "-ERR unknown command '" + cmd.args[0] + "'\r\n";
  }
}

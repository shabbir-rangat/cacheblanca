#pragma once
#include "Store.hpp"
#include "client.hpp"
#include "eventloop.hpp"
#include <optional>
#include <string>
#include <vector>

void evalSET(std::vector<std::string> &str, store &myStore,
             std::string &outbuff);

void evalGET(std::vector<std::string> &key, store &myStore,
             std::string &outbuff);
void evalTTL();
void evalDEL(std::string &key, store &myStore, std::string &outbuff);

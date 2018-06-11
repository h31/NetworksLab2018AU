#pragma once

#include <vector>
#include <string>

size_t writeDNSARequest(std::vector<char> &buf, const std::string &hostname);

std::string parseIPFromDNSResponse(std::vector<char> &buf);

size_t writeDNSAResponse(std::vector<char> &buf, const std::string &hostname, const std::string &ipAddress);

std::string parseHostNameFromDNSRequest(std::vector<char> &buf);

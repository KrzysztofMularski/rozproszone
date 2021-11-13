#pragma once

#include "main.hpp"

struct packet_t;

void addToQueue(packet_t& qPos);

void removeFromQueue(packet_t& qPos);

bool canRead(packet_t& qPos);

bool canWrite(packet_t& qPos);
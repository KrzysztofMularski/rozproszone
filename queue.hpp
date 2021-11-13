#pragma once

#include "main.hpp"

struct packet_t;

void addToQueue(const packet_t&);

void removeFromQueue(const packet_t&);

bool canUse(const packet_t&);

bool canRead(const packet_t&);

bool canWrite(const packet_t&);

bool areSameReqsPresent(const packet_t&);
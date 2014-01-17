#ifndef EXPIRED_OBJECT_REMOVAL_H
#define EXPIRED_OBJECT_REMOVAL_H

#include <iostream>

/**
 * Handles deletion of game objects. Nothing special; just maintains a list of expired objects (e.g. dead enemies), and deletes them when it's safe.
 */

void removeExpiredObjects();

bool isMarked(void* object);

void markForRemoval(void *toBeEXTERMINATED, std::string objectType);

#endif

#include "parser.h"

// Global parser instance
static Parser globalParserInstance;

Parser& getGlobalParser() {
    return globalParserInstance;
}

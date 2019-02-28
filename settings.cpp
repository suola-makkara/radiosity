#include "settings.hpp"

const float Settings::epsilon = 0.0001f;

const unsigned int Settings::minWindowWidth = 800u;
const unsigned int Settings::minWindowHeight = 600u;

unsigned int Settings::windowWidth = 1200u;
unsigned int Settings::windowHeight = 800u;

Settings::Mode Settings::mode = Settings::EDIT;

#ifndef SETTINGS_HPP
#define SETTINGS_HPP

struct Settings
{
	static const float epsilon;

	static const unsigned int minWindowWidth;
	static const unsigned int minWindowHeight;
	
	static unsigned int windowWidth;
	static unsigned int windowHeight;

	enum Mode
	{
		EDIT,
		RENDER
	};

	static Mode mode;
};

#endif // SETTINGS_HPP

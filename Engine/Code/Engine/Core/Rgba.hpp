#pragma once
#include <string>
#include "Engine/Math/Vector4.hpp"
class Rgba
{
public:

	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

	Rgba::Rgba();
	Rgba(Vector4 color);
	Rgba(std::string text);
	bool operator==(const Rgba color);
	bool operator!=(const Rgba color);
	explicit	Rgba::Rgba( unsigned char redByte, unsigned char greenByte,unsigned char blueByte, unsigned char alphaByte=255 );

	void		SetAsBytes( unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte=255 );

	void		SetAsFloats( float normalizedRed, float normalizedgreen,float normalizedblue, float normalizedAlpha=1.0f );

	void	    GetAsFloats( float& out_normalizedRed,float& out_normalizedGreen,float& out_normalizedBlue,float& out_normalizedAlpha ) const;

	Vector4		GetAsFloats();
	void		GetAsFloatArray(float *arr);

	void		ScaleRGB( float rgbScale ); // Scales (and clamps) RGB components, but not A

	void		ScaleAlpha( float alphaScale ); // Scales (and clamps) Alpha, RGB is untouched

	void		SetFromText(const char* text);

	std::string GetAsString();

	static Rgba GetRandomColor();

	static Rgba WHITE;
	static Rgba FADED_WHITE;
	static Rgba RED;
	static Rgba GREEN;
	static Rgba BLUE;
	static Rgba YELLOW;
	static Rgba MAGENTA;
	static Rgba CYAN;
	static Rgba BLACK;
	static Rgba FADED_BLACK;
	static Rgba CONSOLE_BLACK;
	static Rgba CONSOLE_FADED_BLUE;

	static Rgba FADED_BLUE;
	static Rgba FADED_RED;
	static Rgba FADED_GREEN;

};
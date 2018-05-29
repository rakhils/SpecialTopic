#pragma once

class Mathutil;
class FloatRange
{
public:
	float min;
	float max;

	FloatRange();
	FloatRange(float initialMin,float initialMax);
	FloatRange(float initialMinMax);

	FloatRange operator + (const FloatRange value);

	float GetRandomInRange() const;
	void SetFromText(const char* text);

	static FloatRange Interpolate( const FloatRange& start, const FloatRange& end, float fractionTowardEnd );
};
#pragma once

class MathUtil;
class IntRange
{
public:
	int min;
	int max;

	IntRange();
	IntRange( const IntRange& copyFrom );
	IntRange(int initialMin,int initialMax);
	int GetRandomInRange();
	void SetFromText(const char* text);
};
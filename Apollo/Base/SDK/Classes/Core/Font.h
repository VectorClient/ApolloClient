#pragma once

class FontRepos {
private:
	virtual void constructor();
	virtual void a1();
	virtual void a2();
	virtual void a3();
	virtual void a4();
	virtual void a5();
public:
	virtual float getLineLength(std::string* str, float textSize, bool unknown);
	virtual float getLineHeight();
};
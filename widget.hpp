#pragma once
#include "includes.hpp"

class Widget {
private:
	ImVec2 size;
	ImVec2 position;
public:
	virtual void render(); //only call when going through element list (for future)

	void setWidth(float width);
	void setHeight(float height);
	void setPosX(float x);
	void setPosY(float y);

	float getWidth();
	float getHeight();
	float getPosX();
	float getPosY();

	ImVec2 getPos();
};

class TextWidget : public Widget {

private:
	std::string text = "";
public:
	void render();

	void setText(std::string text);

	std::string getText();

};
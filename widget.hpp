#pragma once
#include "includes.hpp"

class Widget {
protected:
	ImVec2 size;
	ImVec2 position;
	bool hidden;
public:
	virtual void render(); //only call when going through element list (for future)

	void setWidth(float width);
	void setHeight(float height);
	void setPosX(float x);
	void setPosY(float y);

	void setHidden(bool hidden);
	bool isHidden();

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

class ButtonWidget : public Widget {

private:
	std::function<void()> function;
	std::string text = "";

public:
	void render();

	void onClick(std::function<void()> function);

	void setText(std::string text);

	std::string getText();

};

class CheckBoxWidget : public Widget {

private:
	bool* boolean = false;
	std::string text = "";

public:
	void render();

	void onClick(bool* boolean);

	void setText(std::string text);

	std::string getText();

};
#pragma once
#include "includes.hpp"

class Widget {
protected:
	ImVec2 size;
	ImVec2 position;
	bool hidden;
	std::string ident;
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

	inline void setIdent(std::string ident) {
		this->ident = ident;
	}

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
	std::string text = "";
	bool* checked;

public:

	inline CheckBoxWidget() {
		checked = new bool(false);
	}

	inline ~CheckBoxWidget() {
		delete checked;
	}

	void render();

	void onClick(bool* boolean);

	void setText(std::string text);

	std::string getText();

	void setChecked(bool value);
	bool isChecked();
};

class TextFieldWidget : public Widget {

private:
	std::string placeholder = "";
	char* input;
	int buffer_size;
public:

	inline TextFieldWidget(int bufSize) {
		buffer_size = bufSize;
		input = new char[buffer_size];
		memset(input, 0, buffer_size);

	}

	inline ~TextFieldWidget() {
		delete[] input;
	}

	void render();

	void setPlaceholder(std::string placeholderText);

	char* getInput();
	void setInput(std::string replacement);
};
#include "widget.hpp"

void Widget::setWidth(float width) {
	size.x = width;
}
void Widget::render() {
}
void Widget::setHeight(float height) {
	size.y = height;
}
void Widget::setPosX(float x) {
	position.x = x;
}
void Widget::setPosY(float y) {
	position.y = y;
}

float Widget::getWidth() {
	return size.x;
}
float Widget::getHeight() {
	return size.y;
}
float Widget::getPosX() {
	return position.x;
}
float Widget::getPosY() {
	return position.y;
}

ImVec2 Widget::getPos() {
	return position;
}

void TextWidget::render() {
	ImGui::Text(text.c_str());

}

void TextWidget::setText(std::string text) {
	this->text = text;
}

std::string TextWidget::getText() {
	return text;
}

void ButtonWidget::render() {

	if (ImGui::Button(this->text.c_str(), size)) {

		this->function();

	}

}

void ButtonWidget::onClick(std::function<void()> function) {

	this->function = function;

}

void ButtonWidget::setText(std::string text) {
	this->text = text;
}

std::string ButtonWidget::getText() {
	return text;
}
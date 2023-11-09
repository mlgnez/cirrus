#include "widget.hpp"

void Widget::setWidth(float width) {
	size.x = width;
}
void Widget::setHeight(float height) {
	size.y = height;
}
void Widget::setPosY(float y) {
	position.y = y;
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
float Widget::getPosY() {
	return position.y;
}
float Widget::getPosY() {
	return position.y;
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
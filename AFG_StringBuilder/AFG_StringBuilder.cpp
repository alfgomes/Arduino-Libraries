#include <AFG_StringBuilder.h>

AFG_StringBuilder::AFG_StringBuilder() {
	len = 0;
}

AFG_StringBuilder::AFG_StringBuilder(String text) {
	len = 0;
	lines[len++] = text;
}

void AFG_StringBuilder::add(String text) {
	this->lines[len++] = text;
}

void AFG_StringBuilder::moveToUp(int line, String text) {
	for (int u = line; u <= len; u++) {
		this->lines[u] = this->lines[u+1];
	}
	this->lines[len--] = text;
}

void AFG_StringBuilder::moveToDown(int line, String text) {
	for (int d = len; d >= line; d--) {
		this->lines[d+1] = this->lines[d];
	}
	this->lines[line] = text;
	len++;
}

void AFG_StringBuilder::insert(int line, String text) {
	if (line > len)
		this->lines[len++] = text;
	else {
		moveToDown(line, text);
	}
}

void AFG_StringBuilder::replace(int line, String text) {
	if (line <= len)
		this->lines[line] = text;
	else
		this->lines[len++] = text;
}

void AFG_StringBuilder::remove(int line) {
	moveToUp(line, "");
}

void AFG_StringBuilder::clear() {
	for (int i = 0; i <= len; i++) {
		this->lines[i] = String("");
	}
	this->len = 0;
}

String AFG_StringBuilder::toString() {
	return toString(false);
}

String AFG_StringBuilder::toString(bool CRLF) {
	String ret = "";
	
	for (int i = 0; i <= len; i++) {
		ret += String(this->lines[i]) + (CRLF ? "\n\r" : "");
	}
	
	return ret;
}

void AFG_StringBuilder::print() {
	Serial.print(toString());
}

void AFG_StringBuilder::printLines() {
	for (int i = 0; i <= len; i++) {
		if (i < len)
			Serial.println(this->lines[i]);
		else
			Serial.print(this->lines[i]);
	}
}

int AFG_StringBuilder::linesCount() {
	return len;
}
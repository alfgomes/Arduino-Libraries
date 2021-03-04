#ifndef AFG_STRINGBUILDER_H_INCLUDED
#define AFG_STRINGBUILDER_H_INCLUDED

#include <Arduino.h>
	
class AFG_StringBuilder {
	public:
		AFG_StringBuilder();
		AFG_StringBuilder(String text);
		void add(String text);
		void insert(int line, String text);
		void replace(int line, String text);
		void remove(int line);
		void clear();
		String toString();
		String toString(bool CRLF);
		void print();
		void printLines();
		int linesCount();
	private:
		String lines[255];
		int len;
		void moveToUp(int line, String text);
		void moveToDown(int line, String text);
};

#endif
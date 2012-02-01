#ifndef __SYMBOL
#define __SYMBOL

#include<string>

using namespace std;

class symbol{

	protected:

		string name;

		bool constant;

		string type;

		int line;

		int column;

		bool initialized;

	public:

		string getName();

		bool isConst();

		string getType();

		int getLine();

		int getColumn();

		bool isInitialized();
};

class symbol_int : public symbol {

	private:

		int value;

	public:

		symbol_int(string name, int value);

		int getValue();
};

class symbol_double : public symbol {

	private:

		double value;

	public:

		symbol_double(string name, double value);

		double getValue();
};

class symbol_char : public symbol {

	private:

		char value;

	public:

		symbol_char(string name, char value);

		char getValue();
};

#endif
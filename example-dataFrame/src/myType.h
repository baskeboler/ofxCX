/*
In order to use your own type with a CX_DataFrame you will have to define stream
insertion and stream extraction operators (operator<< and operator>>, respectively)
for your type. In this example, your type has a public data member, an int, and
a private data member, a float.

The end result will be that you can do this:

CX_DataFrame df;
myType mt(15, 1.357);
df("myType", 0) = mt;
myType copy = df("myType", 0);

*/

#include <iostream>
#include <iomanip>

class myType {
public:
	myType(void) {}

	myType(int i_, float f) {
		i = i_;
		_f = f;
	}

	int i;

private:
	
	float _f;

	//By declaring the stream operators as friend, they will have access to private members of the class.
	//Otherwise _f would not be accessible because the stream operators are not member functions
	//of the class.
	friend std::ostream& operator<< (std::ostream& os, const myType& myt);
	friend std::istream& operator>> (std::istream& is, myType& myt);
};

//These function bodies should usually be put into a .cpp file, with only the
//declarations in the header.
inline std::ostream& operator<< (std::ostream& os, const myType& myt) {
	os << myt.i << ", " << myt._f; //Insert the values of myt into the ostream with a comma-space delimiter. Comma-space is standard for oF stuff.
	return os;
}

inline std::istream& operator>> (std::istream& is, myType& myt) {
	//Extract data in the same order that it went in. 
	is >> myt.i; //This means "get an int out of the istream".
	is.ignore(2); //Ignore the next two characters after the integer (the comma and space: ", ")
	is >> myt._f; //Get a float out of the istream.
	return is;
}

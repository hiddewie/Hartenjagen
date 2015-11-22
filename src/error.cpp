#include <string>
#include <sstream>

#include "hearts.h"

using namespace std;

Error::Error(int id, int info) {
	this->id = id;
	this->info = info;
	this->message = "";
}
Error::Error(string message) {
	this->message = message;
	this->id = 0;
	this->info = 0;
}
string Error::getNumberAsString(int number) {
	return static_cast<ostringstream*>(&(ostringstream() << number))->str();
}
int Error::getID() {
	return this->id;
}
int Error::getInfo() {
	return this->info;
}
string Error::getMessage() {
	return this->message;
}

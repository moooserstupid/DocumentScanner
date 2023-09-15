#include "document_scanner.hpp"



namespace ds = document_scanner;
int main() {
	ds::Scanner scanner;

	scanner.ScanDocument("Resources/paper.jpg");
	return -1;
}
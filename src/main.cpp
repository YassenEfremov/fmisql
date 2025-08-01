#include "../include/fmisql.hpp"


int main() {

	fmisql::init();

	fmisql::cli();

	fmisql::deinit();

	return 0;
}

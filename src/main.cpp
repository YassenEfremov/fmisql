#include "cli/cli.hpp"
#include "pager.hpp"

#include <iostream>


int main() {

	fmisql::db_init();

	fmisql::cli();

	fmisql::db_deinit();

	return 0;
}

#include "../include/fmisql.hpp"

#include "btree.hpp"
#include "pager.hpp"
#include "schema.hpp"


namespace fmisql {

void init() {
	Pager::init();
}

void deinit() {
	Pager::deinit();
	BplusTree::delete_all_trees();
}

} // namespace fmisql

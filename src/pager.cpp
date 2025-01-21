#include "pager.hpp"


int Pager::rows_count;
void *Pager::pages[table_max_pages];

// Insert INTO Sample {(1, "Test", 1), (2, "something else", 100)}
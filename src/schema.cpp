#include "schema.hpp"


int Schema::rows_count;
void *Schema::pages[table_max_pages];

// CreateTable Sample (ID:Int, Name:String, Value:Int)
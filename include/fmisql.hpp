#ifndef FMISQL_HPP
#define FMISQL_HPP

#include "../src/cli/cli.hpp"


namespace fmisql {

/**
 * @brief Initialize the database
 */
void init();

/**
 * @brief Deinitialize the database
 */
void deinit();

} // namespace fmisql


#endif // FMISQL_HPP


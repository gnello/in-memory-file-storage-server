
#ifndef GNL_FSS_SERVER_H
#define GNL_FSS_SERVER_H

#include "./gnl_fss_config.h"

/**
 * Start the in memory file storage server. This invocation
 * is blocking, therefore if it returns it means that the server was
 * stopped by a signal or that an error occurred.
 *
 * @param config    The server config object. It contains all the
 *                  options specified into the configuration file.
 *
 * @return          Returns 0 if the server was stopped by a signal,
 *                  -1 if an error occurred.
 */
int gnl_fss_server_start(const struct gnl_fss_config *config);

#endif //GNL_FSS_SERVER_H
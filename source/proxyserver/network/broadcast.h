/*! These functions are used by the proxy server to  announce it
 *  arrival on the network, and its departure from the network
 */
#ifndef BROADCAST_H
#define BROADCAST_H

#include <QString>

void announce(QString proxyName, int port);
void depart(QString proxyName, int port);

#endif
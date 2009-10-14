/*! These functions are used by the proxy server to  announce it
 *  arrival on the network, and its departure from the network
 */
#ifndef BROADCAST_H
#define BROADCAST_H

#include <QString>

void announce(QString proxyName);
void depart(QString proxyName);

#endif
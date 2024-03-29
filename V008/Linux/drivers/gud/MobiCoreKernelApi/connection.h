/** @addtogroup MCD_MCDIMPL_DAEMON_SRV
 * @{
 * @file
 *
 * Connection data.
 *
 * <!-- Copyright Giesecke & Devrient GmbH 2009 - 2012 -->
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <linux/semaphore.h>

#include <stddef.h>
#include <stdbool.h>

#define MAX_PAYLOAD_SIZE 128

struct connection_t {
	struct sock *socketDescriptor; /**< Netlink socket */
	uint32_t sequenceMagic; /**< Random? magic to match requests/answers */

	struct nlmsghdr *dataMsg;
	uint32_t dataLen; /**< How much connection data is left */
	void *dataStart; /**< Start pointer of remaining data */
	struct sk_buff *skb;

	struct mutex dataLock; /**< Data protection lock */
	struct semaphore dataAvailableSem; /**< Data protection semaphore */

	pid_t selfPid; /**< PID address used for local connection */
	pid_t peerPid; /**< Remote PID for connection */

	struct list_head list; /**< The list param for using the kernel lists*/
};

struct connection_t *connection_new(
	void
);

struct connection_t *connection_create(
	int		  socketDescriptor,
	pid_t		dest
);

void connection_cleanup(
	struct connection_t *conn
);

/**
  * Connect to destination.
  *
  * @param Destination pointer.
  * @return true on success.
  */
bool connection_connect(
	struct connection_t *conn,
	pid_t		dest
);


/**
  * Read bytes from the connection.
  *
  * @param buffer	Pointer to destination buffer.
  * @param len	   Number of bytes to read.
  * @return Number of bytes read.
  */
size_t connection_readDataBlock(
	struct connection_t *conn,
	void		 *buffer,
	uint32_t	 len
);
/**
  * Read bytes from the connection.
  *
  * @param buffer	Pointer to destination buffer.
  * @param len	   Number of bytes to read.
  * @param timeout   Timeout in milliseconds
  * @return Number of bytes read.
  * @return -1 if select() failed (returned -1)
  * @return -2 if no data available, i.e. timeout
  */
size_t connection_readData(
	struct connection_t *conn,
	void		 *buffer,
	uint32_t	 len,
	int32_t	  timeout
);

/**
  * Write bytes to the connection.
  *
  * @param buffer	Pointer to source buffer.
  * @param len		Number of bytes to read.
  * @return Number of bytes written.
  */
size_t connection_writeData(
	struct connection_t *conn,
	void		 *buffer,
	uint32_t	  len
);

/**
 * Write bytes to the connection.
 *
 * @param buffer	Pointer to source buffer.
 * @param len		Number of bytes to read.
 * @return Number of bytes written.
 */
int connection_process(
	struct connection_t *conn,
	struct sk_buff *skb
);

#endif /* CONNECTION_H_ */

/** @} */

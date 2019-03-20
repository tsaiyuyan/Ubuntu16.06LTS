/**
 * @file file.h
 * @comment
 * wrap of file descriptor
 *
 * @author niexw
 */

#ifndef _XCOM_FILE_H_
#define _XCOM_FILE_H_

#include <stdio.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <fcntl.h>
#include "exception.h"
#include "buffer.h"

namespace unp
{

/**
 * @class File
 * @comment
 * wrap of file descriptor
 */
class File
{
protected:
	int fd_;

public:
	//
	// construtor and destructor
	//
	File() : fd_(-1) {}
	explicit File(FILE *stream) : fd_(fileno(stream)) {}
	~File()
	{ close(); }

	int getFd() { return fd_; }
	int getFd() const { return fd_; }

	size_t read(char *buf, size_t count) const
	{
		int ret;
		RETRY:
		if ((ret = ::read(fd_, buf, count)) == -1)
		{
			if (errno == EAGAIN)
				goto RETRY;
			else
				throw EXCEPTION();
		}
		return ret;
	}

	size_t write(char *buf, size_t count) const
	{
		int ret;
		RETRY:
		if ((ret = ::write(fd_, buf, count)) == -1)
		{
			if (errno == EAGAIN)
				goto RETRY;
			else
				throw EXCEPTION();
		}
		return ret;
	}

	void close()
	{
		if (fd_ != -1) {
			::close(fd_);
			fd_ = -1;
		}
	}

	void setNonblock()
	{
		int flags = fcntl(fd_, F_GETFL);
		if (flags == -1)
			throw EXCEPTION();
		flags |= O_NONBLOCK;
		flags = fcntl(fd_, F_SETFL, flags);
		if (flags == -1)
			throw EXCEPTION();
	}

	void clrNonblock()
	{
		int flags = fcntl(fd_, F_GETFL);
		if (flags == -1)
			throw EXCEPTION();
		flags &= ~O_NONBLOCK;
		flags = fcntl(fd_, F_SETFL, flags);
		if (flags == -1)
			throw EXCEPTION();
	}

	size_t readv(CircleBuffer &buf)
	{
		int ret;
		RETRY:
		if ((ret = ::readv(fd_, buf.idle_, buf.idlenum_)) == -1)
		{
			if (errno == EAGAIN)
				goto RETRY;
			else
				throw EXCEPTION();
		}
		buf.afterRead(ret);
		return ret;
	}

	size_t writev(CircleBuffer &buf)
	{
		int ret;
		RETRY:
		if ((ret = ::writev(fd_, buf.data_, buf.datanum_)) == -1)
		{
			if (errno == EAGAIN)
				goto RETRY;
			else
				throw EXCEPTION();
		}
		buf.afterWrite(ret);
		return ret;
	}

	void setFlag(int option)
	{
		int flags;
		RETRY:
		flags = fcntl(fd_, F_GETFL);
		if (flags == -1) {
			if (errno == EINTR)
				goto RETRY;
			else
				throw EXCEPTION();
		}
		flags |= option;
		RETRY1:
		int ret = fcntl(fd_, F_SETFL, flags);
		if (ret == -1) {
			if (errno == EINTR)
				goto RETRY1;
			else
				throw EXCEPTION();
		}
	}

	void clrFlag(int option)
	{
		int flags;
		RETRY:
		flags = fcntl(fd_, F_GETFL);
		if (flags == -1) {
			if (errno == EINTR)
				goto RETRY;
			else
				throw EXCEPTION();
		}
		flags &= ~option;
		RETRY1:
		int ret = fcntl(fd_, F_SETFL, flags);
		if (ret == -1) {
			if (errno == EINTR)
				goto RETRY1;
			else
				throw EXCEPTION();
		}
	}

};

/**
 * @class File2
 * @comment
 * wrap of file descriptor
 */
class File2
{
protected:
	int descriptor_;

public:
	File2() : descriptor_(-1)
	{ }
	explicit File2(FILE *stream) : descriptor_(fileno(stream))
	{ }
	explicit File2(File2 &f) : descriptor_(f.descriptor_)
	{ f.descriptor_ = -1; }
	~File2()
	{ close(); }

	int descriptor() { return descriptor_; }

	size_t read(char *buf, size_t count)
	{
		int ret;
		RETRY:
		if ((ret = ::read(descriptor_, buf, count)) == -1)
		{
			if (errno == EAGAIN)
				goto RETRY;
			else
				throw EXCEPTION();
		}
		return ret;
	}

	size_t write(char *buf, size_t count) const
	{
		int ret;
		RETRY:
		if ((ret = ::write(descriptor_, buf, count)) == -1)
		{
			if (errno == EAGAIN)
				goto RETRY;
			else
				throw EXCEPTION();
		}
		return ret;
	}

	void close()
	{
		if (descriptor_ != -1) {
			::close(descriptor_);
			descriptor_ = -1;
		}
	}

	size_t readv(const struct iovec *iov, int cnt)
	{
		int ret;
		RETRY:
		if ((ret = ::readv(descriptor_, iov, cnt)) == -1)
		{
			if (errno == EAGAIN)
				goto RETRY;
			else
				throw EXCEPTION();
		}
		return ret;
	}

	size_t writev(const struct iovec *iov, int cnt)
	{
		int ret;
		RETRY:
		if ((ret = ::writev(descriptor_, iov, cnt)) == -1)
		{
			if (errno == EAGAIN)
				goto RETRY;
			else
				throw EXCEPTION();
		}
		return ret;
	}

	void setControlOption(int option)
	{
		int flags;
		RETRY:
		flags = fcntl(descriptor_, F_GETFL);
		if (flags == -1) {
			if (errno == EINTR)
				goto RETRY;
			else
				throw EXCEPTION();
		}
		flags |= option;
		RETRY1:
		int ret = fcntl(descriptor_, F_SETFL, flags);
		if (ret == -1) {
			if (errno == EINTR)
				goto RETRY1;
			else
				throw EXCEPTION();
		}
	}

	void clearControlOption(int option)
	{
		int flags;
		RETRY:
		flags = fcntl(descriptor_, F_GETFL);
		if (flags == -1) {
			if (errno == EINTR)
				goto RETRY;
			else
				throw EXCEPTION();
		}
		flags &= ~option;
		RETRY1:
		int ret = fcntl(descriptor_, F_SETFL, flags);
		if (ret == -1) {
			if (errno == EINTR)
				goto RETRY1;
			else
				throw EXCEPTION();
		}
	}

	void setNonblock()
	{
		int flags = fcntl(descriptor_, F_GETFL);
		if (flags == -1)
			throw EXCEPTION();
		flags |= O_NONBLOCK;
		flags = fcntl(descriptor_, F_SETFL, flags);
		if (flags == -1)
			throw EXCEPTION();
	}

	void clrNonblock()
	{
		int flags = fcntl(descriptor_, F_GETFL);
		if (flags == -1)
			throw EXCEPTION();
		flags &= ~O_NONBLOCK;
		flags = fcntl(descriptor_, F_SETFL, flags);
		if (flags == -1)
			throw EXCEPTION();
	}


};

}; // namespace unp
#endif // _XCOM_FILE_H_

/**
 * @file epoll.h
 * @comment
 * wrap of epoll
 *
 * @author niexw
 */

#ifndef _UNP_EPOLL_H_
#define _UNP_EPOLL_H_

#include <sys/epoll.h>
#include <assert.h>
#include <map>
#include <strings.h>
#include "file.h"

namespace unp
{

/**
 * @class Epoll
 * @comment
 * wrap of epoll
 */
class Epoll : public File
{
public:
	Epoll() {}
	~Epoll() {}

	struct Event : public epoll_event
	{
		Event() { events = EPOLLERR; data.u64 = 0; }
		Event(unsigned int type, void *magic)
		{ events = type; data.ptr = magic; }
	};

	int create()
	{
		if ((fd_ = epoll_create(1)) == -1)
			throw EXCEPTION();
		return fd_;
	}

	void registerEvent(int fd, Event &event)
	{
		if (epoll_ctl(fd_, EPOLL_CTL_ADD, fd, &event) == -1)
			throw EXCEPTION();
	}

	void modifyEvent(int fd, Event &event)
	{
		if (epoll_ctl(fd_, EPOLL_CTL_MOD, fd, &event) == -1)
			throw EXCEPTION();
	}

	void unregisterEvent(int fd)
	{
		if (epoll_ctl(fd_, EPOLL_CTL_DEL, fd, NULL) == -1)
			throw EXCEPTION();
	}

	int waitEvent(Event *events, int size, int msec)
	{
		int ret;
		assert(events != NULL);
		RETRY:
		if ((ret = epoll_wait(fd_, events, size,
				msec == -1 ? NULL : msec)) == -1)
		{
			if (errno == EINTR)
				goto RETRY;
			else
				throw EXCEPTION();
		}
		return ret;
	}
};

#include <iostream>
using std::cout;
using std::endl;

class Epoll2 : public File
{
public:
	typedef void* (*Callback)(epoll_event &event, void *);

protected:
	struct Event : public epoll_event
	{
		Callback func_;
		void *param_;

		Event() : func_(NULL), param_(NULL)
		{ events = EPOLLERR; data.u64 = 0; }
		Event(unsigned int type) : func_(NULL), param_(NULL)
		{ events = EPOLLERR | type; data.u64 = 0; }
		Event(unsigned int type, Callback func, void *param)
			: func_(func), param_(param)
		{ events = EPOLLERR | type; data.u64 = 0; }
	};
	typedef std::map<int, Event> Events;

	Events events_;
	epoll_event happens_[10];

	int timeout_;
	Callback func_;
	void *param_;

public:
	Epoll2() : timeout_(-1), func_(NULL), param_(NULL)
	{
		assert(sizeof(Events::iterator) == sizeof(void*));
		fd_ = epoll_create(10);
	}

	Epoll2(int msec, Callback func) : timeout_(msec), func_(NULL), param_(NULL)
	{
		 assert(sizeof(Events::iterator) == sizeof(void*));
		 fd_ = epoll_create(10);
	}

	~Epoll2()
	{ }

	void registerEvent(int fd, int option, Callback func, void *param)
	{
		Event event(option, func, param);
		std::pair<Events::iterator, bool> ret =
				events_.insert(std::pair<int, Event>(fd, event));
		//ret.first->second.data.ptr = (void *)ret.first._M_node;
		bcopy(&ret.first, &ret.first->second.data.ptr, sizeof(void*));
		if (epoll_ctl(fd_, EPOLL_CTL_ADD, fd, &ret.first->second) == -1)
			throw EXCEPTION();
	}

	void setEventOption(int fd, int option)
	{
		Event *p = &events_[fd];
		p->events = option | EPOLLERR;
		if (epoll_ctl(fd_, EPOLL_CTL_MOD, fd, p) == -1)
			throw EXCEPTION();
	}

	void setEventOption(int fd, int option, Callback func, void *param)
	{
		Event *p = &events_[fd];
		p->events = option | EPOLLERR;
		p->func_ = func;
		p->param_ = param;
		if (epoll_ctl(fd_, EPOLL_CTL_MOD, fd, p) == -1)
			throw EXCEPTION();
	}

	void addEventOption(int fd, int option)
	{
		Event *p = &events_[fd];
		p->events |= option;
		if (epoll_ctl(fd_, EPOLL_CTL_MOD, fd, p) == -1)
			throw EXCEPTION();
	}

	void addEventOption(int fd, int option, Callback func, void *param)
	{
		Event *p = &events_[fd];
		p->events |= option;
		p->func_ = func;
		p->param_ = param;
		if (epoll_ctl(fd_, EPOLL_CTL_MOD, fd, p) == -1)
			throw EXCEPTION();
	}

	void clrEventOption(int fd, int option, Callback func, void *param)
	{
		Event *p = &events_[fd];
		p->events &= ~option;
		p->func_ = func;
		p->param_ = param;
		if (epoll_ctl(fd_, EPOLL_CTL_MOD, fd, p) == -1)
			throw EXCEPTION();
	}

	void clrEventOption(int fd, int option)
	{
		Event *p = &events_[fd];
		p->events &= ~option;
		if (epoll_ctl(fd_, EPOLL_CTL_MOD, fd, p) == -1)
			throw EXCEPTION();
	}

	void unregisterEvent(int fd)
	{
		events_.erase(fd);
		if (epoll_ctl(fd_, EPOLL_CTL_DEL, fd, NULL) == -1)
			throw EXCEPTION();
	}

	void setTimeout(int msec, Callback func, void *param)
	{
		timeout_ = msec;
		func_ = func;
		param_ = param;
	}

	bool run()
	{
		int ret;
		RETRY:
		if ((ret = epoll_wait(fd_, happens_, 10, timeout_)) == -1)
		{
			if (errno == EINTR)
				goto RETRY;
			else
				throw EXCEPTION();
		}

		for (int i = 0; i < ret; ++i)
		{
			Events::iterator it;
			bcopy(&happens_[i].data.ptr, &it, sizeof(void *));
			//it._M_node = (std::_Rb_tree_node_base*)happens_[i].data.ptr;
			if (it->second.func_ != NULL)
				it->second.func_(happens_[i], it->second.param_);
			if (happens_[i].events & EPOLLERR)
				throw EXCEPTION();
		}

		if (ret == 0 && func_ != NULL)
			func_(happens_[0], param_);

		return !events_.empty();
	}

};


}; // namespace unp
#endif /* _UNP_EPOLL_H_ */

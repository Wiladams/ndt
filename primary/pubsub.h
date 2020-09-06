#pragma once

#include <memory>
#include <deque>
#include <functional>

/*
	A topic is a single thing to subscribe to.  A "publisher" might
	be composed of several topics.

	Doing it this way allows for different forms of composition and
	general topic management.

	T - The event payload

	The subscriber is a functor
*/
template <typename T>
class Topic
{
public:
	using Subscriber = std::function<void(const Topic<T>& p, const T m)>;

	std::deque<Subscriber> fSubscribers;


public:
	void notify(const T m)
	{
		for (auto & it : fSubscribers) {
			it(*this, m);
		}
	}

	void subscribe(Subscriber s)
	{
		fSubscribers.push_back(s);
	}

	void unsubscribe(Subscriber s)
	{
		// find the subscriber that matches
	}
};
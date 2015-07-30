#pragma once

#include <ppl.h>
#include <ppltasks.h>
#include <memory>
#include <future>

template<class T>
class AsyncResource {
public:
	template<class C, class A1>
	AsyncResource(C callable, A1 arg1)
		: resourceLoaded(false) 
	{
		this->task = concurrency::create_task([=]() {
			return callable(arg1);
		});
	}

	template<class C, class A1, class A2>
	AsyncResource(C callable, A1 arg1, A2 arg2)
		: resourceLoaded(false) 
	{
		this->task = concurrency::create_task([=]() {
			return callable(arg1, arg2);
		});
	}

	T Get() {
		concurrency::critical_section::scoped_lock lk(this->cs);

		if (!this->resourceLoaded) {
			this->resource = this->task.get();
			this->resourceLoaded = true;
		}

		return this->resource;
	}
private:
	concurrency::critical_section cs;
	concurrency::task<T> task;
	T resource;
	bool resourceLoaded;
};
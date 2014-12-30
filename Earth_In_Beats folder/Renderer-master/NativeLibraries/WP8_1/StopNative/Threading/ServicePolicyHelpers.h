#pragma once

#include <memory>

class StdDerefPolicy{
public:
	template<class V>
	static V *Deref(V &v){
		return &v;
	}

	template<class V>
	static V *Deref(V *v){
		return v;
	}

	template<class V, class D>
	static V *Deref(std::unique_ptr<V, D> &v){
		return v.get();
	}

	template<class V>
	static V *Deref(std::shared_ptr<V> &v){
		return v.get();
	}
};

class StdCopyPolicy{
public:
	template<class V>
	static V &Copy(V &v){
		return &v;
	}

	template<class V>
	static V Copy(V *v){
		return v;
	}

	template<class V, class D>
	static std::unique_ptr<V, D> &&Copy(std::unique_ptr<V, D> &v){
		return std::move(v);
	}

	template<class V>
	static std::shared_ptr<V> &Copy(std::shared_ptr<V> &v){
		return v;
	}
};
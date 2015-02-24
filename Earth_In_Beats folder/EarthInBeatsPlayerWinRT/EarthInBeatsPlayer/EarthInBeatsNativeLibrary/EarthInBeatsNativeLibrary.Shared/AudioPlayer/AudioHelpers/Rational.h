#pragma once

#include <cstdint>

template<class T>
class Rational{
public:
	static const Rational HNS;
	static const Rational MS;
	static const Rational SEC;

	T num;
	T den;

	Rational() : num(1), den(1){
	}

	Rational(T num, T den){
		this->num = num;
		this->den = den;
	}

	void SetRational(T num, T den){
		this->num = num;
		this->den = den;
	}
};

template<class T>
const Rational<T> Rational<T>::HNS(1, 10000000);

template<class T>
const Rational<T> Rational<T>::MS(1, 1000);

template<class T>
const Rational<T> Rational<T>::SEC(1, 1);

template<class T, class Impl>
class RationalValueData{
public:
	typedef T Type;
	typedef Rational<T> Unit;

	const Rational<T> &GetUnit() const{
		return static_cast<const Impl *>(this)->GetUnit();
	}

	void SetUnit(const Rational<T> &v){
		static_cast<Impl *>(this)->SetUnit(v);
	}

	const T &GetValue() const{
		return static_cast<const Impl *>(this)->GetValue();
	}

	void SetValue(const T &v){
		static_cast<Impl *>(this)->SetValue(v);
	}
};

template<class T, class Impl>
class RationalValueDataExtended : public RationalValueData < T, Impl > {
};

template<class Impl>
class RationalValueDataExtended<int64_t, Impl> : public RationalValueData < int64_t, Impl > {
public:
	void ConvertCeil(const Rational<int64_t> &r){
		auto a = this->GetValue() * this->GetUnit().num * r.den;
		auto b = this->GetUnit().den * r.num;

		this->SetValue((a / b) + !!(a % b));
		this->SetUnit(r);
	}

	Impl ConvertCeil_cr(const Rational<int64_t> &r) const{
		Impl res;
		RationalValueData<int64_t, Impl> *resPtr = &res;

		auto a = this->GetValue() * this->GetUnit().num * r.den;
		auto b = this->GetUnit().den * r.num;

		resPtr->SetValue((a / b) + !!(a % b));
		resPtr->SetUnit(r);

		return res;
	}
};

template<class T>
class RationalValue : public RationalValueDataExtended < T, RationalValue<T> > {
public:
	Rational<T> unit;
	T value;

	RationalValue()
		: value(0), unit(Unit::MS){
	}

	RationalValue(const T &msTime)
		: value(msTime), unit(Unit::MS){
	}

	RationalValue(const T &value, const Rational<T> &unit)
		: value(value), unit(unit){
	}

	const Rational<T> &GetUnit() const{
		return this->unit;
	}

	void SetUnit(const Rational<T> &v){
		this->unit = v;
	}

	const T &GetValue() const{
		return this->value;
	}

	void SetValue(const T &v){
		this->value = v;
	}

	void SetRational(Rational<T> inputUnit){
		this->unit = inputUnit;
	}

	void Convert(const Rational<T> &r){
		this->value = (this->value * this->unit.num * r.den) / (this->unit.den * r.num);
		this->unit = r;
	}

	RationalValue Convert_cr(const Rational<T> &r) const{
		RationalValue res;
		res.value = (this->value * this->unit.num * r.den) / (this->unit.den * r.num);
		res.unit = r;

		return res;
	}
};

typedef RationalValue<int64_t> Int64Rational;
typedef RationalValue<double>  DoubleRational;
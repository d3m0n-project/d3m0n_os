#ifndef VECTOR_HPP
#define VECTOR_HPP

#include "memory.h"
#include "stdlib.h"

#ifdef __cplusplus
template <typename T>
class Vector
{
public:
	using size_type = size_t;

private:
	T*		data_;
	size_type size_;
	size_type capacity_;

	static T*	move_construct(T* dst, T* src)
	{
		return ::new(static_cast<void*>(dst))T(static_cast<T&&>(*src));
	}

public:
	Vector() : data_(nullptr), size_(0), capacity_(0) {}

	~Vector()
	{
		clear();
		free(data_);
	}

	Vector(const Vector&) = delete;
	Vector& operator=(const Vector&) = delete;

	Vector(Vector&& other) noexcept : data_(other.data_), size_(other.size_), capacity_(other.capacity_)
	{
		other.data_ = nullptr;
		other.size_ = 0;
		other.capacity_ = 0;
	}

	Vector& operator=(Vector&& other) noexcept
	{
		if (this != &other)
		{
			clear();
			free(data_);

			data_ = other.data_;
			size_ = other.size_;
			capacity_ = other.capacity_;

			other.data_ = nullptr;
			other.size_ = 0;
			other.capacity_ = 0;
		}
		return *this;
	}

	bool	reserve(size_type new_capacity)
	{
		if (new_capacity <= capacity_)
			return true;

		if (new_capacity > static_cast<size_type>(-1) / sizeof(T))
			return false;

		T* new_data = static_cast<T*>(
			malloc(new_capacity * sizeof(T)));

		if (new_data == nullptr)
			return false;

		for (size_type i = 0; i < size_; ++i)
			move_construct(new_data + i, data_ + i);

		for (size_type i = 0; i < size_; ++i)
			data_[i].~T();

		free(data_);

		data_ = new_data;
		capacity_ = new_capacity;
		return true;
	}

	bool push_back(const T& value)
	{
		if (size_ == capacity_)
		{
			size_type next = capacity_ ? capacity_ * 2 : 4;

			if (next < capacity_ || next > static_cast<size_type>(-1) / sizeof(T))
				return false;

			if (!reserve(next))
				return false;
		}

		::new (static_cast<void*>(data_ + size_)) T(value);
		++size_;
		return true;
	}

	bool	push_back(T&& value)
	{
		if (size_ == capacity_)
		{
			size_type next = capacity_ ? capacity_ * 2 : 4;

			if (next < capacity_ || next > static_cast<size_type>(-1) / sizeof(T))
				return false;

			if (!reserve(next))
				return false;
		}

		::new (static_cast<void*>(data_ + size_))
			T(static_cast<T&&>(value));

		++size_;
		return true;
	}

	void pop_back()
	{
		if (size_ != 0)
		{
			--size_;
			data_[size_].~T();
		}
	}

	void clear()
	{
		while (size_ != 0)
		{
			--size_;
			data_[size_].~T();
		}
	}

	T& operator[](size_type index)
	{
		return data_[index];
	}

	const T& operator[](size_type index) const
	{
		return data_[index];
	}

	T* data()					{ return data_; }
	const T* data() const		{ return data_; }

	size_type size() const		{ return size_; }
	size_type capacity() const	{ return capacity_; }
	bool empty() const			{ return size_ == 0; }
};
#endif

#endif
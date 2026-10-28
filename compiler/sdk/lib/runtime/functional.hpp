#ifndef RUNTIME_FUNCTIONAL_HPP
#define RUNTIME_FUNCTIONAL_HPP

namespace std
{
	template <typename Signature>
	class function;

	template <typename Return, typename Argument>
	class function<Return(Argument *)>
	{
	private:
		struct callable_base
		{
			virtual Return invoke(Argument *argument) = 0;
			virtual callable_base *clone() const = 0;
			virtual ~callable_base() {}
		};

		template <typename Callable>
		struct callable_holder : callable_base
		{
			Callable callable;

			explicit callable_holder(const Callable &value) : callable(value) {}

			Return invoke(Argument *argument) override
			{
				return callable(argument);
			}

			callable_base *clone() const override
			{
				return new callable_holder<Callable>(callable);
			}
		};

		callable_base *target_;

	public:
		function() : target_(nullptr) {}

		template <typename Callable>
		function(const Callable &callable)
			: target_(new callable_holder<Callable>(callable)) {}

		function(const function &other)
			: target_(other.target_ ? other.target_->clone() : nullptr) {}

		function(function &&other) noexcept : target_(other.target_)
		{
			other.target_ = nullptr;
		}

		~function()
		{
			delete target_;
		}

		function &operator=(const function &other)
		{
			if (this != &other)
			{
				callable_base *copy = other.target_ ? other.target_->clone() : nullptr;
				delete target_;
				target_ = copy;
			}
			return *this;
		}

		function &operator=(function &&other) noexcept
		{
			if (this != &other)
			{
				delete target_;
				target_ = other.target_;
				other.target_ = nullptr;
			}
			return *this;
		}

		template <typename Callable>
		function &operator=(const Callable &callable)
		{
			function replacement(callable);
			*this = static_cast<function &&>(replacement);
			return *this;
		}

		bool empty() const
		{
			return target_ == nullptr;
		}

		Return operator()(Argument *argument) const
		{
			if (target_)
				return target_->invoke(argument);
			return Return();
		}
	};

	template <typename Return, typename FirstArgument, typename SecondArgument>
	class function<Return(FirstArgument *, SecondArgument *)>
	{
	private:
		struct callable_base
		{
			virtual Return invoke(FirstArgument *first, SecondArgument *second) = 0;
			virtual callable_base *clone() const = 0;
			virtual ~callable_base() {}
		};

		template <typename Callable>
		struct callable_holder : callable_base
		{
			Callable callable;

			explicit callable_holder(const Callable &value) : callable(value) {}

			Return invoke(FirstArgument *first, SecondArgument *second) override
			{
				return callable(first, second);
			}

			callable_base *clone() const override
			{
				return new callable_holder<Callable>(callable);
			}
		};

		callable_base *target_;

	public:
		function() : target_(nullptr) {}

		template <typename Callable>
		function(const Callable &callable)
			: target_(new callable_holder<Callable>(callable)) {}

		function(const function &other)
			: target_(other.target_ ? other.target_->clone() : nullptr) {}

		function(function &&other) noexcept : target_(other.target_)
		{
			other.target_ = nullptr;
		}

		~function()
		{
			delete target_;
		}

		function &operator=(const function &other)
		{
			if (this != &other)
			{
				callable_base *copy = other.target_ ? other.target_->clone() : nullptr;
				delete target_;
				target_ = copy;
			}
			return *this;
		}

		function &operator=(function &&other) noexcept
		{
			if (this != &other)
			{
				delete target_;
				target_ = other.target_;
				other.target_ = nullptr;
			}
			return *this;
		}

		template <typename Callable>
		function &operator=(const Callable &callable)
		{
			function replacement(callable);
			*this = static_cast<function &&>(replacement);
			return *this;
		}

		bool empty() const
		{
			return target_ == nullptr;
		}

		Return operator()(FirstArgument *first, SecondArgument *second) const
		{
			if (target_)
				return target_->invoke(first, second);
			return Return();
		}
	};
}

#endif

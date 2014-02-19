#pragma once

namespace Answer
{
	template<typename T>
	class Singleton
	{
	public:
		static T& instance()
		{
			static T t;
			return t;
		}

	private:
		Singleton();
		~Singleton();
		Singleton(const Singleton&);
		Singleton& operator=(const Singleton&);
	};
}


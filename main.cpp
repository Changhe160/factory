template<typename Base>
struct factory
{
	template<typename T>
	struct register_t
	{
		register_t(const std::string& key)
		{
			factory::get().map_.emplace(key, []() { return new T(); });
		}

		template<typename... Args>
		register_t(const std::string& key, Args&&... args)
		{
			factory::get().map_.emplace(key, [&]() { return new T(std::forward<Args>(args)...); });
		}
	};

	//template<typename... Args>
	static Base* produce(const std::string& key)
	{
		if (map_.find(key) == map_.end())
			throw std::invalid_argument("the message key is not exist!");

		return map_[key]();
	}

	template<typename... Args>
	static std::unique_ptr<Base> produce_unique(const std::string& key, Args &&...args)
	{
		return std::unique_ptr<Base>(produce(key,std::forward<Args>(args)...));
	}

	template<typename... Args>
	static std::shared_ptr<Base> produce_shared(const std::string& key, Args &&...args)
	{
		return std::shared_ptr<Base>(produce(key, std::forward<Args>(args)...));
	}

private:
	factory() {};
	factory(const factory&) = delete;
	factory(factory&&) = delete;

	static factory& get()
	{
		static factory instance;
		return instance;
	}
	
	static std::map<std::string, std::function<Base*()>> map_;
};

template<typename Base> 
std::map<std::string, std::function<Base*()>> factory<Base>::map_;

#define REGISTER_VNAME(T) reg_msg_##T##_
#define REGISTER(Base,Derived, key, ...) static factory<Base>::register_t<Derived> REGISTER_VNAME(Derived)(key, ##__VA_ARGS__);


int main() 
{

	REGISTER(Base, A, "A", 1,2);

	factory<Base>::produce("A");

    return 0;
}
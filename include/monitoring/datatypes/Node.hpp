#ifndef NODE_HPP
#define NODE_HPP

#include <string>
#include <utility>

class Node {
public:

	Node(const std::pair<uint64_t, std::string> hardware, 
	     const std::pair<uint64_t, std::string> software,
	     const std::pair<uint64_t, std::string> instance)
		: hardware_(hardware), software_(software), instance_(instance)
	{}


	inline uint64_t hwid() const
	{
		return hardware_.first;
	}
	
	
	inline std::string hwdesc() const
	{
		return hardware_.second;
	}
	
	
	inline uint64_t swid() const
	{
		return software_.first;
	}

	
	inline std::string swdesc() const
	{
		return software_.second;
	}

	
	inline uint64_t inid() const
	{
		return instance_.first;
	}

	inline std::string indesc() const
	{
		return instance_.second;
	}

	
private:
	std::pair<uint64_t, std::string> hardware_;
	std::pair<uint64_t, std::string> software_;
	std::pair<uint64_t, std::string> instance_;
};

#endif

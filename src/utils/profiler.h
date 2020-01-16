#pragma once
#include <string>
#include <vector>
#include <queue>

#define PROFILER_IS_ACTIVE true

using time_stamp = unsigned long long;
struct profiler_node;

class profiler_system
{
	// Class Properties
public:
	static profiler_system & get_instance();
private:
	profiler_system();
	profiler_system(const profiler_system &) = delete;
	profiler_system& operator=(const profiler_system &) = delete;
	~profiler_system();
	
	// Functions
public:
	static time_stamp read_time();
	void start_frame(const std::string & title);
	void end_frame();
	const std::vector<const profiler_node*>& get_frames() { return m_frames; }
	
	// Data
public:
	bool m_frame_active{ false };
private:
	profiler_node * m_current{nullptr};
	std::vector<const profiler_node*> m_frames{};
	time_stamp m_start_time{};
	
	friend struct profiler_node;
	friend class profiler_scope;
};


// Local Node Instantiation
struct profiler_node
{
	profiler_node() = default;
	profiler_node(const profiler_node &) = default;
	profiler_node(const std::string&);
	~profiler_node();

	const std::string m_title;
	std::vector<profiler_node*> m_childs{};
	time_stamp m_time{0u};
	unsigned m_calls{0u};
};


// Public Node Instantiation
class profiler_scope
{
	profiler_node * m_last_parent;
	time_stamp m_start_time;
	size_t m_idx;

public:
	profiler_scope(const std::string&);
	~profiler_scope();
};


// Public Interface
#if PROFILER_IS_ACTIVE
#define PROFILE(title)		profiler_scope profiler_scope_instance {title}
#define START_FRAME(title)	profiler_system::get_instance().start_frame(title)
#define END_FRAME()			profiler_system::get_instance().end_frame()
#define GET_PROFILER_DATA()	profiler_system::get_instance().get_frames()
#ifdef _MSC_VER
#pragma warning (disable: 4456)
#endif
#else
#define PROFILE(title)
#define START_FRAME(title)
#define END_FRAME()
#define GET_PROFILER_DATA() nullptr
#endif 
/**
* @file		profiler.cpp
* @date 	03/31/2019
* @author	Gabriel Mañeru
* @par		Login: gabriel.m
* @par		Course: CS315
* @par		Assignment #3
* @brief 	Implementation of a profiler
*/
#include "profiler.h"
#include <assert.h>
#include <intrin.h>

profiler_system* profiler = &profiler_system::get_instance();

profiler_system & profiler_system::get_instance()
{
	static profiler_system unique;
	return unique;
}
profiler_system::profiler_system()
{
	// Warm Up
	__rdtsc();
	__rdtsc();
	__rdtsc();
}
profiler_system::~profiler_system()
{
	// Clean childs
	for (auto& f : m_frames)
		delete f;
}

time_stamp profiler_system::read_time()
{
	return __rdtsc();
}
void profiler_system::start_frame(const std::string & title)
{
	assert(m_frame_active == false && m_current == nullptr);

	//start a new frame sequence
	m_frame_active = true;
	m_current = new profiler_node{title};
	m_start_time = read_time();
}
void profiler_system::end_frame()
{
	time_stamp end_time = read_time();
	assert(m_frame_active == true);

	//end current frame scope
	m_current->m_time += end_time - m_start_time;
	m_current->m_calls++;
	m_frames.push_back(m_current);
	m_current = nullptr;
	m_frame_active = false;
}

profiler_node::profiler_node(const std::string & title)
	: m_title{ title }, m_childs{}, m_time{ 0u }, m_calls{ 0u } {}
profiler_node::~profiler_node()
{
	// Clean Childs
	for (auto& c : m_childs)
		delete c;
}

profiler_scope::profiler_scope(const std::string & title)
{
	assert(profiler->m_frame_active == true);

	// store previous scope
	m_last_parent = profiler->m_current;

	// seach for child
	for (size_t i = 0; i < m_last_parent->m_childs.size()+1; i++)
	{
		// search in the list
		if (i < m_last_parent->m_childs.size())
		{
			if (m_last_parent->m_childs[i]->m_title == title)
			{
				m_idx =  i;
				break;
			}
		}

		// create a new one
		else
		{
			m_idx = m_last_parent->m_childs.size();
			m_last_parent->m_childs.push_back(new profiler_node{ title });
			break;
		}
	}

	// set new scope
	profiler->m_current = m_last_parent->m_childs[m_idx];
	m_start_time = profiler->read_time();
}
profiler_scope::~profiler_scope()
{
	time_stamp end_time = profiler->read_time();
	assert(profiler->m_frame_active == true);
	
	// end node scope
	profiler_node * node = m_last_parent->m_childs[m_idx];
	assert(profiler->m_current == node);
	node->m_time += end_time - m_start_time;
	node->m_calls++;

	// restore previous scope
	profiler->m_current = m_last_parent;
}

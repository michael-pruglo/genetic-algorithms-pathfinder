#pragma once

#include <algorithm>
#include <queue>
#include <vector>
#include <functional>
#include "Field.hpp"
#include "Path.hpp"

class Bot
{
	friend class Renderer;

protected:
	Path path;
public:
	void makeStep();
	void reset();
	virtual std::pair<int, int> findPath(const Field& field, int startX, int startY, int goalX, int goalY) = 0;
	Path getPath() { return path; }

protected:
	template<typename T, typename priority_t>
	struct PriorityQueue
	{
		typedef std::pair<priority_t, T> PQElement;
		std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> elements;

		inline bool			empty() const { return elements.empty(); }
		inline int			size() const { return elements.size(); }
		inline void			put(T item, priority_t priority) { elements.emplace(priority, item); }
		inline priority_t	bestPriority() { return elements.top().first; }
		inline T			get()
		{
			T best_item = elements.top().second;
			elements.pop();
			return best_item;
		}
	};
};


#pragma once

#include "Bot.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <unordered_map>
#include <unordered_set>

class GeneticBot :
	public Bot
{
public:
	std::pair<int, int> findPath(const Field& field, int startX, int startY, int goalX, int goalY);

private:
	typedef Field::Location Location;
	typedef Field::LocationHasher LocationHasher;

private:
	const double
		CROSSOVER_RATE = 0.5,
		MUTATION_SEVERITY = 0.4, //how many genes in chromosome mutate
		STOP_DIVERSITY_COEF = 1.001, //relation of the highest to the smallest fitness in the group
		PATTERN_RECOGNITION_DISTANCE_LEFT = 0.6, //which part of the map left to begin pattern recognition
		PATTERN_RECOGNITION_REUSED_PERCENT = 0.45; //how many previous chromosomes were legal in the new content
	const int
		POPULATION_SIZE = 16,
		GENERATIONS = 4,
		EXPECTED_POOL_SIZE = POPULATION_SIZE
			+ int(POPULATION_SIZE*(POPULATION_SIZE - 1) / 2 * CROSSOVER_RATE)
			+ POPULATION_SIZE;

	friend struct Chromosome;
	struct Chromosome
	{
		static int legal, illegal;
		const int DM_LOW = 1, DM_HIGH = 10;

		int dm1, dm2, dm3, dm4;
		int m1, m2, m3, m4;
		double fitness;

		Chromosome()
		{
			dm1 = randInt(DM_LOW, DM_HIGH);
			dm2 = randInt(DM_LOW, DM_HIGH);
			dm3 = randInt(DM_LOW, DM_HIGH);
			dm4 = randInt(DM_LOW, DM_HIGH);
			m1 = randInt(-1, 1);
			m2 = randInt(-1, 1);
			m3 = randInt(-1, 1);
			m4 = randInt(-1, 1);
			makeLegal();
		}
		void operator= (const Chromosome& ch)
		{
			dm1 = ch.dm1;
			dm2 = ch.dm2;
			dm3 = ch.dm3;
			dm4 = ch.dm4;
			m1 = ch.m1;
			m2 = ch.m2;
			m3 = ch.m3;
			m4 = ch.m4;
			fitness = ch.fitness;
		}

		bool operator< (const Chromosome& comp) const { return fitness < comp.fitness; }
		inline int moved1() { return dm1 * m1; }
		inline int moved2() { return dm2 * m2; }
		inline int moved3() { return dm3 * m3; }
		inline int moved4() { return dm4 * m4; }
		inline int distance() { return 1; }

		void makeLegal();
		bool isLegal();
		void adapt(const Field& field, const Location& location, const std::unordered_set<Location, LocationHasher>& came_from);
	};
	std::pair<Chromosome, Chromosome> crossover(Chromosome parent1, Chromosome parent2);
	Chromosome mutate(Chromosome original);
	static void show(const Chromosome& c);
	Location finish(Chromosome c, Location start)
	{
		return{
			start.first + c.moved2() + c.moved4(),
			start.second + c.moved1() + c.moved3()
		};
	}
	inline double diversity(const std::vector<Chromosome>& population) const { if (population.size() == 0) return 0; return population.front().fitness / population.back().fitness; }
	void removeIllegal(std::vector<Chromosome>& v, Location current);
	
	//inline static double shorten(double part, double whole, int precision) { return 1.0*int(part / whole*precision) / precision; }
	inline static bool probability(double value) { return 5 % 100 < 100 * value; }
	inline static int randInt(int low, int high) { return 5 % (high - low + 1) + low; }
	//inline static double randDouble(double low, double high, int precision) { return 1.0*randInt(low*precision, high*precision) / precision; }
};


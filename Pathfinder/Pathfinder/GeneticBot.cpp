#include "GeneticBot.hpp"
#include <algorithm>
#include <queue>

#include <iostream>
#include <iomanip>

std::pair<int, int> GeneticBot::findPath(const Field& field, int startX, int startY, int goalX, int goalY)
{
	Location start = { startX, startY }, goal = { goalX, goalY };
	PriorityQueue<Location, int> frontier;
	std::unordered_map<Location, Location, LocationHasher> came_from;
	came_from[start] = start;
	std::unordered_set<Location, LocationHasher> closedList;
	closedList.insert(start);

	int maxFSize = 0, maxSpeciesSize = 0;
	
	Location current = start;
	int totalManhattanDistance = field.heuristic(start, goal);;
	int currentPriority = totalManhattanDistance;
	frontier.put(current, currentPriority);
	srand(time(NULL));
	std::vector<Chromosome> population(POPULATION_SIZE);
	double diversityCoef = STOP_DIVERSITY_COEF+1;
	const int ROOF = (field.columnsAmount() + field.rowsAmount()) * 3;
	bool isPatterned = true;
	int reused = 0, total = 0;
	while (!frontier.empty())
	{
		//std::cout << "CURRENT: " << current.first << ";" << current.second << " came_from:(" << came_from[current].first << ";" << came_from[current].second << ")\n";
		if (currentPriority < frontier.bestPriority() && isPatterned)
		{
			// GA
			
			//validate & adapt parents
			//std::cout << "[]PARENTS:\n";  for (auto&x : population) { show(x); std::cout << "\n"; }
			for (auto& parent : population)
				parent.adapt(field, current, closedList);
			reused += std::count_if(population.begin(), population.end(), [](Chromosome c) {return c.isLegal(); });
			total += POPULATION_SIZE;
			//std::cout << "[---------------------------------------]REUSED: " << 1.0*reused / total << "\n";
			if (1.0 * field.heuristic(current, goal) / totalManhattanDistance < PATTERN_RECOGNITION_DISTANCE_LEFT
				&& 1.0 * reused / total < PATTERN_RECOGNITION_REUSED_PERCENT)
				isPatterned = false;
			//std::cout << "[]ADAPTED:\n";  for (auto&x : population) if (x.isLegal()) { show(x); std::cout << "\n"; }

			for (int i = 0; i < GENERATIONS && diversityCoef>STOP_DIVERSITY_COEF; ++i)
			{
				if (population.size() < POPULATION_SIZE)
				{
					population.resize(POPULATION_SIZE);
					for (auto& parent : population)
						parent.adapt(field, current, closedList);
				}

				std::vector<Chromosome> species(population);
				species.reserve(EXPECTED_POOL_SIZE);
				//generate normal kids
				for (int p1 = 0; p1 < POPULATION_SIZE; ++p1)
					for (int p2 = p1+1; p2 < POPULATION_SIZE; ++p2)
						if (probability(CROSSOVER_RATE))
						{
							auto children = crossover(species[p1], species[p2]);
							species.push_back(children.first);
							species.push_back(children.second);
						}
				//generate mutated kids
				for (int p = 0; p < POPULATION_SIZE; ++p)
						species.push_back(mutate(species[p]));
				//
				if (maxSpeciesSize < species.size())
					maxSpeciesSize = species.size();
				//validate & adapt kids
				//std::cout << "[]ORIGINAL:\n";  for (auto&x : species) { show(x); std::cout << "\n"; }
				for (auto it = species.begin() + POPULATION_SIZE; it < species.end(); ++it)
					it->adapt(field, current, closedList);
				removeIllegal(species, current);
				//std::cout << "[]REMOVE:\n"; for (auto&x : species) { show(x); std::cout << "\n"; }
				//calculate fitness
				for (auto& chromosome : species)
				{
					Location fin = finish(chromosome, current);
					chromosome.fitness = ROOF - field.heuristic(fin, goal) - 0.1*chromosome.distance();
				}
				//std::cout << "ROOF: " << ROOF << "\n";
				//std::cout << "Population: "; for (auto& chromosome : species) std::cout << "\t" << chromosome.fitness; std::cout << "\n";
				//choose best POPULATION_SIZE chromosomes
				std::sort(species.rbegin(), species.rend());
				//std::cout << "Sorted:\n"; for (auto& chromosome : species) { show(chromosome);  std::cout << "\t" << chromosome.fitness << "\n"; } std::cout << "\n";
				if (species.size() > POPULATION_SIZE)
					species.erase(species.begin() + POPULATION_SIZE, species.end());
				population = species;
				//std::cout << "Best:       "; for (auto& chromosome : population) { show(chromosome);  std::cout << "\t" << chromosome.fitness << "\n"; } std::cout << "\n";
				diversityCoef = diversity(species);
				//std::cout << "\t\t\t\t\t\tDiversity coef: " << diversityCoef <<"\n";
			}
			//std::cout << "Result:\n"; for (auto& chromosome : population) { show(chromosome);  std::cout << "\t" << chromosome.fitness << "\n"; } std::cout << "\n";
			//std::cout << "==================================================================================================================================\n";

			//if there is a valid path and fitness < frontier.bestPriority()
			if (diversityCoef <= STOP_DIVERSITY_COEF)
				removeIllegal(population, current);
			if (population.size())
			{
				Chromosome winner = population[0];
				int winnerPriority = field.heuristic(finish(winner, current), goal);
				if (winnerPriority < frontier.bestPriority())
				{
					//std::cout << "******************WE HAVE A WINNER*************** "; show(winner); std::cout << "\n";
					//std::cout << "From (" << current.first << ";" << current.second;
					//move, adding all to closed list
					for (int i = 0; i < std::abs(winner.moved1()); ++i)
					{
						Location next = { current.first, current.second + winner.m1 };
						came_from[next] = current;
						closedList.insert(current);
						current = next;
					}
					//std::cout << ") TO (" << current.first << ";" << current.second;
					for (int i = 0; i < std::abs(winner.moved2()); ++i)
					{
						Location next = { current.first + winner.m2, current.second };
						came_from[next] = current;
						closedList.insert(current);
						current = next;
					}
					//std::cout << ") TO (" << current.first << ";" << current.second;
					for (int i = 0; i < std::abs(winner.moved3()); ++i)
					{
						Location next = { current.first, current.second + winner.m3 };
						came_from[next] = current;
						closedList.insert(current);
						current = next;
					}
					//std::cout << ") TO (" << current.first << ";" << current.second;
					for (int i = 0; i < std::abs(winner.moved4()); ++i)
					{
						Location next = { current.first + winner.m4, current.second };
						came_from[next] = current;
						closedList.insert(current);
						current = next;
					}
					//std::cout << ") TO (" << current.first << ";" << current.second << ")\n";
					//add last point to frontier (open list)
					frontier.put(current, winnerPriority);
				}
			}
			if (population.size() < POPULATION_SIZE)
				population.resize(POPULATION_SIZE);
		}
		// Greedy frontier
		currentPriority = frontier.bestPriority();
		current = frontier.get();
		if (current == goal)
			break;

		for (auto next : field.neighbors(current))
		{
			if (!came_from.count(next))
			{
				frontier.put(next, field.heuristic(next, goal));
				came_from[next] = current;
			}
		}
		closedList.insert(current);
				
		if (maxFSize < frontier.size())
			maxFSize = frontier.size();
	}

	//std::cout << "[] Constructing path...";
	current = goal;
	while (current != start)
	{
		path.add(current.first, current.second);
		current = came_from[current];
	}
	path.add(startX, startY);
	//std::cout << "DONE!\n";

	//std::cout << "LEGAL: " << Chromosome::legal << " ILLEGAL: " << Chromosome::illegal << "\n";
	//std::cout<<(isPatterned?"Was":"Wasn't")<<" patterned\n\n";

	return{ maxFSize * (sizeof(Location) + sizeof(int))
		+ came_from.size() * 2 * sizeof(Location)
		+ closedList.size() * sizeof(Location)
		+ maxSpeciesSize * sizeof(Chromosome),

		path.length()
	};
}

std::pair<GeneticBot::Chromosome, GeneticBot::Chromosome> GeneticBot::crossover(Chromosome parent1, Chromosome parent2)
{
	Chromosome kid1, *first = &kid1, kid2, *second = &kid2;
	int point = rand() % 7 + 1;
	first->dm1 = parent1.dm1;
	second->dm1 = parent2.dm1;
	if (point == 1) std::swap(first, second);
	first->dm2 = parent1.dm2;
	second->dm2 = parent2.dm2;
	if (point == 2) std::swap(first, second);
	first->dm3 = parent1.dm3;
	second->dm3 = parent2.dm3;
	if (point == 3) std::swap(first, second);
	first->dm4 = parent1.dm4;
	second->dm4 = parent2.dm4;
	if (point == 4) std::swap(first, second);
	first->m1 = parent1.m1;
	second->m1 = parent2.m1;
	if (point == 5) std::swap(first, second);
	first->m2 = parent1.m2;
	second->m2 = parent2.m2;
	if (point == 6) std::swap(first, second);
	first->m3 = parent1.m3;
	second->m3 = parent2.m3;
	if (point == 7) std::swap(first, second);
	first->m4 = parent1.m4;
	second->m4 = parent2.m4;

	/*std::cout << "Crossover [at"<<point<<"]: \n\t";
	show(parent1); std::cout << " and "; show(parent2); std::cout << "\n\t";
	std::cout << "gives\n\t";
	show(kid1); std::cout << " and "; show(kid2); std::cout << "\n";*/

	return{ kid1, kid2 };
}

GeneticBot::Chromosome GeneticBot::mutate(Chromosome original)
{
	Chromosome mutant = original;
	if (probability(MUTATION_SEVERITY)) mutant.dm1 = randInt(original.DM_LOW, original.DM_HIGH);
	if (probability(MUTATION_SEVERITY)) mutant.dm2 = randInt(original.DM_LOW, original.DM_HIGH);
	if (probability(MUTATION_SEVERITY)) mutant.dm3 = randInt(original.DM_LOW, original.DM_HIGH);
	if (probability(MUTATION_SEVERITY)) mutant.dm4 = randInt(original.DM_LOW, original.DM_HIGH);
	if (probability(MUTATION_SEVERITY)) mutant.m1 = randInt(-1, 1);
	if (probability(MUTATION_SEVERITY)) mutant.m2 = randInt(-1, 1);
	if (probability(MUTATION_SEVERITY)) mutant.m3 = randInt(-1, 1);
	if (probability(MUTATION_SEVERITY)) mutant.m4 = randInt(-1, 1);
	
	/*std::cout << "Mutation: \n\t";
	show(original); std::cout << " ==> "; show(mutant); std::cout << "\n";*/

	return mutant;
}

void GeneticBot::show(const Chromosome& c)
{
	std::cout <<
		"|" << std::setw(4) << c.dm1 <<
		"|" << std::setw(4) << c.dm2 <<
		"|" << std::setw(4) << c.dm3 <<
		"|" << std::setw(4) << c.dm4 <<
		"|  [" << std::setw(2) << c.m1 <<
		"][" << std::setw(2) << c.m2 <<
		"][" << std::setw(2) << c.m3 <<
		"][" << std::setw(2) << c.m4 <<
		"]";
}

void GeneticBot::removeIllegal(std::vector<Chromosome>& v, Location current)
{
	v.erase(
		std::remove_if(v.begin(), v.end(), [=](Chromosome c) { return !c.isLegal() || finish(c, current) == current; }),
		v.end()
	);
}

void GeneticBot::Chromosome::makeLegal()
{
	//no moves
	if (m1 == 0 && m2 == 0 && m3 == 0 && m4 == 0) m2 = 1;

	//superimposed
	if (m2 == 0 && m1!=0 && m3!=0) m1 = 0;
	if (m3 == 0 && m2!=0 && m4!=0) m2 = 0;
}

bool GeneticBot::Chromosome::isLegal()
{
	//no moves
	if (m1 == 0 && m2 == 0 && m3 == 0 && m4 == 0)
		return false;

	//superimposed
	if (m2 == 0 && m1*m3 == -1) return false;
	if (m3 == 0 && m2*m4 == -1) return false;

	return true;
}

int GeneticBot::Chromosome::legal = 0;
int GeneticBot::Chromosome::illegal = 0;

void GeneticBot::Chromosome::adapt(const Field & field, const Location & location, const std::unordered_set<Location, LocationHasher>& closedList)
{
	makeLegal();
	
	Chromosome buffer = *this;

	int x = location.first, y = location.second, dist;

	//std::cout << "ADAPT \t"; GeneticBot::show(*this); std::cout << "\t(" << x << ";" << y << ") up=" << field.upToObstacle(x, y) << " down=" << field.downToObstacle(x, y) << " right=" << field.rightToObstacle(x, y) << " left=" << field.leftToObstacle(x, y) <<"\n\t";
	
	/* 
	 * if loop
	 * delete looping subpath
	 */
	if (m1*m3 == -1 && m2*m4 == -1 && dm3 <= dm1 && dm2 <= dm4)
	{
		m2 = m3 = 0;
		dm1 -= dm3;
		dm4 -= dm2;
	}

	/* 
	 * if passes through illegal cells
	 */
	if (m1) for (int i = 1; i <= std::abs(moved1()); ++i) if (closedList.count({ x,y + i*m1 })) { if (i>1) dm1 = i - 1; else m1 = 0; break; }
	dist = field.downToObstacle(x, y); if (m1 == 1 && moved1() >= dist) { if (dist > 1) dm1 = dist - 1; else m1 = 0; }
	dist = field.upToObstacle(x, y); if (m1 == -1 && -moved1() >= dist) { if (dist > 1) dm1 = dist - 1; else m1 = 0; }
	if (m1) y += moved1();
	//GeneticBot::show(*this); std::cout << "moved1 = "<<moved1()<<"\n\t(" << x << ";" << y << ") up=" << field.upToObstacle(x, y) << " down=" << field.downToObstacle(x, y) << " right=" << field.rightToObstacle(x, y) << " left=" << field.leftToObstacle(x, y) << "\n\t";

	if (m2) for (int i = 1; i <= std::abs(moved2()); ++i) if (closedList.count({ x + i*m2,y })) { if (i>1) dm2 = i - 1; else m2 = 0; break; }
	dist = field.rightToObstacle(x, y); if (m2 == 1 && moved2() >= dist) { if (dist > 1) dm2 = dist - 1; else m2 = 0; }
	dist = field.leftToObstacle(x, y); if (m2 == -1 && -moved2() >= dist) { if (dist > 1) dm2 = dist - 1; else m2 = 0; }
	if (m2) x += moved2();
	//GeneticBot::show(*this); std::cout << "moved2 = " << moved2() << "\n\t(" << x << ";" << y << ") up=" << field.upToObstacle(x, y) << " down=" << field.downToObstacle(x, y) << " right=" << field.rightToObstacle(x, y) << " left=" << field.leftToObstacle(x, y) << "\n\t";

	if (m3) for (int i = 1; i <= std::abs(moved3()); ++i) if (closedList.count({ x,y + i*m3 })) { if (i>1) dm3 = i - 1; else m3 = 0;  break; }
	dist = field.downToObstacle(x, y); if (m3 == 1 && moved3() >= dist) { if (dist > 1) dm3 = dist - 1; else m3 = 0; }
	dist = field.upToObstacle(x, y); if (m3 == -1 && -moved3() >= dist) { if (dist > 1) dm3 = dist - 1; else m3 = 0; }
	if (m3) y += moved3();
	//GeneticBot::show(*this); std::cout << "moved3 = " << moved3() << "\n\t(" << x << ";" << y << ") up=" << field.upToObstacle(x, y) << " down=" << field.downToObstacle(x, y) << " right=" << field.rightToObstacle(x, y) << " left=" << field.leftToObstacle(x, y) << "\n\t";

	if (m4) for (int i = 1; i <= std::abs(moved4()); ++i) if (closedList.count({ x + i*m4,y })) { if (i>1) dm4 = i - 1; else m4 = 0;  break; }
	dist = field.rightToObstacle(x, y); if (m4 == 1 && moved4() >= dist) { if (dist > 1) dm4 = dist - 1; else m4 = 0; }
	dist = field.leftToObstacle(x, y); if (m4 == -1 && -moved4() >= dist) { if (dist > 1) dm4 = dist - 1; else m4 = 0; }
	if (m4) x += moved4();
	//GeneticBot::show(*this); std::cout << "moved4 = " << moved4() << "\t(" << x << ";" << y << ") up=" << field.upToObstacle(x, y) << " down=" << field.downToObstacle(x, y) << " right=" << field.rightToObstacle(x, y) << " left=" << field.leftToObstacle(x, y) << "\n";

	(isLegal()?legal:illegal)++;
}

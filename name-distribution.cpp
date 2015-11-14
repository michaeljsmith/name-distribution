#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>
#include <cstdlib>

using namespace std;

struct Person {
  Person(int name_): name(name_) {}
  int name;
};

const int INITIAL_NAME_COUNT = 1000;
const int INITIAL_PEOPLE_PER_NAME = 100;
const int CHILDREN_PER_COUPLE = 2;
const int GENERATIONS = 10000;
const vector<double> PERCENTILES_TO_PRINT {10, 50, 90, 99}; 

vector<Person> population;

void reportGenerationStats(int generation) {
  vector<int> names(population.size());
  transform(population.begin(), population.end(), names.begin(), [](Person const& person) {
    return person.name;
  });
  sort(names.begin(), names.end());

  vector<int> frequencies;
  if (names.size() > 0) {
    int count = 1;
    int lastName = names[0];
    for (size_t i = 1; i < names.size(); ++i) {
      if (names[i] == lastName) {
        ++count;
      } else {
        frequencies.push_back(count);
        count = 1;
        lastName = names[i];
      }
    }
  }

  sort(frequencies.begin(), frequencies.end());
  reverse(frequencies.begin(), frequencies.end());

  int numNames = frequencies.size();
  cout << generation << ":  ";
  cout << "Name count: " << numNames << "  ";

  int cumulative = 0;
  int percentileIndex = 0;
  for (size_t i = 0; i < frequencies.size() &&
      percentileIndex < PERCENTILES_TO_PRINT.size(); ++i) {
    cumulative += frequencies[i];
    while (percentileIndex < PERCENTILES_TO_PRINT.size()) {
      double threshold = PERCENTILES_TO_PRINT[percentileIndex] * population.size() / 100.0;
      if (cumulative < threshold) {
        break;
      }

      cout << PERCENTILES_TO_PRINT[percentileIndex] << "%=" << i + 1 << "  ";
      ++percentileIndex;
    }
  }
  for (; percentileIndex < PERCENTILES_TO_PRINT.size(); ++percentileIndex) {
    cout << PERCENTILES_TO_PRINT[percentileIndex] << "%=" << frequencies.size() << "  ";
  }
  cout << "\n";
}

void initPopulation() {
  population.clear();
  for (int name = 0; name < INITIAL_NAME_COUNT; ++name) {
    for (int personWithNameIndex = 0;
        personWithNameIndex < INITIAL_PEOPLE_PER_NAME;
        ++personWithNameIndex) {
      population.push_back(Person(name));
    }
  }
    reportGenerationStats(-1);
}

void advanceGeneration() {
  vector<Person> nextGeneration;
  vector<int> eligiblePopulation(population.size());
  for (size_t i = 0; i < eligiblePopulation.size(); ++i) {
    eligiblePopulation[i] = i;
  }

  random_shuffle(eligiblePopulation.begin(), eligiblePopulation.end());
  while (eligiblePopulation.size() >= 2) {
    Person const& partner0 = population[eligiblePopulation.back()];
    eligiblePopulation.pop_back();

    Person const& partner1 = population[eligiblePopulation.back()];
    eligiblePopulation.pop_back();

    for (int childIndex = 0; childIndex < CHILDREN_PER_COUPLE; ++childIndex) {
      nextGeneration.push_back(Person(partner0.name));
    }
  }

  population = move(nextGeneration);
}

int main(int /*argc*/, char* /*argv*/[]) {
  initPopulation();

  for (int generation = 0; generation < GENERATIONS; ++generation) {
    reportGenerationStats(generation);
    advanceGeneration();
  }

  reportGenerationStats(GENERATIONS);
}


// Sohail Bangi, Royce Lim, Preston Powell, Khang Truong
// Dr. Mark Grinshpon
// Probability & Statistics for Computer Scientists
// 25 April 2022

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <sstream>

#define getRandomElement(x) (x[rand() % x.size()])

using namespace std;

// Simulation variables
int INITIAL_INFECT_COUNT = 1;
int COMPUTER_COUNT = 20;
int FIX_COUNT = 5;
int TEST_COUNT = 50000;
float INFECT_PROBABILITY = .1;

// Computer state
const bool INFECTED = 1;
const bool UNINFECTED = 0;

// probs[x] = probability one will get infect if there are x infect attack it.
vector<float> probs;


// probs[x] = probability one will get infect if there are x infect attack it.
bool bernoulli(float p)
{
    return (float)rand() / RAND_MAX < p;
}


// We use vector instead of array so that sizes can be changed, can store multiple objects, and easier when remove or add elements.
void monteCarloSimulation(float& aCnt, float& bCnt, float& cCnt)
{
  // Array for holding computer state, initially set all to 0 (UNINFECTED)
  vector<bool> states(COMPUTER_COUNT, UNINFECTED);

  // Array for checking wether one GET infect or USED TO get infect
  vector<bool> wasInfected(COMPUTER_COUNT, false);

  // Get all computer indexes with desirable state
  auto getStateIndexes = [&states](bool state) -> vector<int>
  {
    vector<int> result;

    for (int i = 0; i < states.size(); i++)
    {
      if (states[i] == state)
      {
        result.push_back(i);
      }
    }

    return result;
  };

  // Counting computer with desirable state
  auto getStateSize = [&states](bool state) -> int
  {
    return count(states.begin(), states.end(), state);
  };

  // Random to choose initial infect case
  for (int infectCnt = INITIAL_INFECT_COUNT; infectCnt--;)
  {
    auto uninfectIndexes = getStateIndexes(UNINFECTED);
    states[getRandomElement(uninfectIndexes)] = INFECTED;
  }

  bool RUNNING = true;

  // Count number of day in the simulation
  int day_count = 0;
  // Sum of getStateSize(INFECTED) from all days in the simulation
  float infect_count = 0;

  do
  {
    day_count++;

    // Begin spreading phase
    // cache prob(probability one will get infect if there are n other infect attack it)
    // in this case n = getStateSize(INFECTED), current infected count
    float prob = probs[getStateSize(INFECTED)];
    for (auto uninfectIndex : getStateIndexes(UNINFECTED))
    {
      // If bernoulli trial is sucess the uninfect become infected.
      if (bernoulli(prob))
      {
        states[uninfectIndex] = INFECTED;
      }
    }

      // Update infect_count
      infect_count += getStateSize(INFECTED);

      // Update wasInfected by looping throw all infect cases
      for (auto infectIndex : getStateIndexes(INFECTED))
      {
        wasInfected[infectIndex] = true;
      }

      for (int fixCnt = FIX_COUNT; fixCnt--;)
      {
        auto infectIndexes = getStateIndexes(INFECTED);

        if (infectIndexes.empty())
        {
          RUNNING = false;
          break;
        }

        states[getRandomElement(infectIndexes)] = UNINFECTED;
      }

  } while (RUNNING);
  
  // aCnt will be increased by day
  // bCnt will be increase if all computers in the Vector (same purpose as Array) get infected at least once
  // cCnt will be increase right after the morning rountine, to count the number of computer get infected
  aCnt += day_count;
  bCnt += count(wasInfected.begin(), wasInfected.end(), true) == COMPUTER_COUNT;
  cCnt += infect_count / day_count;
}


// Run time fucntion to let you know how long the function run to get to the results
void programInit()
{
  srand(time(NULL));

  float p = 1.;
  for (int i = 0; i < COMPUTER_COUNT + 1; i++)
  {
    // Counting probs[x] with the fomula: P(x) = (1 - (1 - p)^x)
    probs.push_back(1. - p);
    p *= 1. - INFECT_PROBABILITY;
  }
}

// return result of an input, and default value if the input is invalid
template<class intput_t>
intput_t getInput(string name, intput_t defaultValue)
{
  cout << "Input data for " << name << " (default = " << defaultValue << "): ";
  string input;
  getline(cin, input);
  stringstream ss(input);
  intput_t result;
  if (ss >> result)
    return result;
  return defaultValue;
}

void programInput()
{
  TEST_COUNT = getInput<int  >("TEST_COUNT", TEST_COUNT);
  INFECT_PROBABILITY = getInput<float>("INFECT_PROBABILITY", INFECT_PROBABILITY);
  INITIAL_INFECT_COUNT = getInput<int  >("INITIAL_INFECT_COUNT", INITIAL_INFECT_COUNT);
  COMPUTER_COUNT = getInput<int  >("COMPUTER_COUNT", COMPUTER_COUNT);
  FIX_COUNT = getInput<int  >("FIX_COUNT", FIX_COUNT);
}


// Print out Data that user input
void printData()
{
  printf("Data:\n");
  printf("    TEST_COUNT = %d\n", TEST_COUNT);
  printf("    INFECT_PROBABILITY = %f\n", INFECT_PROBABILITY);
  printf("    INITIAL_INFECT_COUNT = %d\n", INITIAL_INFECT_COUNT);
  printf("    COMPUTER_COUNT = %d\n", COMPUTER_COUNT);
  printf("    FIX_COUNT = %d\n", FIX_COUNT);
}

void runSimulation(float& resultA, float& resultB, float& resultC)
{
  float aCnt = 0, bCnt = 0, cCnt = 0;

  for (int itest = TEST_COUNT; itest--;)
  {
    monteCarloSimulation(aCnt, bCnt, cCnt);
  }

  // Result A is the average number of days 
    // to clean up the whole network in total number of trials
  // By taking the aCnt (a Counter) where
    //it count number of days take to clean up network over number of trials
  resultA = aCnt / TEST_COUNT;

  // Result B is the probability of each computer get infected at least once
  resultB = bCnt / TEST_COUNT;

  // Result C is the average number of computer that get infected
  resultC = cCnt / TEST_COUNT;
}

void printResults(float resultA, float resultB, float resultC)
{
  printf("Results:\n");
  printf("    A) %f\n", resultA);
  printf("    B) %f\n", resultB);
  printf("    C) %f\n", resultC);
}

int main()
{
  programInit();
  programInput();
  printf("\n");
  printData();

  printf("\n");
  printf("Running simulation...\n");
  clock_t start = clock();
  float resultA, resultB, resultC;
  runSimulation(resultA, resultB, resultC);
  printf("Time elapsed: %f seconds\n", float(clock() - start) / CLOCKS_PER_SEC);

  printf("\n");
  printResults(resultA, resultB, resultC);
}
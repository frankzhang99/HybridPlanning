
#ifndef ADAPTATIONPLANNER_H_
#define ADAPTATIONPLANNER_H_

#include <string>
#include <vector>
#include <set>
//using namespace std;

extern std::vector<std::string> getActions(const char* adversaryPath, const char* labelsPath,
        std::set<int>& states);
extern std::set<int> getNowStates(const char* statesPath);

class AdaptationPlanner {
protected:
    std::string modelTemplatePath;

    bool generateModel(std::string environmentModel, std::string initialState, const char* modelPath, bool returnPlan);

public:
	AdaptationPlanner();

	/**
	 * Returns a vector of tactics that must be started now
	 * TODO the args should be a probability tree and a key-value map for the state
	 *
	 * @param path if not null, the temp directory path (relative) is stored there, and the
	 *   directory is not deleted
	 */
	std::vector<std::string> plan(std::string environmentModel, std::string initialState, std::string* path = 0, bool returnPlan = true);

	void setModelTemplatePath(const std::string& modelTemplatePath);

	virtual ~AdaptationPlanner();
	void test();
};

#endif /* ADAPTATIONPLANNER_H_ */

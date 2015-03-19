#pragma once

#include <vector>

using namespace std;

class PairwiseComparison {
protected:
	PairwiseComparison() {}

public:
	static vector<float> computePreferences(vector<pair<int, pair<vector<float>, vector<float> > > >& comparisons, int maxIterations);
	static float dot(vector<float> w, vector<float> f);

private:
	static void gradientDescent(vector<float>& w, vector<pair<int, pair<vector<float>, vector<float> > > >& comparisons, int maxIterations, bool l1, float lambda, float eta, float threshold, bool normalize = true);
	static float negativeLogLikelihood(vector<pair<int, pair<vector<float>, vector<float> > > >& comparisons, vector<float> w, bool l1, float lambda);
};


/**
 * Pairwise comparisonデータに基づき、ユーザのpreferenceポイントを求める。
 * ユーザのpreferenceはベクトルで表現され、スコアは、このベクトルと各アイテムのfeatureベクトル
 * の内積で決定される。
 *
 * @author	Gen Nishdia
 * @date	3/23/2015
 * @version	1.0
 */

#pragma once

#include <vector>

using namespace std;

class PairwiseComparison {
protected:
	PairwiseComparison() {}

public:
	static vector<float> computePreferences(vector<pair<int, pair<vector<float>, vector<float> > > >& comparisons, int maxIterations, bool l1, float lambda, float eta, float threshold);
	static float score(const vector<float>& w, const vector<float>& f);

private:
	static void gradientDescent(vector<float>& w, vector<pair<int, pair<vector<float>, vector<float> > > >& comparisons, int maxIterations, bool l1, float lambda, float eta, float threshold, bool normalize = true);
	static float negativeLogLikelihood(vector<pair<int, pair<vector<float>, vector<float> > > >& comparisons, const vector<float>& w, bool l1, float lambda);
	static float dot(const vector<float>& w, const vector<float>& f);
};


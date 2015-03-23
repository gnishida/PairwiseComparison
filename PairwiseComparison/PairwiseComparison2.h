/**
 * Pairwise comparisonデータに基づき、ユーザのpreferenceポイントを求める。
 * ベクトルではない！
 * スコアは、このポイントから、各アイテムのfeatureベクトルへの距離で決定する。
 * Active rankingの論文では、この手法を用いているため、ためしに実装してみた。
 *
 * @author	Gen Nishdia
 * @date	3/23/2015
 * @version	1.0
 */

#pragma once

#include <vector>

using namespace std;

class PairwiseComparison2 {
protected:
	PairwiseComparison2() {}

public:
	static vector<float> computePreferences(vector<pair<int, pair<vector<float>, vector<float> > > >& comparisons, int maxIterations, bool l1, float lambda, float eta, float threshold);
	static float score(const vector<float>& w, const vector<float>& f);

private:
	static void gradientDescent(vector<float>& w, vector<pair<int, pair<vector<float>, vector<float> > > >& comparisons, int maxIterations, bool l1, float lambda, float eta, float threshold, bool normalize = true);
	static float negativeLogLikelihood(vector<pair<int, pair<vector<float>, vector<float> > > >& comparisons, vector<float> w, bool l1, float lambda);
	static float distance(vector<float> w, vector<float> f);
	static float dot(vector<float> w, vector<float> f);
};

